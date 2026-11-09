#pragma once

#include "Bitset.hpp"
#include "TrackedContainer.hpp"
#include "Deque.hpp"

namespace DSA {

	template <concepts::StoragePolicy StoragePolicy, typename Deque_StoragePolicy, typename T>
		requires std::is_nothrow_destructible_v<T>
	class BitmappedPriorityDeque : public TrackedContainer<StoragePolicy, Deque<Deque_StoragePolicy, T>> {
	public:

		using Base = TrackedContainer<StoragePolicy, Deque<Deque_StoragePolicy, T>>;
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

		[[nodiscard]] constexpr T dequeue_greatest(Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX priority = Base::find_last_tracked(explicitCapacity);
			auto& deque = Base::get(priority);
			T data = deque.dequeue_front();
			if (deque.is_empty())
				Base::tracked.reset_bit_at(priority);
			--Base::size;
			return data;
		}

		[[nodiscard]] constexpr T dequeue_least(Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX priority = Base::find_first_tracked(explicitCapacity);
			auto& deque = Base::get(priority);
			T data = deque.dequeue_front();
			if (deque.is_empty())
				Base::untrack(priority);
			--Base::size;
			return data;
		}

		[[nodiscard]] constexpr T dequeue(INDEX priority)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			auto& deque = Base::get(priority);
			T data = deque.dequeue_front();
			if (deque.is_empty())
				Base::untrack(priority);
			--Base::size;
			return data;
		}

		[[nodiscard]] constexpr bool can_enqueue(INDEX priority, INDEX count = 1, Base::Capacity explicitCapacity = {}) const noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			return priority < capacity && Base::get(priority).can_enqueue(count, explicitCapacity);
		}

		constexpr void enqueue(INDEX priority, const T& data, Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			Base::get(priority).enqueue_back(data, explicitCapacity);
			Base::track(priority);
			++Base::size;
		}

		constexpr void enqueue(INDEX priority, T&& data, Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			Base::get(priority).enqueue_back(std::move(data), explicitCapacity);
			Base::track(priority);
			++Base::size;
		}

#pragma endregion

#pragma endregion

	};

}
