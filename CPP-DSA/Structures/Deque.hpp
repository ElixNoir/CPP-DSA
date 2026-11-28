#pragma once

#include "../Algorithms/Aggregation.hpp"
#include "Collection.hpp"
#include "../Algorithms/Math.hpp"

namespace DSA {

	template <concepts::ContainerPolicy ContainerPolicy, typename T_>
		requires std::is_nothrow_destructible_v<T_>
	class Deque : public Collection<ContainerPolicy, T_> {
	public:

		using Base = Collection<ContainerPolicy, T_>;
		using typename Base::ALLOCATOR;
		using typename Base::INDEX;
		using typename Base::T;

		constexpr static bool is_dynamic = Base::is_dynamic;

	protected:

#pragma region Properties

		INDEX back = 0;
		INDEX front = 0;

#pragma endregion

#pragma region Methods

		template <ResizeOperation RESIZE_OPERATION>
			requires (RESIZE_OPERATION != ResizeOperation::UNKNOWN)
		constexpr void helper_resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			const INDEX size = Base::get_size();

			/* //! incomplete
			if constexpr (std::is_trivially_move_constructible_v<T> && concepts::ReallocatableAllocator<ALLOCATOR>) {
				T* const oldData = Base::get_data();
				T* const newData = ALLOCATOR::reallocate(oldData, newCapacity);

				if constexpr (!ALWAYS_NORMALIZE) {
					if (newData == oldData) {
						// normalize end but not front
					}
				}

				// normalize fallback

				return;
			}
			*/

			T* const newData = ALLOCATOR::allocate(newCapacity);
			T* const oldData = Base::get_data();
			const INDEX targetSize = Base::get_target_size();

			const INDEX firstChunk = Aggregation::minimum(targetSize, capacity - front);
			if constexpr (std::is_nothrow_move_constructible_v<T>) {
				reconstruct_range<Direction::Forward, true, false>(newData, newData + firstChunk, oldData + front);
				if (targetSize - firstChunk != 0)
					reconstruct_range<Direction::Forward, true, false>(newData + firstChunk, newData + targetSize, oldData);
			}
			else {
				try {
					move_construct_range<Direction::Forward, true, false>(newData, newData + firstChunk, oldData + front);
				}
				catch (...) {
					ALLOCATOR::deallocate(newData);
					throw;
				}

				if (targetSize - firstChunk != 0) {
					try {
						move_construct_range<Direction::Forward, true, false>(newData + firstChunk, newData + targetSize, oldData);
					}
					catch (...) {
						destroy_range(newData, newData + firstChunk);
						ALLOCATOR::deallocate(newData);
						throw;
					}
				}

				if constexpr (!std::is_trivially_destructible_v<T>) {
					const INDEX oldFirstChunk = Aggregation::minimum(size, capacity - front);
					const INDEX oldSecondChunk = size - oldFirstChunk;

					if (oldSecondChunk != 0)
						destroy_range<Direction::Backward>(oldData, oldData + oldSecondChunk);

					destroy_range<Direction::Backward>(oldData + front, oldData + front + oldFirstChunk);
				}
			}

			ALLOCATOR::deallocate(oldData);

			Base::set_capacity(newCapacity);
			Base::set_data(newData);
			if constexpr (RESIZE_OPERATION == ResizeOperation::SHRINK)
				Base::set_size(targetSize);
			back = targetSize;
			front = 0;
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

		constexpr void destroy() noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>) {
				const INDEX capacity = Base::get_capacity();
				T* const data = Base::get_data();
				constexpr T* const end = data + (front + Base::get_size()) % capacity;
				if (end > front)
					destroy_range<false>(data + front, end);
				else {
					destroy_range<false>(data + front, data + capacity - 1);
					destroy_range(data, end);
				}
			}
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

		constexpr void add_back(const T& data)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			const INDEX capacity = Base::get_capacity();
			copy_construct_at(Base::get_data() + back, data);
			back = (back + 1 == capacity) ? 0 : back + 1;
			++Base::size;
		}

		constexpr void add_back(T&& data)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX capacity = Base::get_capacity();
			move_construct_at(Base::get_data() + back, std::move(data));
			back = (back + 1 == capacity) ? 0 : back + 1;
			++Base::size;
		}

		constexpr void add_front(const T& data)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			const INDEX capacity = Base::get_capacity();
			const INDEX newFront = (front == 0) ? capacity - 1 : front - 1;
			move_construct_at(Base::get_data() + newFront, data);
			front = newFront;
			++Base::size;
		}

		constexpr void add_front(T&& data)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX capacity = Base::get_capacity();
			const INDEX newFront = (front == 0) ? capacity - 1 : front - 1;
			move_construct_at(Base::get_data() + newFront, std::move(data));
			front = newFront;
			++Base::size;
		}

		constexpr void remove_back() noexcept {
			const INDEX capacity = Base::get_capacity();
			back = (back == 0) ? capacity - 1 : back - 1;
			destroy_at(Base::get_data() + back);
			--Base::size;
		}

		constexpr void remove_back_many(INDEX count) noexcept {
			const INDEX capacity = Base::get_capacity();
			const INDEX newBack = (back - count) % capacity;

			T* const data = Base::get_data();
			if (newBack < back)
				destroy_range_backward(data + newBack, data + back);
			else {
				destroy_range_backward(data, data + back);
				destroy_range_backward(data + newBack, data + capacity);
			}

			back = newBack;
			Base::size -= count;
		}

		constexpr void remove_front() noexcept {
			const INDEX capacity = Base::get_capacity();
			destroy_at(Base::get_data() + front);
			front = (front + 1 == capacity) ? 0 : front + 1;
			--Base::size;
		}

		constexpr void remove_front_many(INDEX count) noexcept {
			const INDEX capacity = Base::get_capacity();
			const INDEX newFront = (front + count) % capacity;

			T* const data = Base::get_data();
			if (newFront > front)
				destroy_range<false>(data + front, data + newFront);
			else {
				destroy_range<false>(data + front, data + capacity - 1);
				destroy_range(data, data + newFront);
			}

			front = newFront;
			Base::size -= count;
		}

