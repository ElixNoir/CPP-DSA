#pragma once

#include "IncrementalContainer.hpp"
#include "StackIterator.hpp"

namespace DSA {

	template <typename ContainerConfiguration, typename T>
		requires std::is_nothrow_destructible_v<T>
	class Stack : public IncrementalContainer<ContainerConfiguration, T> {
	public:

		using Base = IncrementalContainer<ContainerConfiguration, T>;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

#pragma region Methods

		void helper_destroy()
			noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			destroy_range_backward(Base::get_data(), Base::get_data() + Base::size);
		}

		template <bool GROW>
		void helper_resize(INDEX newCapacity) noexcept(
			std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<Base>
			&& !concepts::ResizableContainer<Base>
		) {
			using ALLOCATOR = Base::ALLOCATOR;

			T* const oldData = Base::get_data();
			T* const newData = ALLOCATOR::allocate(newCapacity);

			try {
				if constexpr (GROW)
					move_construct_range_backward(newData, newData + Base::size, oldData);
				else
					move_construct_range_backward(newData, newData + Utilities::minimum(Base::size, newCapacity), oldData);
				destroy_range_backward(oldData, oldData + Base::size);
				ALLOCATOR::deallocate(oldData);
			}
			catch (...) {
				ALLOCATOR::deallocate(Base::get_data());
				Base::data = oldData;
				throw;
			}

			Base::data = reinterpret_cast<std::byte*>(newData);
			Base::capacity = newCapacity;
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		using Base::Base;

		~Stack()
			noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			helper_destroy();
		}

		~Stack() noexcept = default;

#pragma endregion

#pragma region Methods

#pragma region Add & Remove

		constexpr void add(const T& value) 
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			copy_construct_at(Base::get_data() + Base::size, value);
			Base::size++;
		}

		constexpr void add(T&& value) 
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			move_construct_at(Base::get_data() + Base::size, std::move(value));
			Base::size++;
		}

		constexpr void add_copy_construct_many(const T* source, INDEX count) 
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			T* const address = Base::get_data() + Base::size;
			copy_construct_range_backward(address, address + count, source);
			Base::size += count;
		}

		constexpr void add_move_construct_many(const T* source, INDEX count) 
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T* const address = Base::get_data() + Base::size;
			move_construct_range_backward(address, address + count, source);
			Base::size += count;
		}

		template <typename... Arguments>
		constexpr void emplace(Arguments&&... arguments) noexcept(
			std::is_nothrow_constructible_v<T, Arguments...>
		) {
			construct_at<T>(Base::get_data() + Base::size, forward<Arguments>(arguments)...);
			Base::size++;
		}

		constexpr void remove() noexcept {
			destroy_at(Base::get_data() + --Base::size);
		}

		constexpr void remove_many(INDEX count) noexcept {
			T* const address = Base::get_data() + Base::size;
			destroy_range_backward(address - count, address);
			Base::size -= count;
		}

#pragma endregion

#pragma region Peek, Pop, & Push

		[[nodiscard]] constexpr bool can_peek() const noexcept {
			return !Base::is_empty();
		}

		[[nodiscard]] constexpr T& peek() noexcept {
			return Base::get_data()[Base::size - 1];
		}

		[[nodiscard]] constexpr const T& peek() const noexcept {
			return Base::get_data()[Base::size - 1];
		}

		[[nodiscard]] constexpr T* peek(INDEX count) noexcept {
			return Base::get_data() + Base::size - count;
		}

		[[nodiscard]] constexpr const T* peek(INDEX count) const noexcept {
			return Base::get_data() + Base::size - count;
		}

		[[nodiscard]] constexpr bool can_pop(INDEX count = 1) const noexcept {
			return Base::can_remove(count);
		}

		[[nodiscard]] constexpr T pop() 
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T* const address = Base::get_data() + --Base::size;
			T value = std::move(*address);
			destroy_at(address);
			return value;
		}

		[[nodiscard]] constexpr void pop_many(T* destination, INDEX count) 
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T* const address = Base::get_data() + Base::size - count;
			move_construct_range_backward(destination, destination + count, address);
			destroy_range_backward(address, address + count);
			Base::size -= count;
		}

		[[nodiscard]] constexpr bool can_push(INDEX count = 1) const noexcept {
			return Base::can_add(count);
		}

		constexpr void push(const T& value) 
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			add(value);
		}

		constexpr void push(T&& value) 
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			add(std::move(value));
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

		void double_capacity()
			noexcept (concepts::NothrowResizableAllocator<Base::ALLOCATOR>
				&& std::is_nothrow_move_constructible_v<T>)
			requires concepts::DynamicContainer<Base>
		{
			if constexpr (concepts::ResizableContainer<Base>)
				Base::double_capacity();
			else
				grow(Base::get_capacity() << 1);
		}

		constexpr void empty() noexcept {
			helper_destroy();
			Base::size = 0;
		}

		void grow(INDEX newCapacity)
			noexcept (concepts::NothrowResizableAllocator<Base::ALLOCATOR>
				&& std::is_nothrow_move_constructible_v<T>)
			requires concepts::DynamicContainer<Base>
		{
			if constexpr (concepts::ResizableContainer<Base>)
				Base::grow(newCapacity);
			else
				helper_resize<true>(newCapacity);
		}

		void reserve(INDEX newCapacity)
			noexcept (concepts::NothrowResizableAllocator<Base::ALLOCATOR>
				&& std::is_nothrow_move_constructible_v<T>)
			requires concepts::DynamicContainer<Base>
		{
			if constexpr (concepts::ResizableContainer<Base>)
				Base::reserve(newCapacity);
			else if (newCapacity > Base::get_capacity())
				grow(newCapacity);
		}

		void resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableAllocator<Base::ALLOCATOR>
				&& std::is_nothrow_move_constructible_v<T>)
			requires concepts::DynamicContainer<Base>
		{
			if constexpr (concepts::ResizableContainer<Base>)
				Base::resize(newCapacity);
			else if (newCapacity > Base::get_capacity())
				grow(newCapacity);
			else
				shrink(newCapacity);
		}

		void shrink(INDEX newCapacity)
			noexcept (concepts::NothrowResizableAllocator<Base::ALLOCATOR>
				&& std::is_nothrow_move_constructible_v<T>)
			requires concepts::DynamicContainer<Base>
		{
			if constexpr (concepts::ResizableContainer<Base>)
				Base::shrink(newCapacity);
			else
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
			return StackIterator<T>(Base::get_data() + Base::size);
		}

		StackIterator<const T> end() const noexcept {
			return StackIterator<const T>(Base::get_data() + Base::size);
		}

#pragma endregion

#pragma endregion

	};

}