#pragma once

#include "Bitset.hpp"
#include "Container.hpp"
#include "../Algorithms/hash.hpp"
#include "../Algorithms/Hashes.hpp"
#include "Iterator.hpp"
#include "KeyValuePair.hpp"

#include <initializer_list>

namespace DSA { //! incomplete

	template <concepts::ContainerPolicy ContainerPolicy, concepts::KeyValuePair KEY_VALUE_PAIR>
	class StaticPerfectHashArray : public ContainerPolicy::template STRUCTURE<KEY_VALUE_PAIR> {
	public:

		using Base = ContainerPolicy::template STRUCTURE<KEY_VALUE_PAIR>;
		using typename Base::INDEX;
		using typename Base::T;

		using KEY = T::KEY;
		using VALUE = T::VALUE;

	protected:

#pragma region Properties

		size_t seed = FNV1aOffset;

#pragma endregion

	public:

#pragma region Constructors & Destructors

		constexpr StaticPerfectHashArray(std::initializer_list<T> list)
			noexcept (concepts::NothrowResizableStorage<Base>)
		{
			overwrite(list);
		}

#pragma endregion

#pragma region Methods

#pragma region Getters

		[[nodiscard]] constexpr size_t get_seed() const noexcept {
			return seed;
		}

#pragma endregion

#pragma region Contains, Find, & Insert Many

		[[nodiscard]] constexpr bool contains(KEY& key) const noexcept {
			return key == Base::get(hash_of(key)).key;
		}

		[[nodiscard]] constexpr KEY* find(KEY& key) noexcept {
			T* entry = &Base::get(hash_of(key));
			return key == entry->key ? entry : nullptr;
		}

		[[nodiscard]] constexpr const KEY* find(KEY key) const noexcept {
			const T* entry = &Base::get(hash_of(key));
			return key == entry->key ? entry : nullptr;
		}

		[[nodiscard]] constexpr INDEX hash_of(KEY key) const noexcept {
			return Algorithms::hash(key, seed) % Base::get_capacity();
		}

		[[nodiscard]] constexpr INDEX hash_of_when_capacity_power_2(KEY key) const noexcept {
			return Algorithms::hash(key, seed) & (Base::get_capacity() - 1);
		}

		constexpr void overwrite(std::initializer_list<T> list)
			noexcept (concepts::NothrowResizableStorage<Base>)
		{
			overwrite(list.begin(), list.size());
		}

		template <concepts::InputIterator<T> ITERATOR>
		constexpr void overwrite(ITERATOR begin, INDEX count)
			noexcept (concepts::NothrowResizableStorage<Base>)
		{
			const INDEX capacity = Base::get_capacity();

			Bitset<ContainerPolicy> visited(
				(capacity + Bitmask<>::bit_count - 1)
				>> Bitmask<>::bit_shift
			);

			visited.zero();

			while (true) {
				bool collision = false;

				ITERATOR current = begin;

				for (INDEX i = 0; i < count; ++i, ++current) {
					const T& entry = *current;
					const size_t index = hash_of(entry.key);

					Bitmask<>& bitmask = visited.bitmask_at(index);

					if (bitmask.get(index)) {
						collision = true;
						break;
					}

					bitmask.set(index);
				}

				if (!collision)
					break;

				++seed;
				visited.zero();
			}

			for (INDEX i = 0; i < count; ++i, ++begin) {
				const T& entry = *begin;
				Base::get(hash_of(entry.key)) = entry;
			}
		}

#pragma endregion

#pragma region Memory Management

		/*
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
		*/

#pragma endregion

#pragma region Setters

		constexpr void set_seed(INDEX newSeed) noexcept {
			seed = newSeed;
		}

#pragma endregion

#pragma endregion

	};

}