#pragma endregion

#pragma region Dequeue, Enqueue, & Peek

		[[nodiscard]] constexpr bool can_dequeue(INDEX count = 1) const noexcept {
			return Base::can_remove(count);
		}

		[[nodiscard]] constexpr T dequeue_back()
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX capacity = Base::get_capacity();
			back = (back == 0) ? capacity - 1 : back - 1;
			T* const address = Base::get_data() + back;
			--Base::size;
			T data = std::move(*address);
			destroy_at(address);
			return data;
		}

		constexpr void dequeue_back_many(T* const destination, INDEX count)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX capacity = Base::get_capacity();
			const INDEX newBack = back >= count ? back - count : capacity - (count - back);

			T* const data = Base::get_data();
			const INDEX firstLength = Aggregation::minimum(count, back + 1);
			if (count - firstLength == 0) {
				move_construct_range<true, false>(destination, destination + count, data + newBack + 1);
				destroy_range(data + newBack + 1, data + back + 1);
			}
			else {
				move_construct_range<true, false>(destination, destination + firstLength, data + newBack + 1);
				move_construct_range<true, false>(destination + firstLength, destination + count, data);
				destroy_range(data + newBack + 1, data + capacity);
				destroy_range(data, data + count - firstLength);
			}
		
			back = newBack;
			Base::size -= count;
		}

		[[nodiscard]] constexpr T dequeue_front()
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX capacity = Base::get_capacity();
			T* const address = Base::get_data() + front;
			front = (front + 1 == capacity) ? 0 : front + 1;
			--Base::size;
			T data = std::move(*address);
			destroy_at(address);
			return data;
		}

		constexpr void dequeue_front_many(T* const destination, INDEX count)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX capacity = Base::get_capacity();
			const INDEX newFront = front >= capacity - count ? front - (capacity - count) : front + count;

			T* const data = Base::get_data();
			const INDEX firstLength = Aggregation::minimum(count, capacity - front);
			if (count - firstLength == 0) {
				move_construct_range<true, false>(destination, destination + count, data + front);
				destroy_range(data + front, data + front + count);
			}
			else {
				move_construct_range<true, false>(destination, destination + firstLength, data + front);
				move_construct_range<true, false>(destination + firstLength, destination + count, data);
				destroy_range(data + front, data + capacity);
				destroy_range(data, data + count - firstLength);
			}
		
			front = newFront;
			Base::size -= count;
		}

		[[nodiscard]] constexpr bool can_enqueue(INDEX count = 1) const noexcept {
			return Base::can_add(count);
		}

		constexpr void enqueue_back(const T& data)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			add_back(data);
		}

		constexpr void enqueue_back(T&& data)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			add_back(std::move(data));
		}

		constexpr void enqueue_front(const T& data)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			add_front(data);
		}

		constexpr void enqueue_front(T&& data)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			add_front(std::move(data));
		}

		[[nodiscard]] constexpr bool can_peek() const noexcept {
			return !Base::is_empty();
		}

		[[nodiscard]] constexpr T& peek_back() noexcept {
			const INDEX capacity = Base::get_capacity();
			return Base::get((back == 0) ? capacity - 1 : back - 1);
		}

		[[nodiscard]] constexpr const T& peek_back() const noexcept {
			const INDEX capacity = Base::get_capacity();
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

		constexpr void double_capacity()
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			grow(capacity << 1);
		}

		constexpr void empty() noexcept {
			destroy();
			Base::set_size(0);
			back = 0;
			front = 0;
		}

		constexpr void expand(float ratio = 1.5)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			grow(Math::ceiling(capacity * ratio));
		}

		constexpr void grow(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			helper_resize<ResizeOperation::GROW>(newCapacity);
		}

		constexpr void resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity >= capacity)
				grow(newCapacity);
			else
				shrink(newCapacity);
		}

		constexpr void reserve(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity > capacity)
				grow(newCapacity);
		}

		constexpr void shrink(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			helper_resize<ResizeOperation::SHRINK>(newCapacity);
		}

#pragma endregion

#pragma endregion

	};

}
