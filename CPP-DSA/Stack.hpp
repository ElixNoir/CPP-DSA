#pragma once

#include "Collection.hpp"
#include "StackIterator.hpp"

namespace DSA {

	template <concepts::StoragePolicy StoragePolicy, typename T>
		requires std::is_nothrow_destructible_v<T>
	class Stack : public Collection<StoragePolicy, T> {
	public:

		using Base = Collection<StoragePolicy, T>;
		using typename Base::INDEX;
		using typename Base::T;

		constexpr static bool has_capacity = Base::has_capacity;

	protected:

#pragma region Methods

		void helper_destroy() noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			const T* data = Base::get_data();
			destroy_range_backward(data, data + Base::get_size());
		}

		template <bool GROW>
		void helper_resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			using ALLOCATOR = Base::ALLOCATOR;

			T* const oldData = Base::get_data();
			T* const newData = ALLOCATOR::allocate(newCapacity);

			try {
				if constexpr (GROW)
					reconstruct_range_backward(newData, newData + Base::get_size(), oldData);
				else {
					reconstruct_range_backward(newData, newData + newCapacity, oldData);
					destroy_range_backward(oldData + newCapacity, oldData + Base::get_size());
					Base::set_size(newCapacity);
				}
			}
			catch (...) {
				ALLOCATOR::deallocate(newData);
				throw;
			}

			ALLOCATOR::deallocate(oldData);

			Base::set_data(newData);
			if constexpr (concepts::DynamicContainer<Base>)
				Base::set_capacity(newCapacity);
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		using Base::Base;

		~Stack() noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			destroy();
		}

		~Stack() noexcept = default;

		void destroy() noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			helper_destroy();
		}

#pragma endregion

#pragma region Methods

#pragma region Add & Remove

		constexpr void add(const T& data) 
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			copy_construct_at(Base::get_data() + Base::get_size(), data);
			++Base::size;
		}

		constexpr void add(T&& data) 
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			move_construct_at(Base::get_data() + Base::get_size(), std::move(data));
			++Base::size;
		}

		constexpr void add_many(const T* source, INDEX count) 
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			T* const address = Base::get_data() + Base::get_size();
			copy_construct_range_backward(address, address + count, source);
			Base::size += count;
		}

		constexpr void add_many(T* source, INDEX count) 
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T* const address = Base::get_data() + Base::get_size();
			move_construct_range_backward(address, address + count, source);
			Base::size += count;
		}

		template <typename... Arguments>
		constexpr void emplace(Arguments&&... arguments)
			noexcept (std::is_nothrow_constructible_v<T, Arguments...>)
		{
			construct_at<T>(Base::get_data() + Base::get_size(), forward<Arguments>(arguments)...);
			++Base::size;
		}

		constexpr void remove() noexcept {
			destroy_at(Base::get_data() + --Base::size);
		}

		constexpr void remove_many(INDEX count) noexcept {
			T* const address = Base::get_data() + Base::get_size();
			destroy_range_backward(address - count, address);
			Base::size -= count;
		}

#pragma endregion

#pragma region Peek, Pop, & Push

		[[nodiscard]] constexpr bool can_peek() const noexcept {
			return !Base::is_empty();
		}

		[[nodiscard]] constexpr T& peek() noexcept {
			return Base::get(Base::get_size() - 1);
		}

		[[nodiscard]] constexpr const T& peek() const noexcept {
			return Base::get(Base::get_size() - 1);
		}

		[[nodiscard]] constexpr T* peek(INDEX count) noexcept {
			return Base::get_data() + Base::get_size() - count;
		}

		[[nodiscard]] constexpr const T* peek(INDEX count) const noexcept {
			return Base::get_data() + Base::get_size() - count;
		}

		[[nodiscard]] constexpr bool can_pop(INDEX count = 1) const noexcept {
			return Base::can_remove(count);
		}

		[[nodiscard]] constexpr T pop() 
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T* const address = Base::get_data() + --Base::size;
			T data = std::move(*address);
			destroy_at(address);
			return data;
		}

		[[nodiscard]] constexpr void pop_many(T* destination, INDEX count) 
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T* const address = Base::get_data() + Base::get_size() - count;
			move_construct_range_backward(destination, destination + count, address);
			destroy_range_backward(address, address + count);
			Base::size -= count;
		}

		[[nodiscard]] constexpr bool can_push(INDEX count = 1, Parameter<INDEX, !has_capacity> explicitCapacity = {}) const noexcept {
			return Base::can_add(count, explicitCapacity);
		}

		constexpr void push(const T& data) 
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			add(data);
		}

		constexpr void push(T&& data) 
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			add(std::move(data));
		}

		constexpr void push_copy_construct_many(const T* source, INDEX count) 
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			add_copy_construct_many(source, count);
		}

		constexpr void push_move_construct_many(const T* source, INDEX count) 
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			add_move_construct_many(source, count);
		}

#pragma endregion

#pragma region Memory Management

		void double_capacity(Parameter<INDEX, !has_capacity> explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			grow(Base::extract_capacity(explicitCapacity) << 1);
		}

		constexpr void empty() noexcept {
			destroy();
			Base::set_size(0);
		}

		void grow(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			helper_resize<true>(newCapacity);
		}

		void reserve(INDEX newCapacity, Parameter<INDEX, !has_capacity> explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			if (newCapacity > capacity)
				grow(newCapacity, capacity);
		}

		void resize(INDEX newCapacity, Parameter<INDEX, !has_capacity> explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			if (newCapacity > capacity)
				grow(newCapacity, capacity);
			else
				shrink(newCapacity);
		}

		void shrink(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			helper_resize<false>(newCapacity);
		}

#pragma endregion

#pragma region Iteration

		StackIterator<T> begin() noexcept {
			return StackIterator<T>(Base::get_data());
		}

		StackIterator<const T> begin() const noexcept {
			return StackIterator<const T>(Base::get_data());
		}

		StackIterator<T> end() noexcept {
			return StackIterator<T>(Base::get_data() + Base::get_size());
		}

		StackIterator<const T> end() const noexcept {
			return StackIterator<const T>(Base::get_data() + Base::get_size());
		}

#pragma endregion

#pragma endregion

	};

}