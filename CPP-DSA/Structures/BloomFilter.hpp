#pragma once

#include "Bitmask.hpp"
#include "Collection.hpp"
#include "../Algorithms/hash.hpp"

namespace DSA {

	template <concepts::ContainerPolicy ContainerPolicy, typename KEY>
	class BloomFilter : public Collection<ContainerPolicy, Bitmask<>> { //! incomplete?
	public:

		using Base = Collection<ContainerPolicy, Bitmask<>>;
		using typename Base::INDEX;
		using typename Base::T;

		constexpr static auto bit_count = T::bit_count;
		constexpr static auto bit_shift = T::bit_shift;

	protected:

#pragma region Properties

		INDEX bucket_count = 0;
		INDEX hash_count = 0;

		T* data;

#pragma endregion

#pragma region Methods

		constexpr uintmax_t hash_again(const uintmax_t hash) noexcept {
			uintmax_t result = hash ^ 0x9e3779b97f4a7c15ULL;

			result ^= result >> 30;
			result *= 0xbf58476d1ce4e5b9ULL;
			result ^= result >> 27;
			result *= 0x94d049bb133111ebULL;
			result ^= result >> 31;

			return result | 1;
		}

		constexpr void hashes(T* const destination, const KEY& key) const noexcept {
			const uintmax_t hash1 = Algorithms::hash(key);
			const uintmax_t hash2 = hash_again(hash1);
			for (size_t index = 0; index < hash_count; ++index)
				destination[index] = (hash1 + index * hash2) % bucket_count;
		}

		constexpr void hashes_power_of_2(T* const destination, const KEY& key) const noexcept {
			const uintmax_t hash1 = Algorithms::hash(key);
			const uintmax_t hash2 = hash_again(hash1);
			for (size_t index = 0; index < hash_count; ++index)
				destination[index] = (hash1 + index * hash2) & bucket_count;
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		BloomFilter(const size_t initialCapacity, const double falsePositiveProbability)
			noexcept (concepts::NothrowResizableStorage<Base>)
		: Base(bucket_count >> bit_shift),
			bucket_count(static_cast<double>(initialCapacity) * -natural_logarithm(falsePositiveProbability) / Constants::squared_natural_logarithm_of_2), hash_count(bucket_count * Constants::natural_logarithm_of_2 / initialCapacity) {}

#pragma endregion

#pragma region Methods

#pragma region Add & Contains

		constexpr void add(const KEY& key) noexcept {
			const uintmax_t hash1 = Algorithms::hash(key);
			const uintmax_t hash2 = hash_again(hash1);

			for (size_t index = 0; index < hash_count; ++index) {
				const size_t bucket = (hash1 + index * hash2) % bucket_count;
				data[bucket >> bit_shift].set(bucket & (bit_count - 1));
			}

			++Base::size;
		}

		[[nodiscard]] constexpr bool contains(const KEY& key) const noexcept {
			const uintmax_t hash1 = Algorithms::hash(key);
			const uintmax_t hash2 = hash_again(hash1);

			for (size_t index = 0; index < hash_count; ++index) {
				const size_t bucket = (hash1 + index * hash2) % bucket_count;
				if (data[bucket >> bit_shift].get(bucket & (bit_count - 1)) == 0)
					return false;
			}

			return true;
		}

		// add_many
		// contains_all
		// get_load_factor
		// get_false_positive_probability

#pragma endregion

#pragma endregion

	};

}