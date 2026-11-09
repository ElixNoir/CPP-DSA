#pragma once

#include "TrackedContainer.hpp"

namespace Algorithms {

	namespace Hashing {

		template <typename T>
		constexpr static uintmax_t default_hash(const T& data) noexcept {
			constexpr size_t size = sizeof(T);
			constexpr size_t chunkSize = sizeof(uintmax_t);

			if constexpr (size <= chunkSize) {
				if constexpr (std::is_trivially_copyable_v<T>)
					return *reinterpret_cast<const uintmax_t*>(reinterpret_cast<const std::byte*>(&data));
				else {
					uintmax_t hash = 0;
					std::memcpy(&hash, &data, size);
					return hash;
				}
			}
			else {
				const std::byte* address = reinterpret_cast<const std::byte*>(&data);
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

	}

}

namespace DSA {

	template <concepts::StoragePolicy ContainerConfiguration, typename T, uintmax_t(*HASH)(const T&) = Algorithms::Hashing::default_hash>
		requires std::is_nothrow_destructible_v<T>
	class Hashmap : public TrackedContainer<ContainerConfiguration, T> {
	public:

		using Base = TrackedContainer<ContainerConfiguration, T>;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

#pragma region Methods

		void helper_resize(INDEX newCapacity, Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			using ALLOCATOR = Base::ALLOCATOR;

			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			const INDEX newBitsetCapacity = (newCapacity + Bitmask<uintmax_t>::BitCount - 1) >> Bitmask<uintmax_t>::BitShift;
			Base::TRACKED newTracked(newBitsetCapacity);
			newTracked.zero(newBitsetCapacity);
			const INDEX oldBitsetCapacity = (capacity + Bitmask<uintmax_t>::BitCount - 1) >> Bitmask<uintmax_t>::BitShift;

			T* const newData = ALLOCATOR::allocate(newCapacity);
			T* const oldData = Base::get_data();
			const INDEX size = Base::get_size();

			if constexpr (std::is_nothrow_move_constructible_v<T>) {
				for (INDEX index : Base::tracked.ones(oldBitsetCapacity)) {
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
					for (INDEX index : Base::tracked.ones(oldBitsetCapacity)) {
						T& key = oldData[index];
						const INDEX newIndex = HASH(key) % newCapacity;
						move_construct_at(newData + newIndex, std::move(key));
						newTracked.set_bit_at(newIndex);
					}
					if constexpr (!std::is_trivially_destructible_v<T>) {
						for (INDEX index : Base::tracked.ones(newBitsetCapacity)) {
							T& key = oldData[index];
							destroy_at(newData + HASH(key) % newCapacity);
						}
					}
				}
				catch (...) {
					if constexpr (!std::is_trivially_destructible_v<T>) {
						INDEX count = 0;
						for (INDEX index : Base::tracked.ones(newBitsetCapacity)) {
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
			if constexpr (concepts::HasMutableCapacity<Base>)
				Base::set_capacity(newCapacity);
			Base::tracked = std::move(newTracked);
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		~Hashmap() noexcept = default;

		~Hashmap() noexcept
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

#pragma region Find, Insert, Set, Upsert

		[[nodiscard]] constexpr INDEX find(const T& key, Base::Capacity explicitCapacity = {}) const noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
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

		[[nodiscard]] constexpr INDEX find_unoccupied(const T& key, Base::Capacity explicitCapacity = {}) const noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			const INDEX begin = HASH(key) % capacity;

			INDEX index = Base::find_first_untracked(begin, explicitCapacity);
			if (index == capacity)
				index = Base::find_unoccupied(0, explicitCapacity);
			//if (index == capacity) // if container is used well, unreachable due to threshold in DynamicContainer
			//    return capacity;

			return index;
		}

		void insert(const T& key, Base::Capacity explicitCapacity = {}) noexcept {
			set(find_unoccupied(key, explicitCapacity), key);
			++Base::size;
		}

		void insert(T&& key, Base::Capacity explicitCapacity = {}) noexcept {
			set(find_unoccupied(key, explicitCapacity), std::move(key));
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

		void upsert(const T& key, Base::Capacity explicitCapacity = {}) noexcept {
			set(find(key, explicitCapacity), key);
		}

		void upsert(T&& key, Base::Capacity explicitCapacity = {}) noexcept {
			set(find(key, explicitCapacity), std::move(key));
		}

#pragma endregion

#pragma region Memory Management

		void double_capacity(Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			grow(capacity << 1);
		}

		[[nodiscard]] constexpr bool should_grow(Base::Capacity explicitCapacity = {}) const noexcept {
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			return static_cast<double>(Base::get_size()) / capacity > 0.8;
		}

		void grow(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			helper_resize<true>(newCapacity);
		}

		void reserve(INDEX newCapacity, Base::Capacity explicitCapacity = {})
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
			if (newCapacity > oldCapacity)
				grow(newCapacity);
		}

		void resize(INDEX newCapacity, Base::Capacity explicitCapacity = {}))
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::extract_capacity(explicitCapacity);
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
