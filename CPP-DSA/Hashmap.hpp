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

	template <concepts::StorageConfiguration ContainerConfiguration, typename T, uintmax_t(*HASH)(const T&) = default_hash>
		requires std::is_nothrow_destructible_v<T>
	class Hashmap : public TrackedContainer<ContainerConfiguration, T> {
	public:

		using Base = TrackedContainer<ContainerConfiguration, T>;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

#pragma region Methods

		void helper_resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			using ALLOCATOR = Base::ALLOCATOR;

			const INDEX size = Base::get_size();

			T* oldData = Base::get_data();
			T* newData = ALLOCATOR::allocate();

			Base::TRACKED newTracked((newCapacity + Bitmask<uintmax_t>::BitCount - 1) >> Bitmask<uintmax_t>::BitShift);
			newTracked.zero();

			if constexpr (std::is_nothrow_move_constructible_v<T>) {
				for (INDEX index : Base::tracked.ones()) {
					T& key = oldData[index];
					const INDEX newIndex = HASH(key) % newCapacity;
					move_construct_at(newData + newIndex, std::move(key));
					if constexpr (!std::is_trivially_destructible_v<T>)
						destroy_at(newData + index);
					newTracked.set_bit_at(newIndex);
				}
			}
			else {
				try {
					for (INDEX index : Base::tracked.ones()) {
						T& key = oldData[index];
						const INDEX newIndex = HASH(key) % newCapacity;
						move_construct_at(newData + newIndex, std::move(key));
						newTracked.set_bit_at(newIndex);
					}
					if constexpr (!std::is_trivially_destructible_v<T>) {
						for (INDEX index : Base::tracked.ones()) {
							T& key = oldData[index];
							destroy_at(newData + HASH(key) % newCapacity);
						}
					}
				}
				catch (...) {
					if constexpr (!std::is_trivially_destructible_v<T>) {
						INDEX count = 0;
						for (INDEX index : Base::tracked.ones()) {
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

			Base::set_data(newData);
			Base::set_capacity(newCapacity);
			Base::tracked = newTracked;
		}

#pragma endregion

	public:

#pragma region Methods

#pragma region Find, Insert, Set, Upsert

		[[nodiscard]] constexpr INDEX find(const T& key)
			const noexcept
			requires concepts::Container<Base>
		{
			return find(key, Base::get_capacity());
		}

		[[nodiscard]] constexpr INDEX find(const T& key, INDEX capacity)
			const noexcept
			requires concepts::Storage<Base>
		{
			const T* data = Base::get_data();
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

		[[nodiscard]] constexpr INDEX find_unoccupied(const T& key)
			const noexcept
			requires concepts::Container<Base>
		{
			return find_unoccupied(key, Base::get_capacity());
		}

		[[nodiscard]] constexpr INDEX find_unoccupied(const T& key, INDEX capacity)
			const noexcept
			requires concepts::Storage<Base>
		{
			const INDEX begin = HASH(key) % capacity;

			INDEX index = Base::find_unoccupied(begin);
			if (index == capacity)
				index = Base::find_unoccupied(0);
			//if (index == capacity) // if container is used well, unreachable due to threshold in DynamicContainer
			//    return capacity;

			return index;
		}

		void insert(const T& key)
			noexcept
			requires concepts::Container<Base>
		{
			set(find_unoccupied(key), key);
			++Base::size;
		}

		void insert(const T& key, INDEX capacity)
			noexcept
			requires concepts::Storage<Base>
		{
			set(find_unoccupied(key, capacity), key);
			++Base::size;
		}

		void insert(T&& key)
			noexcept
			requires concepts::Container<Base>
		{
			set(find_unoccupied(key), std::move(key));
			++Base::size;
		}

		void insert(T&& key, INDEX capacity)
			noexcept
			requires concepts::Storage<Base>
		{
			set(find_unoccupied(key, capacity), std::move(key));
			++Base::size;
		}

		void set(INDEX index, const T& key) noexcept {
			copy_construct_at(Base::get_data() + index, key);
			Base::tracked.set_bit_at(index);
			++Base::size;
		}

		void set(INDEX index, T&& key) noexcept {
			move_construct_at(Base::get_data() + index, std::move(key));
			Base::tracked.set_bit_at(index);
			++Base::size;
		}

		void upsert(const T& key)
			noexcept
			requires concepts::Container<Base>
		{
			upsert(key, Base::get_capacity());
		}

		void upsert(const T& key, INDEX capacity)
			noexcept
			requires concepts::Storage<Base>
		{
			set(find(key, capacity), key);
		}

		void upsert(T&& key)
			noexcept
			requires concepts::Container<Base>
		{
			upsert(std::move(key), Base::get_capacity());
		}

		void upsert(T&& key, INDEX capacity)
			noexcept
			requires concepts::Storage<Base>
		{
			set(find(key, capacity), std::move(key));
		}

#pragma endregion

#pragma region Memory Management

		void double_capacity()
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicContainer<Base>
		{
			double_capacity(Base::get_capacity());
		}

		void double_capacity(INDEX oldCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			grow(oldCapacity << 1);
		}

		[[nodiscard]] constexpr bool should_grow()
			const noexcept
			requires concepts::Container<Base>
		{
			return should_grow(Base::get_capacity());
		}

		[[nodiscard]] constexpr bool should_grow(INDEX capacity)
			const noexcept
			requires concepts::Storage<Base>
		{
			return static_cast<double>(size) / capacity > 0.8;
		}

		void grow(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			helper_resize<true>(newCapacity);
		}

		void reserve(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicContainer<Base>
		{
			reserve(newCapacity, Base::get_capacity());
		}

		void reserve(INDEX newCapacity, INDEX oldCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			if (newCapacity > oldCapacity)
				grow(newCapacity);
		}

		void resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicContainer<Base>
		{
			resize(newCapacity, Base::get_capacity());
		}

		void resize(INDEX newCapacity, INDEX oldCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			if (newCapacity > oldCapacity)
				grow(newCapacity);
			else
				shrink(newCapacity);
		}

		void shrink(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			helper_resize<false>(newCapacity);
		}

#pragma endregion

#pragma endregion

	};

}