#pragma once

#include "Collection.hpp"
#include "StackIterator.hpp"

namespace DSA {

	template <concepts::ContainerPolicy ContainerPolicy, typename T>
		requires std::is_nothrow_destructible_v<T>
	class Stack : public Collection<ContainerPolicy, T> {
	public:

		using Base = Collection<ContainerPolicy, T>;
		using typename Base::INDEX;
		using typename Base::T;

		using Self = Stack<ContainerPolicy, T>;

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
			Base::set_capacity(newCapacity);
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		using Base::Base;

		~Stack() noexcept = default;

		~Stack() noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			destroy();
		}

		void destroy() noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
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

		template <concepts::InputIterator<T> ITERATOR>
		constexpr void add_many(ITERATOR sourceIterator, INDEX count)
			noexcept (is_move_iterator_v<ITERATOR>
				? std::is_nothrow_move_constructible_v<T>
				: std::is_nothrow_copy_constructible_v<T>)
		{
			if constexpr (is_move_iterator_v<ITERATOR>) {
				T* const address = Base::get_data() + Base::get_size();
				move_construct_range(address, address + count, sourceIterator);
				Base::size += count;
			}
			else {
				T* const address = Base::get_data() + Base::get_size();
				copy_construct_range(address, address + count, sourceIterator);
				Base::size += count;
			}
			
		}

		template <typename... ARGUMENTS>
		constexpr void emplace(ARGUMENTS&&... arguments)
			noexcept (std::is_nothrow_constructible_v<T, ARGUMENTS...>)
		{
			construct_at<T>(Base::get_data() + Base::get_size(), std::forward<ARGUMENTS>(arguments)...);
			++Base::size;
		}

		constexpr void remove() noexcept {
			destroy_at(Base::get_data() + --Base::size);
		}

		constexpr void remove_many(INDEX count) noexcept {
			T* const address = Base::get_data() + Base::get_size();
			destroy_range<Direction::Backward>(address - count, address);
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
			move_construct_range<Direction::Backward>(destination, destination + count, address);
			destroy_range<Direction::Backward>(address, address + count);
			Base::size -= count;
		}

		[[nodiscard]] constexpr bool can_push(INDEX count = 1) const noexcept {
			return Base::can_add(count);
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

		template <concepts::InputIterator<T> ITERATOR>
		constexpr void push_many(ITERATOR sourceIterator, INDEX count)
			noexcept (noexcept(add_many(sourceIterator, count)))
		{
			add_many(sourceIterator, count);
		}

#pragma endregion

#pragma region Memory Management

		void double_capacity()
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::get_capacity();
			grow(capacity << 1);
		}

		constexpr void empty() noexcept {
			destroy();
			Base::set_size(0);
		}

		void expand(float ratio = 1.5)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::get_capacity();
			helper_resize<true>(Math::ceiling(capacity * ratio));
		}

		void grow(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			helper_resize<true>(newCapacity);
		}

		void reserve(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity > capacity)
				grow(newCapacity);
		}

		void resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity > capacity)
				grow(newCapacity);
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

		constexpr StackIterator<T> begin() noexcept {
			return StackIterator<T>(Base::get_data());
		}

		constexpr StackIterator<const T> begin() const noexcept {
			return StackIterator<const T>(Base::get_data());
		}

		constexpr StackIterator<T> end() noexcept {
			return StackIterator<T>(Base::get_data() + Base::get_size());
		}

		constexpr StackIterator<const T> end() const noexcept {
			return StackIterator<const T>(Base::get_data() + Base::get_size());
		}

		constexpr StackIterator<T> iterator_at(INDEX index) noexcept {
			return StackIterator<T>(Base::get_data() + index);
		}

		constexpr StackIterator<const T> iterator_at(INDEX index) const noexcept {
			return StackIterator<const T>(Base::get_data() + index);
		}

#pragma endregion

#pragma endregion

	};

}