#pragma once

#include "Collection.hpp"
#include "Math.hpp"

namespace DSA {

	template <concepts::StoragePolicy StoragePolicy, typename T>
		requires std::is_nothrow_destructible_v<T>
	class Deque : public Collection<StoragePolicy, T> {
	public:

		using Base = Collection<StoragePolicy, T>;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

		INDEX back = 0;
		INDEX front = 0;

#pragma region Methods

		void helper_destroy(Base::Capacity explicitCapacity = {}) noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			T* const data = Base::get_data();

			constexpr T* const end = data + (front + Base::get_size()) % capacity;

			if (end > front)
				destroy_range_forward<false>(data + front, end);
			else {
				destroy_range_forward<false>(data + front, data + capacity - 1);
				destroy_range_forward(data, end);
			}
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		~Deque() noexcept = default;

		~Deque() noexcept
			requires concepts::HasCapacity<Base>
		{
			destroy();
		}

		constexpr void destroy(Base::Capacity explicitCapacity = {}) noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				helper_destroy(explicitCapacity);
		}

#pragma endregion

#pragma region Methods

#pragma region Getters

		[[nodiscard]] constexpr INDEX get_back() const noexcept {
			return back;
		}

		[[nodiscard]] constexpr INDEX get_front() const noexcept {
			return front;
		}

#pragma endregion

#pragma region Add & Remove

		constexpr void add_back(const T& data, Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			copy_construct_at(Base::get_data() + back, data);
			back = (back + 1 == capacity) ? 0 : back + 1;
			++Base::size;
		}

		constexpr void add_back(T&& data, Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			move_construct_at(Base::get_data() + back, std::move(data));
			back = (back + 1 == capacity) ? 0 : back + 1;
			++Base::size;
		}

		constexpr void add_front(const T& data, Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			const INDEX newFront = (front == 0) ? capacity - 1 : front - 1;
			move_construct_at(Base::get_data() + newFront, data);
			front = newFront;
			++Base::size;
		}

		constexpr void add_front(T&& data, Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			const INDEX newFront = (front == 0) ? capacity - 1 : front - 1;
			move_construct_at(Base::get_data() + newFront, std::move(data));
			front = newFront;
			++Base::size;
		}

		constexpr void remove_back(Base::Capacity explicitCapacity = {}) noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			back = (back == 0) ? capacity - 1 : back - 1;
			destroy_at(Base::get_data() + back);
			--Base::size;
		}

		constexpr void remove_back_many(INDEX count, Base::Capacity explicitCapacity = {}) noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			T* const data = Base::get_data();

			INDEX newBack = (back - count) % capacity;

			if (newBack < back)
				destroy_range_backward(data + newBack, data + back);
			else {
				destroy_range_backward(data, data + back);
				destroy_range_backward(data + newBack, data + capacity);
			}

			back = newBack;
			Base::size -= count;
		}

		constexpr void remove_front(Base::Capacity explicitCapacity = {}) noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			destroy_at(Base::get_data() + front);
			front = (front + 1 == capacity) ? 0 : front + 1;
			--Base::size;
		}

		constexpr void remove_front_many(INDEX count, Base::Capacity explicitCapacity = {}) noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			T* const data = Base::get_data();

			INDEX newFront = (front + count) % capacity;

			if (newFront > front)
				destroy_range_forward<false>(data + front, data + newFront);
			else {
				destroy_range_forward<false>(data + front, data + capacity - 1);
				destroy_range_forward(data, data + newFront);
			}

			front = newFront;
			Base::size -= count;
		}

#pragma endregion

