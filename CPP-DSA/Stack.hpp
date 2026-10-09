#pragma once

#include "IncrementalContainer.hpp"
#include "StackIterator.hpp"

namespace DSA {

	template <concepts::Container CONTAINER>
		requires std::is_nothrow_destructible_v<typename CONTAINER::T>
	class Stack : public IncrementalContainer<CONTAINER> {
	public:

		using Base = IncrementalContainer<CONTAINER>;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

		using Base::data;
		using Base::size;

#pragma region Methods

		void helper_destroy() noexcept requires (
			!std::is_trivially_destructible_v<T>
		) {
			destroy_range_backward(Base::get_data(), Base::get_data() + size);
		}

		template <bool GROW>
		void helper_resize(INDEX newCapacity) noexcept(
			std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
			&& !concepts::ResizableContainer<CONTAINER>
		) {
			using ALLOCATOR = CONTAINER::ALLOCATOR;

			T* const oldData = Base::get_data();
			T* const newData = ALLOCATOR::allocate(newCapacity);

			try {
				if constexpr (GROW)
					move_construct_range_backward(newData, newData + size, oldData);
				else
					move_construct_range_backward(newData, newData + Utilities::minimum(size, newCapacity), oldData);
				destroy_range_backward(oldData, oldData + size);
				ALLOCATOR::deallocate(oldData);
			}
			catch (...) {
				ALLOCATOR::deallocate(Base::get_data());
				data = oldData;
				throw;
			}

			data = reinterpret_cast<std::byte*>(newData);
			Base::capacity = newCapacity;
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		using Base::Base;

		~Stack() noexcept requires (
			!std::is_trivially_destructible_v<T>
		) {
			helper_destroy();
		}

		~Stack() = default;

#pragma endregion

#pragma region Methods

#pragma region Add & Remove

		constexpr void add(const T& value) noexcept(
			std::is_nothrow_copy_constructible_v<T>
		) {
			copy_construct_at(Base::get_data() + size, value);
			size++;
		}

		constexpr void add(T&& value) noexcept(
			std::is_nothrow_move_constructible_v<T>
		) {
			move_construct_at(Base::get_data() + size, move(value));
			size++;
		}

		constexpr void add_copy_construct_many(const T* source, INDEX count) noexcept(
			std::is_nothrow_copy_constructible_v<T>
		) {
			T* const address = Base::get_data() + size;
			copy_construct_range_backward(address, address + count, source);
			size += count;
		}

		constexpr void add_move_construct_many(const T* source, INDEX count) noexcept(
			std::is_nothrow_move_constructible_v<T>
		) {
			T* const address = Base::get_data() + size;
			move_construct_range_backward(address, address + count, source);
			size += count;
		}

		template <typename... Arguments>
		constexpr void emplace(Arguments&&... arguments) noexcept(
			std::is_nothrow_constructible_v<T, Arguments...>
		) {
			construct_at<T>(Base::get_data() + size, forward<Arguments>(arguments)...);
			size++;
		}

		constexpr void empty() noexcept {
			helper_destroy();
			size = 0;
		}

		constexpr void remove() noexcept {
			destroy_at(Base::get_data() + --size);
		}

		constexpr void remove_many(INDEX count) noexcept {
			T* const address = Base::get_data() + size;
			destroy_range_backward(address - count, address);
			size -= count;
		}

#pragma endregion

#pragma region Peek, Pop, & Push

		[[nodiscard]] constexpr bool can_peek() const noexcept {
			return !Base::is_empty();
		}

		[[nodiscard]] constexpr T& peek() noexcept {
			return Base::get_data()[size - 1];
		}

		[[nodiscard]] constexpr const T& peek() const noexcept {
			return Base::get_data()[size - 1];
		}

		[[nodiscard]] constexpr T* peek(INDEX count) noexcept {
			return Base::get_data() + size - count;
		}

		[[nodiscard]] constexpr const T* peek(INDEX count) const noexcept {
			return Base::get_data() + size - count;
		}

		[[nodiscard]] constexpr bool can_pop(INDEX count = 1) const noexcept {
			return Base::can_remove(count);
		}

		[[nodiscard]] constexpr T pop() noexcept(
			std::is_nothrow_move_constructible_v<T>
		) {
			T* const address = Base::get_data() + --size;
			T value = move(*address);
			destroy_at(address);
			return value;
		}

		[[nodiscard]] constexpr void pop_many(T* destination, INDEX count) noexcept(
			std::is_nothrow_move_constructible_v<T>
		) {
			T* const address = Base::get_data() + size - count;
			move_construct_range_backward(destination, destination + count, address);
			destroy_range_backward(address, address + count);
			size -= count;
		}

		[[nodiscard]] constexpr bool can_push(INDEX count = 1) const noexcept {
			return Base::can_add(count);
		}

		constexpr void push(const T& value) noexcept(
			std::is_nothrow_copy_constructible_v<T>
		) {
			add(value);
		}

		constexpr void push(T&& value) noexcept(
			std::is_nothrow_move_constructible_v<T>
		) {
			add(move(value));
		}

		constexpr void push_copy_construct_many(const T* source, INDEX count) noexcept(
			std::is_nothrow_copy_constructible_v<T>
		) {
			add_copy_construct_many(source, count);
		}

		constexpr void push_move_construct_many(const T* source, INDEX count) noexcept(
			std::is_nothrow_move_constructible_v<T>
		) {
			add_move_construct_many(source, count);
		}

#pragma endregion

#pragma region Memory Management

		void double_capacity() noexcept(
			concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
			&& !concepts::ResizableContainer<CONTAINER>
		) {
			grow(Base::get_capacity() << 1);
		}
		using Base::double_capacity;

		void grow(INDEX newCapacity) noexcept(
			concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
			&& !concepts::ResizableContainer<CONTAINER>
		) {
			helper_resize<true>(newCapacity);
		}
		using Base::grow;

		void reserve(INDEX newCapacity) noexcept(
			concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
			&& !concepts::ResizableContainer<CONTAINER>
		) {
			if (newCapacity > Base::get_capacity())
				grow(newCapacity);
		}
		using Base::reserve;

		void resize(INDEX newCapacity) noexcept(
			concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
			&& !concepts::ResizableContainer<CONTAINER>
		) {
			if (newCapacity > Base::get_capacity())
				grow(newCapacity);
			else
				shrink(newCapacity);
		}
		using Base::resize;

		void shrink(INDEX newCapacity) noexcept(
			concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>
		) requires (
			concepts::DynamicContainer<CONTAINER>
			&& !concepts::ResizableContainer<CONTAINER>
		) {
			helper_resize<false>(newCapacity);
		}
		using Base::shrink;

#pragma endregion

#pragma region Iteration

		StackIterator<T> begin() noexcept {
			return StackIterator<T>(Base::get_data());
		}

		StackIterator<const T> begin() const noexcept {
			return StackIterator<const T>(Base::get_data());
		}

		StackIterator<T> end() noexcept {
			return StackIterator<T>(Base::get_data() + size);
		}

		StackIterator<const T> end() const noexcept {
			return StackIterator<const T>(Base::get_data() + size);
		}

#pragma endregion

#pragma endregion

	};

}