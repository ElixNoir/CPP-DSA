#pragma once

#include "Bitset.hpp"
#include "TrackedContainer.hpp"

template <typename T>
uintmax_t default_hash(const T& value) noexcept {
    constexpr size_t size = sizeof(T);
    constexpr size_t chunkSize = sizeof(uintmax_t);

    if constexpr (size <= chunkSize) {
        if constexpr (std::is_trivially_copyable_v<T>)
            return *reinterpret_cast<const uintmax_t*>(reinterpret_cast<const std::byte*>(&value));
        else {
            uintmax_t hash = 0;
            std::memcpy(&hash, &value, size);
            return hash;
        }
    }
    else {
        const std::byte* address = reinterpret_cast<const std::byte*>(&value);
        uintmax_t hash = 0;
        size_t bytes_left = size;

        while (bytes_left >= chunkSize) {
            uintmax_t chunk;
            std::memcpy(&chunk, address, chunkSize);
            hash ^= chunk;
            address += chunkSize;
            bytes_left -= chunkSize;
        }

        if (bytes_left > 0) {
            uintmax_t chunk = 0;
            std::memcpy(&chunk, address, bytes_left);
            hash ^= chunk;
        }

        return hash;
    }
}

namespace DSA {

    template <concepts::Container CONTAINER, uintmax_t(*HASH)(const CONTAINER::T&) = default_hash>
    class Hashmap : public TrackedContainer<CONTAINER> {
    public:

        using Base = TrackedContainer<CONTAINER>;
        using typename Base::INDEX;
        using typename Base::T;

    protected:

        using Base::data;
        using Base::size;
        using Base::tracked;

#pragma region Methods

        void helper_resize(INDEX newCapacity) noexcept(
            std::is_nothrow_move_constructible_v<T>
        ) requires (
            concepts::DynamicContainer<CONTAINER>
        ) {
            using ALLOCATOR = CONTAINER::ALLOCATOR;

            T* oldData = Base::get_data();
            T* newData = ALLOCATOR::allocate();

            std::remove_reference_t<decltype(tracked)> newTracked((newCapacity + Bitmask<uintmax_t>::BitCount - 1) >> Bitmask<uintmax_t>::BitShift);
            newTracked.zero();

            if constexpr (std::is_nothrow_move_constructible_v<T>) {
                for (INDEX index : tracked.ones()) {
                    T& key = oldData[index];
                    const INDEX newIndex = HASH(key) % newCapacity; // rehashing
                    move_construct_at(newData + newIndex, std::move(key));
                    if constexpr (!std::is_trivially_destructible_v<T>)
                        destroy_at(newData + index);
                    newTracked.set_bit_at(newIndex);
                }
            }
            else {
                try {
                    for (INDEX index : tracked.ones()) {
                        T& key = oldData[index];
                        const INDEX newIndex = HASH(key) % newCapacity; // rehashing
                        move_construct_at(newData + newIndex, std::move(key));
                        newTracked.set_bit_at(newIndex);
                    }
                    if constexpr (!std::is_trivially_destructible_v<T>) {
                        for (INDEX index : tracked.ones()) {
                            T& key = oldData[index];
                            destroy_at(newData + HASH(key) % newCapacity);
                        }
                    }
                }
                catch (...) {
                    if constexpr (!std::is_trivially_destructible_v<T>) {
                        INDEX count = 0;
                        for (INDEX index : tracked.ones()) {
                            if (count == size)
                                break;
                            T& key = oldData[index];
                            destroy_at(newData + HASH(key) % newCapacity);
                            count++;
                        }
                    }
                    ALLOCATOR::deallocate(newData);
                    throw;
                }
            }

            data = reinterpret_cast<std::byte*>(newData);
            Base::capacity = newCapacity;
            tracked = newTracked;
        }

#pragma endregion

    public:

#pragma region Find, Insert, Set, Upsert

        [[nodiscard]] constexpr INDEX find(const T& key) const noexcept {
            const T* data = Base::get_data();
            const INDEX capacity = Base::get_capacity();
            const INDEX begin = HASH(key) % capacity;

            INDEX index = begin;
            for (; index < capacity; index++)
                if (key == data[index])
                    return index;

            index = 0;
            for (; index < begin; index++)
                if (key == data[index])
                    return index;

            return capacity; // if container is used well, unreachable due to threshold in DynamicContainer
        }

        [[nodiscard]] constexpr INDEX find_unoccupied(const T& key) const noexcept {
            const INDEX capacity = Base::get_capacity();
            const INDEX begin = HASH(key) % capacity;

            INDEX index = Base::find_unoccupied(begin);
            if (index == capacity)
                index = Base::find_unoccupied(0);
            //if (index == capacity) // if container is used well, unreachable due to threshold in DynamicContainer
            //    return capacity;

            return index;
        }

        void insert(const T& key) noexcept {
            set(find_unoccupied(key), key);
            size++;
        }

        void insert(T&& key) noexcept {
            set(find_unoccupied(key), std::move(key));
            size++;
        }

        void set(INDEX index, const T& key) noexcept {
            copy_construct_at(Base::get_data() + index, key);
            Base::set_tracked(index);
            size++;
        }

        void set(INDEX index, T&& key) noexcept {
            move_construct_at(Base::get_data() + index, std::move(key));
            Base::set_tracked(index);
            size++;
        }

        void upsert(const T& key) noexcept {
            set(find(key), key);
        }

        void upsert(T&& key) noexcept {
            set(find(key), std::move(key));
        }

#pragma endregion

#pragma region Memory Management

        [[nodiscard]] constexpr bool should_grow() const noexcept {
            return static_cast<double>(size) / Base::get_capacity() > 0.8;
        }

        void double_capacity() noexcept(
            concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
            && std::is_nothrow_move_constructible_v<T>
        ) requires (
            concepts::DynamicContainer<CONTAINER>
        ) {
            if constexpr (concepts::ResizableContainer<CONTAINER>)
                Base::double_capacity();
            else
                grow(Base::get_capacity() << 1);
        }

        void grow(INDEX newCapacity) noexcept(
            concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
            && std::is_nothrow_move_constructible_v<T>
        ) requires (
            concepts::DynamicContainer<CONTAINER>
        ) {
            if constexpr (concepts::ResizableContainer<CONTAINER>)
                Base::grow(newCapacity);
            else
                helper_resize<true>(newCapacity);
        }

        void reserve(INDEX newCapacity) noexcept(
            concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
            && std::is_nothrow_move_constructible_v<T>
        ) requires (
            concepts::DynamicContainer<CONTAINER>
        ) {
            if constexpr (concepts::ResizableContainer<CONTAINER>)
                Base::reserve(newCapacity);
            else if (newCapacity > Base::get_capacity())
                grow(newCapacity);
        }

        void resize(INDEX newCapacity) noexcept(
            concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
            && std::is_nothrow_move_constructible_v<T>
        ) requires (
            concepts::DynamicContainer<CONTAINER>
        ) {
            if constexpr (concepts::ResizableContainer<CONTAINER>)
                Base::resize(newCapacity);
            else if (newCapacity > Base::get_capacity())
                grow(newCapacity);
            else
                shrink(newCapacity);
        }

        void shrink(INDEX newCapacity) noexcept(
            concepts::NothrowResizableAllocator<CONTAINER::ALLOCATOR>
            && std::is_nothrow_move_constructible_v<T>
        ) requires (
            concepts::DynamicContainer<CONTAINER>
        ) {
            if constexpr (concepts::ResizableContainer<CONTAINER>)
                Base::shrink(newCapacity);
            else
                helper_resize<false>(newCapacity);
        }

#pragma endregion

    };

}