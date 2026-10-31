#pragma once

#include "TrackedContainer.hpp"
#include "Bitset.hpp"

namespace DSA {

	template <typename ContainerConfiguration, typename T>
		requires std::is_nothrow_destructible_v<T>
	class SparseDeque : public TrackedContainer<ContainerConfiguration, T> {
	public:

		using Base = TrackedContainer<ContainerConfiguration, T>;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

		INDEX back = 0;
		INDEX front = 0;
		
		// Number of physical positions in the logical circular interval
		// [front, back). This can be greater than size because it includes
		// holes left by arbitrary removals.
		INDEX span = 0;

#pragma region Methods

		[[nodiscard]] constexpr INDEX next_index(INDEX index) const noexcept {
			return (static_cast<INDEX>(index) + 1 == Base::get_capacity())
				? INDEX{ 0 }
				: static_cast<INDEX>(index + 1);
		}

		[[nodiscard]] constexpr INDEX previous_index(INDEX index) const noexcept {
			return index == 0
				? static_cast<INDEX>(Base::get_capacity() - 1)
				: static_cast<INDEX>(index - 1);
		}

		[[nodiscard]] constexpr INDEX distance(INDEX from, INDEX to) const noexcept {
			return from < to ? to - from : Base::get_capacity() - from + to;
		}

		constexpr void normalize_front() noexcept {
			if (Base::get_size() == 0) {
				span = 0;
				front = back;
				return;
			}

			if (Base::is_tracking(front))
				return;

			const INDEX next = Base::find_first_tracked(next_index(front));

			// size != 0 guarantees that another occupied slot exists.
			if (next == Base::get_capacity()) {
				span = 0;
				Base::set_size(0);
				front = back;
				return;
			}

			const INDEX skipped = distance(front, static_cast<INDEX>(next));

			front = static_cast<INDEX>(next);
			span -= skipped;
		}

		constexpr void normalize_back() noexcept {
			if (Base::get_size() == 0) {
				span = 0;
				front = back;
				return;
			}

			// The back cursor is one position past the logical tail. Trim
			// trailing holes so the final position in the span is occupied.
			while (span != 0) {
				const INDEX last = previous_index(back);

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
			requires concepts::DynamicContainer<Base>
		: Base(initialCapacity) {}

		~SparseDeque() noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				Base::helper_destroy();
		}

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

		[[nodiscard]] constexpr bool can_enqueue(INDEX count = 1) const noexcept {
			return count <= Base::get_capacity() - span;
		}

		[[nodiscard]] constexpr bool can_dequeue(INDEX count = 1) const noexcept {
			return Base::can_remove(count);
		}

		[[nodiscard]] constexpr bool is_full() const noexcept {
			return span == Base::get_capacity();
		}

#pragma endregion

#pragma region Add & Remove

		[[nodiscard]] constexpr INDEX add_back(const T& value)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			const INDEX index = back;

			copy_construct_at(Base::get_data() + index, value);
			Base::track(index);

			++Base::size;
			++span;
			back = next_index(back);

			return index;
		}

		[[nodiscard]] constexpr INDEX add_back(T&& value)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX index = back;

			move_construct_at(Base::get_data() + index, std::move(value));
			Base::track(index);

			++Base::size;
			++span;
			back = next_index(back);

			return index;
		}

		[[nodiscard]] constexpr INDEX add_front(const T& value)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			const INDEX newFront = previous_index(front);

			copy_construct_at(Base::get_data() + newFront, value);
			Base::track(newFront);

			front = newFront;
			++Base::size;
			++span;

			return newFront;
		}

		[[nodiscard]] constexpr INDEX add_front(T&& value)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX newFront = previous_index(front);

			move_construct_at(Base::get_data() + newFront, std::move(value));
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
			const INDEX index = front;

			destroy_at(Base::get_data() + index);
			Base::untrack(index);

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
			T value = std::move(Base::get(front));
			remove_front();
			return value;
		}

		[[nodiscard]] constexpr T dequeue_back()
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T value = std::move(Base::get(previous_index(back)));
			remove_back();
			return value;
		}

		[[nodiscard]] constexpr bool can_enqueue(INDEX count = 1) const noexcept {
			return Base::can_add(count);
		}

		[[nodiscard]] constexpr INDEX enqueue_back(const T& value)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			return add_back(value);
		}

		[[nodiscard]] constexpr INDEX enqueue_back(T&& value)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			return add_back(std::move(value));
		}

		[[nodiscard]] constexpr INDEX enqueue_front(const T& value)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			return add_front(value);
		}

		[[nodiscard]] constexpr INDEX enqueue_front(T&& value)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			return add_front(std::move(value));
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

		constexpr void empty() noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				Base::helper_destroy();

			Base::tracked.zero();

			Base::size = 0;
			back = 0;
			front = 0;
			span = 0;
		}

#pragma endregion

#pragma endregion

	};

}
