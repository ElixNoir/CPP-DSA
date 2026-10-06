#pragma once

#include "DSA.hpp"
#include "IncrementalContainer.hpp"
#include "StackIterator.hpp"

namespace DSA {

	template <concepts::Container CONTAINER>
	class Stack : public IncrementalContainer<CONTAINER> {
	public:

		using Base = IncrementalContainer<CONTAINER>;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

		using Base::data;
		using Base::size;

	public:

#pragma region Constructors & Destructors

		using Base::Base;

		~Stack() requires (!std::is_trivially_destructible_v<T>) {
			destroy_range_backward(Base::get_data(), Base::get_data() + size);
		}

		~Stack() = default;

#pragma endregion

#pragma region Methods

#pragma region IncrementalContainer

		constexpr void add(const T& value) {
			construct_at(Base::get_data() + size++, value);
		}

		constexpr void add(const T&& value) {
			construct_at(Base::get_data() + size++, move(value));
		}

		template <typename... Arguments>
		constexpr void emplace(Arguments&&... arguments) {
			construct_at<T>(Base::get_data() + size++, forward<Arguments>(arguments)...);
		}

		constexpr void empty() noexcept {
			destroy_range_backward(Base::get_data(), Base::get_data() + size);
			size = 0;
		}

		constexpr void remove() noexcept {
			size--;
			destroy_at(Base::get_data() + size);
		}

		constexpr void remove(INDEX count) noexcept {
			size -= count;
			destroy_range_backward(Base::get_data(), Base::get_data() + size);
		}

#pragma endregion

#pragma region Stack

		constexpr bool can_peek() const noexcept {
			return !Base::is_empty();
		}

		constexpr T& peek() noexcept {
			return Base::get_data()[size - 1];
		}

		constexpr const T& peek() const noexcept {
			return Base::get_data()[size - 1];
		}

		constexpr bool can_pop(INDEX count = 1) const noexcept {
			return Base::can_remove(count);
		}

		constexpr T pop() noexcept {
			T* const address = Base::get_data() + --size;
			T value = move(*address);
			destroy_at(address);
			return value;
		}

		constexpr bool can_push(INDEX count = 1) const noexcept {
			return Base::can_add(count);
		}

		constexpr void push(const T& value) {
			add(value);
		}

		constexpr void push(const T&& value) {
			add(value);
		}

#pragma region Memory Management

		void double_capacity() requires
			concepts::DynamicContainer<CONTAINER>
			&& (!concepts::ResizableContainer<CONTAINER>)
		{
			grow(Base::get_capacity() << 1);
		}

		void double_capacity() requires concepts::ResizableContainer<CONTAINER> {
			Base::double_capacity();
		}

		void grow(INDEX newCapacity) requires
			concepts::DynamicContainer<CONTAINER>
			&& (!concepts::ResizableContainer<CONTAINER>)
		{
			using ALLOCATOR = CONTAINER::ALLOCATOR;

			T* oldData = Base::get_data();
			data = reinterpret_cast<std::byte*>(ALLOCATOR::allocate(newCapacity));
			move_construct_range_backward(Base::get_data(), oldData, size);
			destroy_range_backward(oldData, oldData + size);
			ALLOCATOR::deallocate(oldData);

			Base::capacity = newCapacity;
		}

		void grow(INDEX newCapacity) requires concepts::ResizableContainer<CONTAINER> {
			Base::grow(newCapacity);
		}

		void reserve(INDEX newCapacity) requires
			concepts::DynamicContainer<CONTAINER>
			&& (!concepts::ResizableContainer<CONTAINER>)
		{
			if (newCapacity > Base::get_capacity())
				resize(newCapacity);
		}

		void reserve(INDEX newCapacity) requires concepts::ResizableContainer<CONTAINER> {
			Base::reserve(newCapacity);
		}

		void resize(INDEX newCapacity) requires
			concepts::DynamicContainer<CONTAINER>
			&& (!concepts::ResizableContainer<CONTAINER>)
		{
			if (newCapacity > Base::get_capacity())
				grow(newCapacity);
			else
				shrink(newCapacity);
		}

		void resize(INDEX newCapacity) requires concepts::ResizableContainer<CONTAINER> {
			Base::resize(newCapacity);
		}

		void shrink(INDEX newCapacity) requires
			concepts::DynamicContainer<CONTAINER>
			&& (!concepts::ResizableContainer<CONTAINER>)
		{
			using ALLOCATOR = CONTAINER::ALLOCATOR;

			T* oldData = Base::get_data();
			data = reinterpret_cast<std::byte*>(ALLOCATOR::allocate(newCapacity));
			move_construct_range_backward(Base::get_data(), oldData, size < newCapacity ? size : newCapacity);
			destroy_range_backward(oldData, oldData + size);
			ALLOCATOR::deallocate(oldData);

			Base::capacity = newCapacity;
		}

		void shrink(INDEX newCapacity) requires concepts::ResizableContainer<CONTAINER> {
			Base::shrink(newCapacity);
		}

#pragma endregion

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