#pragma once

#include "Aggregation.hpp"
#include "Bitset.hpp"
#include "Collection.hpp"
#include "hash.hpp"
#include "Math.hpp"

#include <iostream>

namespace DSA {

	template <concepts::ContainerPolicy ContainerPolicy, typename _T>
	class PointerHashmap : public Collection<ContainerPolicy, _T*> {
	public:

		using Base = Collection<ContainerPolicy, _T*>;
		using typename Base::ALLOCATOR;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

		using BitsetPolicy = std::conditional_t<
			concepts::DynamicStoragePolicy<ContainerPolicy>,
			HeapStoragePolicy<INDEX, typename ContainerPolicy::ALLOCATOR_TEMPLATE>,
			InlineStoragePolicy<((ContainerPolicy::maximum_capacity + Bitmask<>::bit_count - 1) >> Bitmask<>::bit_shift)>
		>;

		Bitset<BitsetPolicy> key_states;

#pragma region Methods

		template <ResizeOperation RESIZE_OPERATION>
			requires (RESIZE_OPERATION != ResizeOperation::UNKNOWN)
		constexpr void helper_resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::get_capacity();

			Bitset<BitsetPolicy> newKeyStates((newCapacity + Bitmask<>::bit_count - 1) >> Bitmask<>::bit_shift);
			newKeyStates.zero();

			const INDEX size = Base::get_size();
			const T* newData = ALLOCATOR::allocate(newCapacity);
			const T* oldData = Base::get_data();

			INDEX count;
			if constexpr (RESIZE_OPERATION == ResizeOperation::GROW)
				count = size;
			else
				count = Aggregation::minimum(newCapacity, size);

			for (INDEX index : key_states.ones()) {
				if (count == 0)
					break;
				T key = oldData[index];
				const INDEX newIndex = Algorithms::hash_pointer(key) % newCapacity;
				newData[newIndex] = key;
				newKeyStates.set_bit_at(newIndex);
				--count;
			}

			if constexpr (std::is_nothrow_move_assignable_v<Bitset<BitsetPolicy>>)
				key_states = std::move(newKeyStates);
			else {
				try {
					key_states = std::move(newKeyStates);
				}
				catch (...) {
					ALLOCATOR::deallocate(newData);
					throw;
				}
			}

			ALLOCATOR::deallocate(oldData);

			Base::set_capacity(newCapacity);
			Base::set_data(newData);
		}

#pragma endregion
		 
	public:

#pragma region Constructors & Destructors

		~PointerHashmap() noexcept = default;

		~PointerHashmap() noexcept
			requires concepts::HasCapacity<Base>
		{
			destroy();
		}

		constexpr void destroy() noexcept {
			const INDEX capacity = Base::get_capacity();
			//constexpr INDEX keyStatesCapacity = (capacity + Bitmask<>::bit_count - 1) >> Bitmask<>::bit_shift;
			key_states.destroy();
		}

#pragma endregion

#pragma region Methods

#pragma region Find, Insert, Remove, & Upsert

		[[nodiscard]] constexpr INDEX find_occupied(const T key) const noexcept {
			const INDEX capacity = Base::get_capacity();
			const INDEX begin = Algorithms::hash_pointer(key) % capacity;
			const T data = Base::get_data();

			INDEX index = begin;
			for (; index < capacity; ++index) {
				const T other = data[index];
				const Bitmask<>::T q = key_states.get_bit_at(index);
				if ((q == 0) & (other == nullptr)) // if empty...
					return capacity;
				if (*key == *other) // if not empty and they are the same...
					return q ? index : capacity; // if not deleted, then return a valid index; otherwise, return an invalid index
			}

			// if nothing was determined...
			for (index = 0; index < begin; ++index) { // wrap around
				const T other = data[index];
				const Bitmask<>::T q = key_states.get_bit_at(index);
				if ((q == 0) & (other == nullptr))
					return capacity;
				if (*key == *other)
					return q ? index : capacity;
			}

			return capacity;
		}

		[[nodiscard]] constexpr INDEX find_unoccupied(const T key) const noexcept {
			const INDEX capacity = Base::get_capacity();
			const INDEX begin = Algorithms::hash_pointer(key) % capacity;

			// the first zero bit is guaranteed to be either deleted or empty
			INDEX index = key_states.find_first_bit_at_or_after<true>(begin);//, { capacity });
			if (index == capacity) // if no zero bit was found...
				index = key_states.find_first_bit<true>(0, begin >> Bitmask<>::bit_shift); // wrap around

			return index;
		}

		constexpr INDEX insert(const T key) noexcept {
			return insert_at(find_unoccupied(key));
		}

		constexpr INDEX insert_at(const T key, INDEX index) noexcept {
			Base::get(index) = key;
			key_states.set_bit_at(index);
			++Base::size;
			return index;
		}

		constexpr void remove(const T key) noexcept {
			const INDEX capacity = Base::get_capacity();
			const INDEX index = find_occupied(key);
			if (index == capacity)
				return;
			remove_at(index);
		}

		constexpr void remove_at(INDEX index) noexcept {
			key_states.reset_bit_at(index);
		}

		constexpr INDEX upsert(const T key) noexcept {
			const INDEX capacity = Base::get_capacity();
			const INDEX index = find_occupied(key);
			if (index == capacity)
				return insert(key);
			return index;
		}

#pragma endregion

#pragma region Memory Management

		constexpr void double_capacity()
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::get_capacity();
			grow(capacity << 1);
		}

		constexpr void expand(float ratio = 1.5)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::get_capacity();
			grow(Math::ceiling(capacity * ratio));
		}

		constexpr void empty()
			noexcept (concepts::NothrowResizableStorage<Base>)
		{
			key_states.zero();
			Base::set_size(0);
		}

		constexpr void grow(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires concepts::DynamicStorage<Base>
		{
			helper_resize(newCapacity);
		}

		constexpr void reserve(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires concepts::DynamicStorage<Base>
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity >= capacity)
				grow(newCapacity);
		}

#pragma endregion

#pragma region Iteration



#pragma endregion

#pragma region Operators

		/*friend std::ostream& operator<<(std::ostream& ostream, const Self& t) noexcept
			requires concepts::HasCapacity<Self>
		{
			const INDEX capacity = t.get_capacity();
			for (auto index : t.key_states.ones((capacity + Bitmask<>::bit_count - 1) >> Bitmask<>::bit_shift))
				ostream << "{ " << index << ", " << (*t.get(index)) << " }\n";
			return ostream;
		}*/

#pragma endregion

#pragma endregion

	};

}
