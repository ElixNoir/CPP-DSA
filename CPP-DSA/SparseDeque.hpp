#pragma once

#include "TrackedContainer.hpp"
#include "Bitset.hpp"

namespace DSA {

	template <concepts::StorageConfiguration ContainerConfiguration, typename T>
		requires std::is_nothrow_destructible_v<T>
	class SparseDeque : public TrackedContainer<ContainerConfiguration, T> {
	public:

		using Base = TrackedContainer<ContainerConfiguration, T>;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

		INDEX back = 0;
		INDEX front = 0;
		INDEX span = 0;

#pragma region Methods

		[[nodiscard]] constexpr INDEX next_index(INDEX index)
			const noexcept
			requires concepts::Container<Base>
		{
			return next_index(index, Base::get_capacity());)
		}

		[[nodiscard]] constexpr INDEX next_index(INDEX index, INDEX capacity)
			const noexcept
			requires concepts::Storage<Base>
		{
			return (static_cast<INDEX>(index) + 1 == capacity)
				? INDEX{ 0 }
				: static_cast<INDEX>(index + 1);
		}

		[[nodiscard]] constexpr INDEX previous_index(INDEX index)
			const noexcept
			requires concepts::Container<Base>
		{
			return previous_index(index, Base::get_capacity());
		}

		[[nodiscard]] constexpr INDEX previous_index(INDEX index, INDEX capacity)
			const noexcept
			requires concepts::Storage<Base>
		{
			return index == 0
				? static_cast<INDEX>(capacity - 1)
				: static_cast<INDEX>(index - 1);
		}

		[[nodiscard]] constexpr INDEX distance(INDEX from, INDEX to)
			const noexcept
			requires concepts::Container<Base>
		{
			return distance(from, to, Base::get_capacity());
		}

		[[nodiscard]] constexpr INDEX distance(INDEX from, INDEX to, INDEX capacity)
			const noexcept
			requires concepts::Storage<Base>
		{
			return from < to ? to - from : capacity - from + to;
		}

		constexpr void normalize_front()
			noexcept
			requires concepts::Container<Base>
		{
			normalize_front(Base::get_capacity());
		}

		constexpr void normalize_front(INDEX capacity)
			noexcept
			requires concepts::Storage<Base>
		{
			if (Base::get_size() == 0) {
				span = 0;
				front = back;
				return;
			}

			if (Base::is_tracking(front))
				return;

			const INDEX next = Base::find_first_tracked(next_index(front));

			// size != 0 guarantees that another occupied slot exists.
			if (next == capacity) {
				span = 0;
				Base::set_size(0);
				front = back;
				return;
			}

			const INDEX skipped = distance(front, static_cast<INDEX>(next), capacity);

			front = static_cast<INDEX>(next);
			span -= skipped;
		}

		constexpr void normalize_back()
			noexcept
			requires concepts::Container<Base>
		{
			normalize_back(Base::get_capacity());
		}

