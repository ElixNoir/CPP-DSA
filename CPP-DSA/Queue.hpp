#pragma once

#pragma region Dependencies

#include "IncrementalContainer.hpp"

#pragma endregion

namespace DSA {

    template <concepts::Container CONTAINER>
        requires std::is_nothrow_destructible_v<typename CONTAINER::T>
    class Queue : IncrementalContainer<CONTAINER> {
    public:

        using Base = IncrementalContainer<CONTAINER>;
        using typename Base::INDEX;
        using typename Base::T;

    protected:

        using Base::data;
        using Base::size;

        INDEX back = 0;
        INDEX front = 0;

    public:

#pragma region Methods

#pragma region Queue

#pragma region Getters

        [[nodiscard]] constexpr INDEX get_back() const noexcept {
            return back;
        }

        [[nodiscard]] constexpr INDEX get_front() const noexcept {
            return front;
        }

#pragma endregion

#pragma region Memory Management

        void double_capacity() noexcept(
            concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
            && std::is_nothrow_move_constructible_v<T>
        ) requires (
            concepts::DynamicContainer<CONTAINER>
        ) {
            grow(2 * newCapacity);
        }

        void grow(INDEX newCapacity) noexcept(
            concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
            && std::is_nothrow_move_constructible_v<T>
        ) requires (
            concepts::DynamicContainer<CONTAINER>
        ) {
            using ALLOCATOR = CONTAINER::ALLOCATOR;

            if constexpr (concepts::ReallocatableAllocator<ALLOCATOR> && std::is_trivially_copyable_v<T>)
                data = reinterpret_cast<std::byte*>(ALLOCATOR::reallocate(Base::get_data(), newCapacity));
            else {
                T* oldData = Base::get_data();
                T* newData = ALLOCATOR::allocate(newCapacity);

                try {
                    if (back > front)
                        move_construct_range_forward<false>(newData + front, oldData + front, back);
                    else if (size != 0) {
                        const INDEX oldCapacity = Base::get_capacity();
                        move_construct_range_forward<false>(newData + front, oldData + front, oldCapacity - front);
                        move_construct_range_forward(newData + oldCapacity, oldData + oldCapacity, back);
                    }
                }
                catch (...) {
                    ALLOCATOR::deallocate(newData);
                    throw;
                }

                ALLOCATOR::deallocate(oldData);
                data = reinterpret_cast<std::byte*>(newData);
            }

            Base::capacity = newCapacity;
            back = size;
            front = 0;
        }

        void reserve(INDEX newCapacity) noexcept(
            concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
            && std::is_nothrow_move_constructible_v<T>
        ) requires (
            concepts::DynamicContainer<CONTAINER>
        ) {
            if (newCapacity > Base::get_capacity())
                grow(newCapacity);
        }

#pragma endregion

#pragma region Add & Remove

        void add_back(const T& value) noexcept(
            std::is_nothrow_copy_constructible_v<T>
        ) {
            copy_construct_at(Base::get_data() + back, value);
            back = (back + 1) % Base::get_capacity();
            size++;
        }

        void add_back(T&& value) noexcept(
            std::is_nothrow_move_constructible_v<T>
        ) {
            move_construct_at(Base::get_data() + back, move(value));
            back = (back + 1) % Base::get_capacity();
            size++;
        }

        void add_front(const T& value) noexcept(
            std::is_nothrow_copy_constructible_v<T>
        ) {
            copy_construct_at(Base::get_data() + front, value);
            front = (front - 1) % Base::get_capacity();
            size++;
        }

        void add_front(T&& value) noexcept(
            std::is_nothrow_move_constructible_v<T>
        ) {
            move_construct_at(Base::get_data() + front, move(value));
            front = (front - 1) % Base::get_capacity();
            size++;
        }

        void remove_back() noexcept {
            back = (back - 1) % Base::get_capacity();
            size--;
            destroy_at(Base::get_data() + back);
        }

        void remove_back_many(INDEX count) noexcept {
            const INDEX capacity = Base::get_capacity();
            T* const data = Base::get_data();

            INDEX newBack = (back - count) % capacity;

            if (newBack < back)
                destroy_range_backward(data + newBack, data + back);
            else {
                destroy_range_backward(data, data + back);
                destroy_range_backward(data + newBack, data + capacity);
            }

            back = newBack;
            size -= count;
        }