#pragma region Dequeue, Enqueue, & Peek

		[[nodiscard]] constexpr bool can_dequeue(INDEX count = 1) const noexcept {
			return Base::can_remove(count);
		}

		[[nodiscard]] constexpr T dequeue_back(Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			back = (back == 0) ? capacity - 1 : back - 1;
			T* const address = Base::get_data() + back;
			--Base::size;
			T data = std::move(*address);
			destroy_at(address);
			return data;
		}

		[[nodiscard]] constexpr void dequeue_back_many(T* const destination, INDEX count, Base::Capacity explicitCapacity = {})
		    noexcept (std::is_nothrow_move_constructible_v<T>)
		{
		    const INDEX capacity = Base::extract_capacity(explicitCapacity);
		    T* const data = Base::get_data();
		
		    const INDEX newBack = back >= count ? back - count : capacity - (count - back);
		
		    const INDEX firstLength = Aggregation::minimum(count, back + 1);
		
		    if (count - firstLength == 0) {
		        move_construct_range_forward<true, false>(destination,
		            destination + count,
		            data + newBack + 1);
		
		        destroy_range_forward(data + newBack + 1, data + back + 1);
		    }
		    else {
		        move_construct_range_forward<true, false>(destination,
		            destination + firstLength,
		            data + newBack + 1);
		
		        move_construct_range_forward<true, false>(destination + firstLength,
		            destination + count,
		            data);
		
		        destroy_range_forward(data + newBack + 1, data + capacity);
		        destroy_range_forward(data, data + count - firstLength);
		    }
		
		    back = newBack;
		    Base::size -= count;
		}

		[[nodiscard]] constexpr T dequeue_front(Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			T* const address = Base::get_data() + front;
			front = (front + 1 == capacity) ? 0 : front + 1;
			--Base::size;
			T data = std::move(*address);
			destroy_at(address);
			return data;
		}

		[[nodiscard]] constexpr void dequeue_front_many(T* const destination, INDEX count, Base::Capacity explicitCapacity = {})
		    noexcept (std::is_nothrow_move_constructible_v<T>)
		{
		    const INDEX capacity = Base::extract_capacity(explicitCapacity);
		    T* const data = Base::get_data();
		
		    const INDEX newFront = front >= capacity - count ? front - (capacity - count) : front + count;
		
		    const INDEX firstLength = Aggregation::minimum(count, capacity - front);
		
		    if (count - firstLength == 0) {
		        move_construct_range_forward<true, false>(destination,
		            destination + count,
		            data + front);
		
		        destroy_range_forward(data + front, data + front + count);
		    }
		    else {
		        move_construct_range_forward<true, false>(destination,
		            destination + firstLength,
		            data + front);
		
		        move_construct_range_forward<true, false>(destination + firstLength,
		            destination + count,
		            data);
		
		        destroy_range_forward(data + front, data + capacity);
		        destroy_range_forward(data, data + count - firstLength);
		    }
		
		    front = newFront;
		    Base::size -= count;
		}

		[[nodiscard]] constexpr bool can_enqueue(INDEX count = 1, Base::Capacity explicitCapacity = {}) const noexcept {
			return Base::can_add(count, explicitCapacity);
		}

		constexpr void enqueue_back(const T& data, Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			add_back(data, explicitCapacity);
		}

		constexpr void enqueue_back(T&& data, Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			add_back(std::move(data), explicitCapacity);
		}

		constexpr void enqueue_front(const T& data, Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			add_front(data, explicitCapacity);
		}

		constexpr void enqueue_front(T&& data, Base::Capacity explicitCapacity = {})
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			add_front(std::move(data), explicitCapacity);
		}

		[[nodiscard]] constexpr bool can_peek() const noexcept {
			return !Base::is_empty();
		}

		[[nodiscard]] constexpr T& peek_back(Base::Capacity explicitCapacity = {}) noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			return Base::get((back == 0) ? capacity - 1 : back - 1);
		}

		[[nodiscard]] constexpr const T& peek_back(Base::Capacity explicitCapacity = {}) const noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			return Base::get((back == 0) ? capacity - 1 : back - 1);
		}

		[[nodiscard]] constexpr T& peek_front() noexcept {
			return Base::get(front);
		}

		[[nodiscard]] constexpr const T& peek_front() const noexcept {
			return Base::get(front);
		}

#pragma endregion

#pragma region Memory Management

		void double_capacity(Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			grow(capacity << 1, exlicitCapacity);
		}

		constexpr void empty(Base::Capacity explicitCapacity = {}) noexcept {
			destroy(explicitCapacity);
			Base::set_size(0);
			back = 0;
			front = 0;
		}

		void expand(float ratio = 1.5, Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			grow(Math::ceiling(capacity * ratio), explicitCapacity);
		}

		void grow(INDEX newCapacity, Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			using ALLOCATOR = Base::ALLOCATOR;

			const INDEX size = Base::get_size();

			if constexpr (concepts::ReallocatableAllocator<ALLOCATOR> && std::is_trivially_copyable_v<T>)
				Base::data = reinterpret_cast<std::byte*>(ALLOCATOR::reallocate(Base::get_data(), newCapacity));
			else {
				T* const oldData = Base::get_data();
				T* const newData = ALLOCATOR::allocate(newCapacity);

				try {
					const INDEX count = Aggregation::minimum(size, capacity - front);
					move_construct_range_forward<true, false>(newData, newData + count, oldData + front);
					if (size - count != 0)
						move_construct_range_forward(newData + count, newData + size, oldData);
				}
				catch (...) {
					ALLOCATOR::deallocate(newData);
					throw;
				}

				ALLOCATOR::deallocate(oldData);
				Base::data = reinterpret_cast<std::byte*>(newData);
			}

			if constexpr (concepts::HasMutableCapacity<Base>)
				Base::set_capacity(newCapacity);
			back = size;
			front = 0;
		}

		void reserve(INDEX newCapacity, Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			if (newCapacity > capacity)
				grow(newCapacity, explicitCapacity);
		}

#pragma endregion

#pragma endregion

	};

}
