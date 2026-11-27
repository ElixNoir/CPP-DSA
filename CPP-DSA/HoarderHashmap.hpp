#pragma once

#include "hash.hpp"
#include "TrackedContainer.hpp"

namespace DSA {

	template <concepts::ContainerPolicy ContainerPolicy, typename T_>
		requires std::is_nothrow_destructible_v<T_>
	class Hashmap : public TrackedContainer<ContainerPolicy, T_> {
	public:

		using Base = TrackedContainer<ContainerPolicy, T_>;
		using typename Base::ALLOCATOR;
		using typename Base::INDEX;
		using typename Base::T;

		constexpr static bool is_dynamic = Base::is_dynamic;

	protected:

#pragma region Methods

		template <ResizeOperation RESIZE_OPERATION>
		void helper_resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			const INDEX newBitsetCapacity = (newCapacity + Bitmask<uintmax_t>::bit_count - 1) >> Bitmask<uintmax_t>::bit_shift;
			typename Base::TRACKED newTracked(newBitsetCapacity);
			newTracked.zero(newBitsetCapacity);

			T* const newData = ALLOCATOR::allocate(newCapacity);
			T* const oldData = Base::get_data();
			const INDEX size = Base::get_size();

			if constexpr (std::is_nothrow_move_constructible_v<T>) {
				for (const INDEX index : Base::tracked.ones()) {
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
					for (const INDEX index : Base::tracked.ones()) {
						T& key = oldData[index];
						const INDEX newIndex = Algorithms::hash(key) % newCapacity;
						move_construct_at(newData + newIndex, std::move(key));
						newTracked.set_bit_at(newIndex);
					}
					if constexpr (!std::is_trivially_destructible_v<T>) {
						for (const INDEX index : Base::tracked.ones(newBitsetCapacity)) {
							T& key = oldData[index];
							destroy_at(newData + Algorithms::hash(key) % newCapacity);
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
							destroy_at(newData + Algorithms::hash(key) % newCapacity);
							++count;
						}
					}
					ALLOCATOR::deallocate(newData);
					throw;
				}
			}

			Base::set_capacity(newCapacity);
			Base::set_data(newData);
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

		constexpr void destroy() noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>) {
				T* const data = Base::get_data();
				for (INDEX index : Base::tracked.ones())
					destroy_at(data + index);
			}
		}

#pragma endregion

#pragma region Methods

#pragma region Find, Insert, Upsert

		[[nodiscard]] constexpr INDEX find_occupied(const T& key) const noexcept {
			const INDEX capacity = Base::get_capacity();
			const T* data = Base::get_data();
			const INDEX begin = Algorithms::hash(key) % capacity;

			INDEX index = begin;
			for (; index < capacity; ++index)
				if (key == data[index])
					return index;

			index = 0;
			for (; index < begin; ++index)
				if (key == data[index])
					return index;

			return capacity; // if container is used well, unreachable due to threshold in DynamicContainer
		}

		[[nodiscard]] constexpr INDEX find_unoccupied(const T& key) const noexcept {
			const INDEX capacity = Base::get_capacity();
			const INDEX begin = Algorithms::hash(key) % capacity;

			INDEX index = Base::find_first_untracked(begin, capacity);
			if (index == capacity)
				index = Base::find_unoccupied(0, capacity);
			//if (index == capacity) // if container is used well, unreachable due to threshold in DynamicContainer
			//    return capacity;

			return index;
		}

		constexpr void insert(const T& key) noexcept {
			const INDEX capacity = Base::get_capacity();
			insert_at(find_unoccupied(key, capacity), key);
		}

		constexpr void insert(T&& key) noexcept {
			const INDEX capacity = Base::get_capacity();
			insert_at(find_unoccupied(key, capacity), std::move(key));
		}

		constexpr INDEX insert_at(const T key, INDEX index) noexcept {
			copy_construct_at(Base::get_data() + index, key);
			Base::tracked.set_bit_at(index);
			++Base::size;
			return index;
		}

		constexpr INDEX insert_at(T&& key, INDEX index) noexcept {
			move_construct_at(Base::get_data() + index, std::move(key));
			Base::tracked.set_bit_at(index);
			++Base::size;
			return index;
		}

		constexpr INDEX upsert(const T key) noexcept {
			const INDEX capacity = Base::get_capacity();
			const INDEX index = find_occupied(key);
			if (index == capacity)
				return insert(key);
			return index;
		}

		constexpr INDEX upsert(T&& key) noexcept {
			const INDEX capacity = Base::get_capacity();
			const INDEX index = find_occupied(std::move(key));
			if (index == capacity)
				return insert(std::move(key));
			return index;
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

		constexpr void expand(float ratio = 1.5)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			grow(Math::ceiling(capacity * ratio), capacity);
		}

		[[nodiscard]] constexpr bool should_grow() const noexcept {
			const INDEX capacity = Base::get_capacity();
			return static_cast<double>(Base::get_size()) / capacity > 0.8;
		}

		constexpr void grow(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			helper_resize<ResizeOperation::GROW>(newCapacity);
		}

		constexpr void reserve(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity > capacity)
				grow(newCapacity);
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
