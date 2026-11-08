#pragma once

#include "TrackedContainer.hpp"
#include "Bitset.hpp"

namespace DSA {

	template <concepts::StoragePolicy ContainerConfiguration, typename T>
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

		[[nodiscard]] constexpr INDEX next_index(INDEX index, INDEX capacity = Base::default_capacity()) const noexcept {
			return (static_cast<INDEX>(index) + 1 == capacity)
				? INDEX{ 0 }
				: static_cast<INDEX>(index + 1);
		}

		[[nodiscard]] constexpr INDEX previous_index(INDEX index, INDEX capacity = Base::default_capacity()) const noexcept {
			return index == 0
				? static_cast<INDEX>(capacity - 1)
				: static_cast<INDEX>(index - 1);
		}

		[[nodiscard]] constexpr INDEX distance(INDEX from, INDEX to, INDEX capacity = Base::default_capacity()) const noexcept {
			return from < to ? to - from : capacity - from + to;
		}

		constexpr void normalize_front(INDEX capacity = Base::default_capacity()) noexcept {
			if (Base::get_size() == 0) {
				span = 0;
				front = back;
				return;
			}

			if (Base::is_tracking(front))
				return;

			const INDEX next = Base::find_first_tracked(next_index(front));

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

		constexpr void normalize_back(INDEX capacity = Base::default_capacity()) noexcept {
			if (Base::get_size() == 0) {
				span = 0;
				front = back;
				return;
			}

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

		[[nodiscard]] constexpr bool is_full(INDEX capacity = Base::default_capacity()) const noexcept {
			return span == capacity;
		}

#pragma endregion

#pragma region Add & Remove

		[[nodiscard]] constexpr INDEX add_back(const T& value, INDEX capacity = Base::default_capacity())
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			const INDEX index = back;

			copy_construct_at(Base::get_data() + index, value);
			Base::track(index);

			++Base::size;
			++span;
			back = next_index(back, capacity);

			return index;
		}

		[[nodiscard]] constexpr INDEX add_back(T&& value, INDEX capacity = Base::default_capacity())
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX index = back;

			move_construct_at(Base::get_data() + index, std::move(value));
			Base::track(index);

			++Base::size;
			++span;
			back = next_index(back, capacity);

			return index;
		}

		[[nodiscard]] constexpr INDEX add_front(const T& value, INDEX capacity = Base::default_capacity())
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			const INDEX newFront = previous_index(front, capacity);

			copy_construct_at(Base::get_data() + newFront, value);
			Base::track(newFront);

			front = newFront;
			++Base::size;
			++span;

			return newFront;
		}

		[[nodiscard]] constexpr INDEX add_front(T&& value, INDEX capacity = Base::default_capacity())
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX newFront = previous_index(front, capacity);

			move_construct_at(Base::get_data() + newFront, std::move(value));
			Base::track(newFront);

			front = newFront;
			++Base::size;
			++span;

			return newFront;
		}

		constexpr void remove(INDEX index, INDEX capacity = Base::default_capacity()) noexcept {
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

		constexpr void remove_front(INDEX capacity = Base::default_capacity()) noexcept {
			const INDEX index = front;

			destroy_at(Base::get_data() + index);
			Base::untrack(index);

			--Base::size;
			--span;
			front = next_index(front, capacity);

			normalize_front(capacity);
		}

		constexpr void remove_back(INDEX capacity = Base::default_capacity()) noexcept {
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
			return Base::can_add(capacity, count);
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

		constexpr void empty(INDEX capacity = Base::default_capacity())
			noexcept (concepts::NothrowResizableStorage<Base, T>)
		{
			Base::empty(capacity);
			back = 0;
			front = 0;
			span = 0;
		}

#pragma endregion

#pragma endregion

	};

}