        void remove_front() noexcept {
            front = (front + 1) % Base::get_capacity();
            size--;
            destroy_at(Base::get_data() + front);
        }

        void remove_front_many(INDEX count) noexcept {
            const INDEX capacity = Base::get_capacity();
            T* const data = Base::get_data();

            INDEX newFront = (front + count) % capacity;

            if (newFront > front)
                destroy_range_forward<false>(data + front, data + newFront);
            else {
                destroy_range_forward<false>(data + front, data + capacity - 1);
                destroy_range_forward(data, data + newFront);
            }

            front = newFront;
            size -= count;
        }

#pragma endregion

#pragma region Dequeue, Enqueue, & Peek

        [[nodiscard]] constexpr bool can_dequeue(INDEX count = 1) const noexcept {
            return Base::can_remove(count);
        }

        [[nodiscard]] T dequeue_back() noexcept(
            std::is_nothrow_move_constructible_v<T>
        ) {
            back = (back - 1) % Base::get_capacity();
            T* const address = Base::get_data() + back;
            size--;
            T value = move(*address);
            destroy_at(address);
            return value;
        }

        [[nodiscard]] constexpr void dequeue_back_many(T* destination, INDEX count) noexcept(
            std::is_nothrow_move_constructible_v<T>
        ) {
            const INDEX capacity = Base::get_capacity();
            T* const data = Base::get_data();

            INDEX newBack = (back - count) % capacity;

            if (newBack < back) {
                move_construct_range_backward(destination, data + newBack, count);
                destroy_range_backward(data + newBack, data + back);
            }
            else {
                move_construct_range_backward(destination, data, back);
                move_construct_range_backward(destination + back, data + newBack, capacity - newBack);
                destroy_range_backward(data, data + back);
                destroy_range_backward(data + newBack, data + capacity);
            }

            back = newBack;
            size -= count;
        }

        [[nodiscard]] T dequeue_front() noexcept(
            std::is_nothrow_move_constructible_v<T>
        ) {
            front = (front + 1) % Base::get_capacity();
            T* const address = Base::get_data() + front;
            size--;
            T value = move(*address);
            destroy_at(address);
            return value;
        }

        [[nodiscard]] constexpr void dequeue_front_many(T* destination, INDEX count) noexcept(
            std::is_nothrow_move_constructible_v<T>
        ) {
            const INDEX capacity = Base::get_capacity();
            T* const data = Base::get_data();

            INDEX newFront = (front + count) % capacity;

            if (newFront > front) {
                move_construct_range_backward<false>(destination, data + front, count);
                destroy_range_forward<false>(data + front, data + newFront);
            }
            else {
                move_construct_range_backward<false>(destination, data + front, capacity - front - 1);
                move_construct_range_backward(destination + capacity - front, data, newFront);
                destroy_range_forward<false>(data + front, data + capacity - 1);
                destroy_range_forward(data, data + newFront);
            }

            front = newFront;
            size -= count;
        }

        [[nodiscard]] constexpr bool can_enqueue(INDEX count = 1) const noexcept {
            return Base::can_add(count);
        }

        void enqueue_back(const T& value) noexcept(
            std::is_nothrow_copy_constructible_v<T>
        ) {
            add_back(value);
        }

        void enqueue_back(T&& value) noexcept(
            std::is_nothrow_move_constructible_v<T>
        ) {
            add_back(move(value));
        }

        void enqueue_front(const T& value) noexcept(
            std::is_nothrow_copy_constructible_v<T>
        ) {
            add_front(value);
        }

        void enqueue_front(T&& value) noexcept(
            std::is_nothrow_move_constructible_v<T>
        ) {
            add_front(move(value));
        }

        [[nodiscard]] constexpr bool can_peek(INDEX count = 1) const noexcept {
            return !Base::is_empty();
        }

        [[nodiscard]] constexpr T& peek_back() noexcept {
            return Base::get_data()[back];
        }

        [[nodiscard]] constexpr const T& peek_back() const noexcept {
            return Base::get_data()[back];
        }

        [[nodiscard]] constexpr T& peek_front() noexcept {
            return Base::get_data()[front];
        }

        [[nodiscard]] constexpr const T& peek_front() const noexcept {
            return Base::get_data()[front];
        }

#pragma endregion

#pragma endregion

    };

}