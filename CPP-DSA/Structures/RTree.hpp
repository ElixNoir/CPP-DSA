#pragma once

#include <array>

#include "Bitset.hpp"
#include "Bounds2.hpp"
#include "../Stack.hpp"

/*
namespace DSA {

	template <concepts::ContainerPolicy ContainerPolicy, typename VALUE_TYPE, typename PRECISION_TYPE = double, size_t NODE_CAPACITY = 16>
	class RTree : public Collection<ContainerPolicy, > {
	public:

		using Base = Collection<ContainerPolicy, T>;
		using typename Base::INDEX;

		using Height = uint16_t;

		struct RTreeNode {

			//static constexpr size_t MINIMUM_SIZE = NODE_CAPACITY >> 1;

			struct Entry {

				Bounds2<PRECISION_TYPE> plane;

				union {

					INDEX child_index;
					VALUE_TYPE value;

				};

			};

			std::array<Entry, NODE_CAPACITY + 1> entries;
			uint16_t count = 0;

		};

		constexpr static INDEX invalid_index = Base::invalid_index;

	protected:

		INDEX rootIndex = invalid_index;
		Bitset<ContainerPolicy> leaves;

	public:

#pragma region Methods

#pragma region Getters

		[[nodiscard]] constexpr Height get_height() const noexcept {

		}

		[[nodiscard]] constexpr INDEX get_root_index() const noexcept {
			return rootIndex;
		}

#pragma endregion

#pragma region Add & Remove

#pragma endregion

#pragma region Insert & Upsert

#pragma endregion

#pragma region Queries

		[[nodiscard]] constexpr bool can_query() const noexcept {
			return !Base::is_empty();
		}

		template <typename OPERATION>
			requires std::invocable<OPERATION, const VALUE_TYPE&>
		constexpr void query(const Bounds2<PRECISION_TYPE>& bounds, OPERATION&& operation) const {
			Stack<DSA::ContainerPolicy<InlineStoragePolicy<64>>, INDEX> stack;
			stack.push(rootIndex);

			while (stack.can_pop()) {
				const auto& node = Base::get(stack.pop());
				for (uint16_t index = 0; index < node.count; ++index) {
					const Entry& entry = node.entries[index];
					if (!entry.plane.intersects(bounds))
						continue;

					if (node.is_leaf)
						operation(entry.value);
					else
						stack.push(entry.child_index);
				}
			}
		}

		template <typename OPERATION>
			requires std::invocable<OPERATION, const VALUE_TYPE&>
		constexpr void nearest(const Bounds2<PRECISION_TYPE>& bounds, OPERATION&& operation) const noexcept {
			
		}

#pragma endregion

#pragma region Memory Management

		constexpr void empty() noexcept {

		}

#pragma endregion

#pragma endregion

	};

}
*/