		constexpr void normalize_back(INDEX capacity)
			noexcept
			requires concepts::Storage<Base>
		{
			if (Base::get_size() == 0) {
				span = 0;
				front = back;
				return;
			}

			// The back cursor is one position past the logical tail. Trim
			// trailing holes so the final position in the span is occupied.
			while (span != 0) {
				const INDEX last = previous_index(back, capacity);

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
			requires concepts::DynamicStorage<Base>
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

		[[nodiscard]] constexpr bool is_full()
			const noexcept
			requires concepts::Container<Base>
		{
			return is_full(Base::get_capacity());
		}

		[[nodiscard]] constexpr bool is_full(INDEX capacity)
			const noexcept
			requires concepts::Storage<Base>
		{
			return span == capacity;
		}

#pragma endregion

#pragma region Add & Remove

		[[nodiscard]] constexpr INDEX add_back(const T& value)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
			requires concepts::Container<Base>
		{
			return add_back(value, Base::get_capacity());
		}

		[[nodiscard]] constexpr INDEX add_back(const T& value, INDEX capacity)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
			requires concepts::Storage<Base>
		{
			const INDEX index = back;

			copy_construct_at(Base::get_data() + index, value);
			Base::track(index);

			++Base::size;
			++span;
			back = next_index(back, capacity);

			return index;
		}

		[[nodiscard]] constexpr INDEX add_back(T&& value)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
			requires concepts::Container<Base>
		{
			return add_back(std::move(value), Base::get_capacity());
		}

		[[nodiscard]] constexpr INDEX add_back(T&& value, INDEX capacity)
			noexcept (std::is_nothrow_move_constructible_v<T>)
			requires concepts::Storage<Base>
		{
			const INDEX index = back;

			move_construct_at(Base::get_data() + index, std::move(value));
			Base::track(index);

			++Base::size;
			++span;
			back = next_index(back, capacity);

			return index;
		}

		[[nodiscard]] constexpr INDEX add_front(const T& value)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
			requires concepts::Container<Base>
		{
			return add_front(value, Base::get_capacity());
		}

		[[nodiscard]] constexpr INDEX add_front(const T& value, INDEX capacity)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
			requires concepts::Storage<Base>
		{
			const INDEX newFront = previous_index(front, capacity);

			copy_construct_at(Base::get_data() + newFront, value);
			Base::track(newFront);

			front = newFront;
			++Base::size;
			++span;

			return newFront;
		}

		[[nodiscard]] constexpr INDEX add_front(T&& value)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
			requires concepts::Container<Base>
		{
			return add_front(std::move(value), Base::get_capacity());
		}

		[[nodiscard]] constexpr INDEX add_front(T&& value, INDEX capacity)
			noexcept (std::is_nothrow_move_constructible_v<T>)
			requires concepts::Storage<Base>
		{
			const INDEX newFront = previous_index(front, capacity);

			move_construct_at(Base::get_data() + newFront, std::move(value));
			Base::track(newFront);

			front = newFront;
			++Base::size;
			++span;

			return newFront;
		}

		constexpr void remove(INDEX index)
			noexcept
			requires concepts::Container<Base>
		{
			remove(index, Base::get_capacity());
		}

		constexpr void remove(INDEX index, INDEX capacity)
			noexcept
			requires concepts::Storage<Base>
		{
			destroy_at(Base::get_data() + index);
			Base::untrack(index);

			if (--Base::get_size() == 0) {
				span = 0;
				front = back;
				return;
			}

			normalize_front(capacity);
			normalize_back(capacity);
		}

		constexpr void remove_front()
			noexcept
			requires concepts::Container<Base>
		{
			remove_front(Base::get_capacity());
		}

		constexpr void remove_front(INDEX capacity)
			noexcept
			requires concepts::Storage<Base>
		{
			const INDEX index = front;

			destroy_at(Base::get_data() + index);
			Base::untrack(index);

			--Base::size;
			--span;
			front = next_index(front, capacity);

			normalize_front(capacity);
		}

		constexpr void remove_back()
			noexcept
			requires concepts::Container<Base>
		{
			remove_back(Base::get_capacity());
		}

		constexpr void remove_back(INDEX capacity)
			noexcept
			requires concepts::Storage<Base>
		{
			const INDEX index = previous_index(back, capacity);

			destroy_at(Base::get_data() + index);
			Base::untrack(index);

			--Base::size;
			--span;
			back = index;

			normalize_back(capacity);
		}

#pragma endregion

#pragma region Dequeue, Enqueue, & Peek

		[[nodiscard]] constexpr bool can_dequeue(INDEX count = 1) const noexcept {
			return Base::can_remove(count);
		}

		[[nodiscard]] constexpr T dequeue_front()
			noexcept (std::is_nothrow_move_constructible_v<T>)
			requires concepts::Container<Base>
		{
			return dequeue_front(Base::get_capacity());
		}

		[[nodiscard]] constexpr T dequeue_front(INDEX capacity)
			noexcept (std::is_nothrow_move_constructible_v<T>)
			requires concepts::Storage<Base>
		{
			T value = std::move(Base::get(front));
			remove_front(capacity);
			return value;
		}

		[[nodiscard]] constexpr T dequeue_back()
			noexcept (std::is_nothrow_move_constructible_v<T>)
			requires concepts::Container<Base>
		{
			return dequeue_back(Base::get_capacity());
		}

		[[nodiscard]] constexpr T dequeue_back(INDEX capacity)
			noexcept (std::is_nothrow_move_constructible_v<T>)
			requires concepts::Storage<Base>
		{
			T value = std::move(Base::get(previous_index(back, capacity)));
			remove_back(capacity);
			return value;
		}

		[[nodiscard]] constexpr bool can_enqueue(INDEX count = 1)
			noexcept (std::is_nothrow_move_constructible_v<T>)
			requires concepts::Container<Base>
		{
			return can_enqueue(Base::get_capacity(), count);
		}

		[[nodiscard]] constexpr bool can_enqueue(INDEX capacity, INDEX count = 1)
			noexcept (std::is_nothrow_move_constructible_v<T>)
			requires concepts::Storage<Base>
		{
			return Base::can_add(count, capacity);
		}

		[[nodiscard]] constexpr INDEX enqueue_back(const T& value)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
			requires concepts::Container<Base>
		{
			return enqueue_back(value, Base::get_capacity());
		}

		[[nodiscard]] constexpr INDEX enqueue_back(const T& value, INDEX capacity)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
			requires concepts::Storage<Base>
		{
			return add_back(value, capacity);
		}

		[[nodiscard]] constexpr INDEX enqueue_back(T&& value)
			noexcept (std::is_nothrow_move_constructible_v<T>)
			requires concepts::Container<Base>
		{
			return enqueue_back(std::move(value), Base::get_capacity());
		}

		[[nodiscard]] constexpr INDEX enqueue_back(T&& value, INDEX capacity)
			noexcept (std::is_nothrow_move_constructible_v<T>)
			requires concepts::Storage<Base>
		{
			return add_back(std::move(value), capacity);
		}

		[[nodiscard]] constexpr INDEX enqueue_front(const T& value)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
			requires concepts::Container<Base>
		{
			return enqueue_front(value, Base::get_capacity());
		}

		[[nodiscard]] constexpr INDEX enqueue_front(const T& value, INDEX capacity)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
			requires concepts::Storage<Base>
		{
			return add_front(value, capacity);
		}

		[[nodiscard]] constexpr INDEX enqueue_front(T&& value)
			noexcept (std::is_nothrow_move_constructible_v<T>)
			requires concepts::Container<Base>
		{
			return enqueue_front(std::move(value), Base::get_capacity());
		}

		[[nodiscard]] constexpr INDEX enqueue_front(T&& value, INDEX capacity)
			noexcept (std::is_nothrow_move_constructible_v<T>)
			requires concepts::Storage<Base>
		{
			return add_front(std::move(value), capacity);
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

		[[nodiscard]] constexpr T& peek_back(INDEX capacity) noexcept {
			return Base::get(previous_index(back, capacity));
		}

		[[nodiscard]] constexpr const T& peek_back(INDEX capacity) const noexcept {
			return Base::get(previous_index(back, capacity));
		}

#pragma endregion

#pragma region Memory Management

		constexpr void empty() noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				Base::helper_destroy();

			Base::tracked.zero();

			Base::set_size(0);
			back = 0;
			front = 0;
			span = 0;
		}

#pragma endregion

#pragma endregion

	};

}
