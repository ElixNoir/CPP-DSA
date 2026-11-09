#pragma once

#include "Collection.hpp"
#include "Math.hpp"

namespace DSA {

	template <concepts::StoragePolicy ContainerConfiguration, typename T>
		requires std::is_nothrow_destructible_v<T>
	class Deque : public Collection<ContainerConfiguration, T> {
	public:

		using Base = Collection<ContainerConfiguration, T>;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

		INDEX back = 0;
		INDEX front = 0;

#pragma region Methods

		void helper_destroy(INDEX capacity = Base::default_capacity()) noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
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

		~Deque() noexcept
			requires concepts::Container<Base>
		{
			destroy();
		}

		constexpr void destroy(INDEX capacity = Base::default_capacity()) noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				helper_destroy(capacity);
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

		constexpr void add_back(const T& data, INDEX capacity = Base::default_capacity())
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			copy_construct_at(Base::get_data() + back, data);
			back = (back + 1 == capacity) ? 0 : back + 1;
			++Base::size;
		}

		constexpr void add_back(T&& data, INDEX capacity = Base::default_capacity())
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			move_construct_at(Base::get_data() + back, std::move(data));
			back = (back + 1 == capacity) ? 0 : back + 1;
			++Base::size;
		}

		constexpr void add_front(const T& data, INDEX capacity = Base::default_capacity())
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			const INDEX newFront = (front == 0) ? capacity - 1 : front - 1;
			move_construct_at(Base::get_data() + newFront, data);
			front = newFront;
			++Base::size;
		}

		constexpr void add_front(T&& data, INDEX capacity = Base::default_capacity())
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX newFront = (front == 0) ? capacity - 1 : front - 1;
			move_construct_at(Base::get_data() + newFront, std::move(data));
			front = newFront;
			++Base::size;
		}

		constexpr void remove_back(INDEX capacity = Base::default_capacity()) noexcept {
			back = (back == 0) ? capacity - 1 : back - 1;
			destroy_at(Base::get_data() + back);
			--Base::size;
		}

		constexpr void remove_back_many(INDEX count, INDEX capacity = Base::default_capacity()) noexcept {
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

		constexpr void remove_front(INDEX capacity = Base::default_capacity()) noexcept {
			destroy_at(Base::get_data() + front);
			front = (front + 1 == capacity) ? 0 : front + 1;
			--Base::size;
		}

		constexpr void remove_front_many(INDEX count, INDEX capacity = Base::default_capacity()) noexcept {
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

		[[nodiscard]] constexpr bool can_dequeue(INDEX count = 1, INDEX capacity = Base::default_capacity()) const noexcept {
			return Base::can_remove(count, capacity);
		}

		[[nodiscard]] constexpr T dequeue_back(INDEX capacity = Base::default_capacity())
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			back = (back == 0) ? capacity - 1 : back - 1;
			T* const address = Base::get_data() + back;
			--Base::size;
			T data = std::move(*address);
			destroy_at(address);
			return data;
		}

		[[nodiscard]] constexpr void dequeue_back_many(T* const destination, INDEX count, INDEX capacity = Base::default_capacity()) // TO-DO: TEST
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T* const data = Base::get_data();

			INDEX newBack = (back - count) % capacity;

			if (newBack < back) {
				move_construct_range_backward(destination, destination + count, data + newBack);
				destroy_range_backward(data + newBack, data + back);
			}
			else {
				move_construct_range_backward(destination, destination + back, data);
				move_construct_range_backward(destination + back, destination + capacity - newBack, data + newBack);
				destroy_range_backward(data, data + back);
				destroy_range_backward(data + newBack, data + capacity);
			}

			back = newBack;
			Base::size -= count;
		}

		[[nodiscard]] constexpr T dequeue_front(INDEX capacity = Base::default_capacity())
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T* const address = Base::get_data() + front;
			front = (front + 1 == capacity) ? 0 : front + 1;
			--Base::size;
			T data = std::move(*address);
			destroy_at(address);
			return data;
		}

		[[nodiscard]] constexpr void dequeue_front_many(T* const destination, INDEX count, INDEX capacity = Base::default_capacity()) // TO-DO: TEST
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			T* const data = Base::get_data();

			const INDEX newFront = (front + count) % capacity;

			if (newFront > front) {
				move_construct_range_forward<true, false>(destination, destination + count, data + front);
				destroy_range_forward<false>(data + front, data + newFront);
			}
			else {
				T* const midway = destination + capacity - front;
				move_construct_range_forward<true, false>(destination, midway - 1, data + front);
				move_construct_range_forward(midway, midway + newFront, data);
				destroy_range_forward<false>(data + front, data + capacity - 1);
				destroy_range_forward(data, data + newFront);
			}

			front = newFront;
			Base::size -= count;
		}

		[[nodiscard]] constexpr bool can_enqueue(INDEX count = 1, INDEX capacity = Base::default_capacity()) const noexcept {
			return Base::can_add(capacity, count);
		}

		constexpr void enqueue_back(const T& data, INDEX capacity = Base::default_capacity())
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			add_back(data, capacity);
		}

		constexpr void enqueue_back(T&& data, INDEX capacity = Base::default_capacity())
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			add_back(std::move(data), capacity);
		}

		constexpr void enqueue_front(const T& data, INDEX capacity = Base::default_capacity())
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			add_front(data, capacity);
		}

		constexpr void enqueue_front(T&& data, INDEX capacity = Base::default_capacity())
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			add_front(std::move(data), capacity);
		}

		[[nodiscard]] constexpr bool can_peek() const noexcept {
			return !Base::is_empty();
		}

		[[nodiscard]] constexpr T& peek_back(INDEX capacity = Base::default_capacity()) noexcept {
			return Base::get((back == 0) ? capacity - 1 : back - 1);
		}

		[[nodiscard]] constexpr const T& peek_back(INDEX capacity = Base::default_capacity()) const noexcept {
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

		void double_capacity(INDEX capacity = Base::default_capacity())
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			grow(capacity << 1);
		}

		constexpr void empty() noexcept {
			destroy();
			Base::set_size(0);
			back = 0;
			front = 0;
		}

		void grow(INDEX newCapacity, INDEX capacity = Base::default_capacity())
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
					const INDEX count = Algorithms::Math::minimum(size, capacity - front);
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

			if constexpr (concepts::DynamicContainer<Base>)
				Base::set_capacity(newCapacity);
			back = size;
			front = 0;
		}

		void reserve(INDEX newCapacity, INDEX oldCapacity = Base::default_capacity())
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			if (newCapacity > oldCapacity)
				grow(newCapacity, oldCapacity);
		}

#pragma endregion

#pragma endregion

	};

}