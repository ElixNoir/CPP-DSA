#pragma once

#include "TrackedContainer.hpp"
#include "Bitset.hpp"

namespace DSA {

	template <concepts::ContainerPolicy ContainerPolicy, typename T>
		requires std::is_nothrow_destructible_v<T>
	class SparseDeque : public TrackedContainer<ContainerPolicy, T> {
	public:

		using Base = TrackedContainer<ContainerPolicy, T>;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

#pragma region Properties

		INDEX back = 0;
		INDEX front = 0;
		INDEX span = 0;

#pragma endregion

#pragma region Methods

		[[nodiscard]] constexpr INDEX next_index(INDEX index) const noexcept {
			const INDEX capacity = Base::get_capacity();
			return (static_cast<INDEX>(index) + 1 == capacity)
				? INDEX{ 0 }
				: static_cast<INDEX>(index + 1);
		}

		[[nodiscard]] constexpr INDEX previous_index(INDEX index) const noexcept {
			const INDEX capacity = Base::get_capacity();
			return index == 0
				? static_cast<INDEX>(capacity - 1)
				: static_cast<INDEX>(index - 1);
		}

		[[nodiscard]] constexpr INDEX distance(INDEX from, INDEX to) const noexcept {
			const INDEX capacity = Base::get_capacity();
			return from < to ? to - from : capacity - from + to;
		}

		constexpr void normalize_front() noexcept {
			if (Base::get_size() == 0) {
				span = 0;
				front = back;
				return;
			}

			if (Base::is_tracking(front))
				return;

			const INDEX capacity = Base::get_capacity();
			const INDEX next = Base::find_first_tracked_at_or_after(next_index(front));
			if (next == capacity) {
				span = 0;
				Base::set_size(0);
				front = back;
				return;
			}

			const INDEX skipped = distance(front, next);
			front = next;
			span -= skipped;
		}

		constexpr void normalize_back() noexcept {
			if (Base::get_size() == 0) {
				span = 0;
				front = back;
				return;
			}

			const INDEX capacity = Base::get_capacity();
			const INDEX last = previous_index(back);
			const INDEX previous = Base::find_last_tracked_at_or_before(last);
			if (previous == capacity) {
				span = 0;
				Base::set_size(0);
				front = back;
				return;
			}

			const INDEX skipped = distance(next_index(previous), back);
			back = next_index(previous);
			span -= skipped;
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		using Base::Base;

#pragma endregion

#pragma region Methods

#pragma region Getters

		[[nodiscard]] constexpr INDEX get_back() const noexcept {
			return back;
		}

		[[nodiscard]] constexpr INDEX get_front() const noexcept {
			return front;
		}

		[[nodiscard]] constexpr INDEX get_span() const noexcept {
			return span;
		}

#pragma endregion

#pragma region Checks

		[[nodiscard]] constexpr bool is_full() const noexcept {
			const INDEX capacity = Base::get_capacity();
			return span == capacity;
		}

#pragma endregion

#pragma region Add & Remove

		[[nodiscard]] constexpr INDEX add_back(const T& data)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			const INDEX index = back;

			copy_construct_at(Base::get_data() + index, data);
			Base::track(index);

			++Base::size;
			++span;
			back = next_index(back);

			return index;
		}

		[[nodiscard]] constexpr INDEX add_back(T&& data)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX index = back;

			move_construct_at(Base::get_data() + index, std::move(data));
			Base::track(index);

			++Base::size;
			++span;
			back = next_index(back);

			return index;
		}

		[[nodiscard]] constexpr INDEX add_front(const T& data)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			const INDEX newFront = previous_index(front);

			copy_construct_at(Base::get_data() + newFront, data);
			Base::track(newFront);

			front = newFront;
			++Base::size;
			++span;

			return newFront;
		}

		[[nodiscard]] constexpr INDEX add_front(T&& data)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX newFront = previous_index(front);

			move_construct_at(Base::get_data() + newFront, std::move(data));
			Base::track(newFront);

			front = newFront;
			++Base::size;
			++span;

			return newFront;
		}

		constexpr void remove(INDEX index) noexcept {
			destroy_at(Base::get_data() + index);
			Base::untrack(index);

			if (--Base::get_size() == 0) {
				span = 0;
				front = back;
				return;
			}

			normalize_front();
			normalize_back();
		}

		constexpr void remove_front() noexcept {
			destroy_at(Base::get_data() + front);
			Base::untrack(front);

			--Base::size;
			--span;
			front = next_index(front);

			normalize_front();
		}

		constexpr void remove_back() noexcept {
			const INDEX index = previous_index(back);

			destroy_at(Base::get_data() + index);
			Base::untrack(index);

			--Base::size;
			--span;
			back = index;

			normalize_back();
		}

#pragma endregion

#pragma region Dequeue, Enqueue, & Peek

		[[nodiscard]] constexpr bool can_dequeue(INDEX count = 1) const noexcept {
			return Base::can_remove(count);
		}

		[[nodiscard]] constexpr T dequeue_front()
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T data = std::move(Base::get(front));
			remove_front();
			return data;
		}

		[[nodiscard]] constexpr T dequeue_back()
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T data = std::move(Base::get(previous_index(back)));
			remove_back();
			return data;
		}

		[[nodiscard]] constexpr bool can_enqueue(INDEX count = 1)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			return Base::can_add(count);
		}

		[[nodiscard]] constexpr INDEX enqueue_back(const T& data)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			return add_back(data);
		}

		[[nodiscard]] constexpr INDEX enqueue_back(T&& data)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			return add_back(std::move(data));
		}

		[[nodiscard]] constexpr INDEX enqueue_front(const T& data)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			return add_front(data);
		}

		[[nodiscard]] constexpr INDEX enqueue_front(T&& data)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			return add_front(std::move(data));
		}

		[[nodiscard]] constexpr bool can_peek() const noexcept {
			return !Base::is_empty();
		}

		[[nodiscard]] constexpr T& peek_front() noexcept {
			return Base::get(front);
		}

		[[nodiscard]] constexpr const T& peek_front() const noexcept {
			return Base::get(front);
		}

		[[nodiscard]] constexpr T& peek_back() noexcept {
			return Base::get(previous_index(back));
		}

		[[nodiscard]] constexpr const T& peek_back() const noexcept {
			return Base::get(previous_index(back));
		}

#pragma endregion

#pragma region Memory Management

		constexpr void empty()
			noexcept (concepts::NothrowResizableStorage<Base>)
		{
			Base::empty();
			back = 0;
			front = 0;
			span = 0;
		}

#pragma endregion

#pragma endregion

	};

}
