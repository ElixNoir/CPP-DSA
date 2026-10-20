#pragma once

#include "Bitset.hpp"
#include "TrackedContainer.hpp"
#include "Deque.hpp"

namespace DSA {

	template <typename ContainerConfiguration, typename Deque_ContainerConfiguration, typename T>
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

		[[nodiscard]] constexpr bool can_dequeue(INDEX priority, INDEX count = 1) const noexcept {
			return priority < Base::get_capacity() && Base::get_data()[priority].can_dequeue(count);
		}

		[[nodiscard]] constexpr bool can_enqueue(INDEX priority, INDEX count = 1) const noexcept {
			return priority < Base::get_capacity() && Base::get_data()[priority].can_enqueue(count);
		}

		[[nodiscard]] T dequeue_greatest() {
			const INDEX capacity = Base::get_capacity();
			const INDEX priority = Base::tracked.last_leading_one(capacity);
			auto& deque = Base::get_data()[priority];
			T value = deque.dequeue_front();
			if (deque.is_empty())
				Base::tracked.reset_bit_at(priority);
			Base::size--;
			return value;
		}

		[[nodiscard]] T dequeue_least() {
			const INDEX capacity = Base::get_capacity();
			const INDEX priority = Base::tracked.first_trailing_one();
			auto& deque = Base::get_data()[priority];
			T value = deque.dequeue_front();
			if (deque.is_empty())
				Base::tracked.reset_bit_at(priority);
			Base::size--;
			return value;
		}

		[[nodiscard]] T dequeue(INDEX priority) {
			auto& deque = Base::get_data()[priority];
			T value = deque.dequeue_front();
			if (deque.is_empty())
				Base::tracked.reset_bit_at(priority);
			Base::size--;
			return value;
		}

		[[nodiscard]] constexpr bool can_enqueue(INDEX priority) const noexcept {
			return priority < Base::get_capacity() && Base::get_data()[priority].can_enqueue();
		}

		void enqueue(INDEX priority, const T& value) {
			Base::get_data()[priority].enqueue_back(value);
			Base::tracked.set_bit_at(priority);
			Base::size++;
		}

		void enqueue(INDEX priority, T&& value) {
			Base::get_data()[priority].enqueue_back(std::move(value));
			Base::tracked.set_bit_at(priority);
			Base::size++;
		}

#pragma endregion

#pragma endregion

	};

}