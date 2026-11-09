#pragma once

#include "TrackedContainer.hpp"
#include "Bitset.hpp"

namespace DSA {

	template <concepts::StoragePolicy StoragePolicy, typename T>
		requires std::is_nothrow_destructible_v<T>
	class SparseDeque : public TrackedContainer<StoragePolicy, T> {
	public:

		using Base = TrackedContainer<StoragePolicy, T>;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

		INDEX back = 0;
		INDEX front = 0;
		INDEX span = 0;

#pragma region Methods

		[[nodiscard]] constexpr INDEX next_index(INDEX index, Base::Capacity explicitCapacity = {}) const noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			return (static_cast<INDEX>(index) + 1 == capacity)
				? INDEX{ 0 }
				: static_cast<INDEX>(index + 1);
		}

		[[nodiscard]] constexpr INDEX previous_index(INDEX index, Base::Capacity explicitCapacity = {}) const noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			return index == 0
				? static_cast<INDEX>(capacity - 1)
				: static_cast<INDEX>(index - 1);
		}

		[[nodiscard]] constexpr INDEX distance(INDEX from, INDEX to, Base::Capacity explicitCapacity = {}) const noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			return from < to ? to - from : capacity - from + to;
		}

		constexpr void normalize_front(Base::Capacity explicitCapacity = {}) noexcept {
			if (Base::get_size() == 0) {
				span = 0;
				front = back;
				return;
			}

			if (Base::is_tracking(front))
				return;

			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			const INDEX next = Base::find_first_tracked(next_index(front, explicitCapacity), explicitCapacity);

			if (next == capacity) {
				span = 0;
				Base::set_size(0);
				front = back;
				return;
			}

			const INDEX skipped = distance(front, next, capacity);

			front = static_cast<INDEX>(next);
			span -= skipped;
		}

		constexpr void normalize_back(Base::Capacity explicitCapacity = {}) noexcept {
			if (Base::get_size() == 0) {
				span = 0;
				front = back;
				return;
			}

			while (span != 0) {
				const INDEX last = previous_index(back, explicitCapacity);
				if (Base::is_tracking(last))
					break;
				back = last;
				--span;
			}

			if (span == 0)
				front = back;
		}

#pragma endregion

	public:

#pragma region Methods

#pragma region Constructors & Destructors

		SparseDeque() noexcept = default;

		SparseDeque(INDEX initialCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		: Base(initialCapacity) {}

#pragma endregion

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

		[[nodiscard]] constexpr bool is_full(Base::Capacity explicitCapacity = {}) const noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			return span == capacity;
		}

#pragma endregion

#pragma region Add & Remove

		[[nodiscard]] constexpr INDEX add_back(const T& data, Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			const INDEX index = back;

			copy_construct_at(Base::get_data() + index, data);
			Base::track(index);

			++Base::size;
			++span;
			back = next_index(back, explicitCapacity);

			return index;
		}

		[[nodiscard]] constexpr INDEX add_back(T&& data, Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX index = back;

			move_construct_at(Base::get_data() + index, std::move(data));
			Base::track(index);

			++Base::size;
			++span;
			back = next_index(back, explicitCapacity);

			return index;
		}

		[[nodiscard]] constexpr INDEX add_front(const T& data, Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			const INDEX newFront = previous_index(front, explicitCapacity);

			copy_construct_at(Base::get_data() + newFront, data);
			Base::track(newFront);

			front = newFront;
			++Base::size;
			++span;

			return newFront;
		}

		[[nodiscard]] constexpr INDEX add_front(T&& data, Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX newFront = previous_index(front, explicitCapacity);

			move_construct_at(Base::get_data() + newFront, std::move(data));
			Base::track(newFront);

			front = newFront;
			++Base::size;
			++span;

			return newFront;
		}

		constexpr void remove(INDEX index, Base::Capacity explicitCapacity = {}) noexcept {
			destroy_at(Base::get_data() + index);
			Base::untrack(index);

			if (--Base::get_size() == 0) {
				span = 0;
				front = back;
				return;
			}

			normalize_front(explicitCapacity);
			normalize_back(explicitCapacity);
		}

		constexpr void remove_front(Base::Capacity explicitCapacity = {}) noexcept {
			const INDEX index = front;

			destroy_at(Base::get_data() + index);
			Base::untrack(index);

			--Base::size;
			--span;
			front = next_index(front, explicitCapacity);

			normalize_front(explicitCapacity);
		}

		constexpr void remove_back(Base::Capacity explicitCapacity = {}) noexcept {
			const INDEX index = previous_index(back, explicitCapacity);

			destroy_at(Base::get_data() + index);
			Base::untrack(index);

			--Base::size;
			--span;
			back = index;

			normalize_back(explicitCapacity);
		}

#pragma endregion

#pragma region Dequeue, Enqueue, & Peek

		[[nodiscard]] constexpr bool can_dequeue(INDEX count = 1) const noexcept {
			return Base::can_remove(count);
		}

		[[nodiscard]] constexpr T dequeue_front(Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T data = std::move(Base::get(front));
			remove_front(explicitCapacity);
			return data;
		}

		[[nodiscard]] constexpr T dequeue_back(Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T data = std::move(Base::get(previous_index(back, explicitCapacity)));
			remove_back(explicitCapacity);
			return data;
		}

		[[nodiscard]] constexpr bool can_enqueue(INDEX count = 1, Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_move_constructible_v<T>)
			requires concepts::Storage<Base>
		{
			return Base::can_add(explicitCapacity, count);
		}

		[[nodiscard]] constexpr INDEX enqueue_back(const T& data, Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_copy_constructible_v<T>)
			requires concepts::Storage<Base>
		{
			return add_back(data, explicitCapacity);
		}

		[[nodiscard]] constexpr INDEX enqueue_back(T&& data, Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_move_constructible_v<T>)
			requires concepts::Storage<Base>
		{
			return add_back(std::move(data), explicitCapacity);
		}

		[[nodiscard]] constexpr INDEX enqueue_front(const T& data, Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_copy_constructible_v<T>)
			requires concepts::Storage<Base>
		{
			return add_front(data, explicitCapacity);
		}

		[[nodiscard]] constexpr INDEX enqueue_front(T&& data, Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_move_constructible_v<T>)
			requires concepts::Storage<Base>
		{
			return add_front(std::move(data), explicitCapacity);
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
			return peek_back(Base::get_capacity());
		}

		[[nodiscard]] constexpr const T& peek_back() const noexcept {
			return peek_back(Base::get_capacity());
		}

		[[nodiscard]] constexpr T& peek_back(Base::Capacity explicitCapacity = {}) noexcept {
			return Base::get(previous_index(back, explicitCapacity));
		}

		[[nodiscard]] constexpr const T& peek_back(Base::Capacity explicitCapacity = {}) const noexcept {
			return Base::get(previous_index(back, explicitCapacity));
		}

#pragma endregion

#pragma region Memory Management

		constexpr void empty(Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
		{
			Base::empty(explicitCapacity);
			back = 0;
			front = 0;
			span = 0;
		}

#pragma endregion

#pragma endregion

	};

}
