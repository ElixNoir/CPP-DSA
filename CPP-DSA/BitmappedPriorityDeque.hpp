#pragma once

#include "TrackedContainer.hpp"
#include "Deque.hpp"

namespace DSA {

	template <concepts::ContainerPolicy ContainerPolicy, typename DEQUE_CONTAINER_POLICY, typename T_>
		requires std::is_nothrow_destructible_v<T_>
	class BitmappedPriorityDeque : public TrackedContainer<ContainerPolicy, Deque<DEQUE_CONTAINER_POLICY, T_>> {
	public:

		using Base = TrackedContainer<ContainerPolicy, Deque<DEQUE_CONTAINER_POLICY, T_>>;
		using typename Base::INDEX;
		using typename Base::T;

#pragma region Methods

#pragma region Dequeue & Enqueue

		[[nodiscard]] constexpr bool can_dequeue(const INDEX count = 1) const noexcept {
			return Base::can_remove(count);
		}

		[[nodiscard]] constexpr bool can_dequeue_from_priority(const INDEX priority, const INDEX count = 1) const noexcept {
			return Base::get(priority).can_dequeue(count);
		}

		[[nodiscard]] constexpr T dequeue_greatest()
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX priority = Base::find_last_tracked();
			auto& deque = Base::get(priority);
			T data = deque.dequeue_front();
			if (deque.is_empty())
				Base::tracked.reset_bit_at(priority);
			--Base::size;
			return data;
		}

		[[nodiscard]] constexpr T dequeue_least()
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX priority = Base::find_first_tracked();
			auto& deque = Base::get(priority);
			T data = deque.dequeue_front();
			if (deque.is_empty())
				Base::untrack(priority);
			--Base::size;
			return data;
		}

		[[nodiscard]] constexpr T dequeue(const INDEX priority)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			auto& deque = Base::get(priority);
			T data = deque.dequeue_front();
			if (deque.is_empty())
				Base::untrack(priority);
			--Base::size;
			return data;
		}

		[[nodiscard]] constexpr bool can_enqueue(const INDEX priority, const INDEX count = 1) const noexcept {
			const INDEX capacity = Base::get_capacity();
			return priority < capacity && Base::get(priority).can_enqueue(count, capacity);
		}

		constexpr void enqueue(const INDEX priority, const T& data)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			Base::get(priority).enqueue_back(data);
			Base::track(priority);
			++Base::size;
		}

		constexpr void enqueue(const INDEX priority, T&& data)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			Base::get(priority).enqueue_back(std::move(data));
			Base::track(priority);
			++Base::size;
		}

#pragma endregion

#pragma endregion

	};

}
