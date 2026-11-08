#pragma once

#include "Bitset.hpp"
#include "TrackedContainer.hpp"
#include "Deque.hpp"

namespace DSA {

	template <concepts::StoragePolicy ContainerConfiguration, typename Deque_ContainerConfiguration, typename T>
		requires std::is_nothrow_destructible_v<T>
	class BitmappedPriorityDeque : public TrackedContainer<ContainerConfiguration, Deque<Deque_ContainerConfiguration, T>> {
	public:

		using Base = TrackedContainer<ContainerConfiguration, Deque<Deque_ContainerConfiguration, T>>;
		using typename Base::INDEX;
		using typename Base::T;

#pragma region Methods

#pragma region Dequeue & Enqueue

		[[nodiscard]] constexpr bool can_dequeue(INDEX count = 1) const noexcept {
			return Base::can_remove(count);
		}

		[[nodiscard]] constexpr bool can_dequeue_from_priority(INDEX priority, INDEX count = 1) const noexcept {
			return Base::get(priority).can_dequeue(count);
		}

		[[nodiscard]] constexpr T dequeue_greatest(INDEX capacity = Base::default_capacity())
			noexcept (std::is_nothrow_move_constructible_v<T>)
			requires concepts::Storage<Base>
		{
			const INDEX priority = Base::tracked.last_leading_one(capacity);
			auto& deque = Base::get(priority);
			T value = deque.dequeue_front();
			if (deque.is_empty())
				Base::tracked.reset_bit_at(priority);
			--Base::size;
			return value;
		}

		[[nodiscard]] constexpr T dequeue_least(INDEX capacity = Base::default_capacity())
			noexcept (std::is_nothrow_move_constructible_v<T>)
			requires concepts::Storage<Base>
		{
			const INDEX priority = Base::find_first_tracked_and_capacity(0, capacity);
			auto& deque = Base::get(priority);
			T value = deque.dequeue_front();
			if (deque.is_empty())
				Base::untrack(priority);
			--Base::size;
			return value;
		}

		[[nodiscard]] constexpr T dequeue(INDEX priority)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			auto& deque = Base::get(priority);
			T value = deque.dequeue_front();
			if (deque.is_empty())
				Base::untrack(priority);
			--Base::size;
			return value;
		}

		[[nodiscard]] constexpr bool can_enqueue(INDEX priority, INDEX count = 1, INDEX capacity = Base::default_capacity()) const noexcept {
			return priority < capacity && Base::get(priority).can_enqueue(capacity, count);
		}

		constexpr void enqueue(INDEX priority, const T& value, INDEX capacity = Base::default_capacity())

			requires concepts::Storage<Base>
		{
			Base::get(priority).enqueue_back(value, capacity);
			Base::track(priority);
			++Base::size;
		}

		constexpr void enqueue(INDEX priority, T&& value, INDEX capacity = Base::default_capacity())

			requires concepts::Storage<Base>
		{
			Base::get(priority).enqueue_back(std::move(value), capacity);
			Base::track(priority);
			++Base::size;
		}

#pragma endregion

#pragma endregion

	};

}