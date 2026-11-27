#pragma once

#pragma region Dependencies

#include "String.hpp"
#include "BinaryTree.hpp"

#pragma endregion

namespace DSA {

	namespace concepts {

		template <typename T>
		concept PatriciaTreeNode = BinaryTreeNode<T> && requires(T t) {

			typename T::T;

			{ t.bit_index } -> std::same_as<uint32_t>;
			{ t.key } -> std::same_as<typename T::T*>;

		};

		template <typename T>
		concept PatriciaTreeKey = HasSize<T>;

	}

	template <concepts::PatriciaTreeKey T_, std::unsigned_integral NODE_REFERENCE_TYPE = uint32_t>
	struct PatriciaTreeNode : public BinaryTreeNode<NODE_REFERENCE_TYPE> {

		using T = T_;

#pragma region Properties

		uint32_t bit_index = 0;
		T key;

#pragma endregion

	};

	template <concepts::ContainerPolicy ContainerPolicy, typename T_ = String>
	class PatriciaTree : public BinaryTree<ContainerPolicy, PatriciaTreeNode<T_, typename ContainerPolicy::INDEX>> { //! incomplete
	public:

		using Base = BinaryTree<ContainerPolicy, PatriciaTreeNode<typename ContainerPolicy::INDEX>>;
		using typename Base::INDEX;
		using typename Base::T;

		using KEY = T::T;

	protected:

#pragma region Methods

		constexpr static bool get_bit(const KEY key, const uint32_t bitIndex) noexcept {
			size_t byte_pos = static_cast<size_t>(bitIndex) >> 3;
			if (byte_pos >= key.get_size())
				return false;
			return (key[byte_pos] >> (7 - (bitIndex & 7))) & 1;
		}

		constexpr static uint32_t first_differing_bit(const KEY key1, const KEY key2) noexcept {
			const size_t maxBits = ((key1.get_size() > key2.get_size() ? key1.get_size() : key2.get_size()) + 1) << 3;
			for (size_t bitIndex = 0; bitIndex < maxBits; ++bitIndex)
				if (get_bit(key1, bitIndex) != get_bit(key2, bitIndex))
					return bitIndex;

			return -1;
		}

		[[nodiscard]] constexpr INDEX search_closest(const KEY key, const INDEX begin) const noexcept {
			INDEX index = begin;
			int previousBit = -1;

			while (index != Base::invalid_index) {
				const T& node = Base::get(index);
				if (node.data.bit_index <= previousBit)
					break;
				previousBit = node.data.bit_index;
				index = get_bit(key, previousBit) ? node.right : node.left;
			}

			return index;
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		using Base::Base;

#pragma endregion

#pragma region Methods

#pragma region Contains, Insert, & Remove

		[[nodiscard]] constexpr bool contains(const KEY& key) const noexcept {
			if (Base::root == Base::invalid_index)
				return false;

			INDEX closest = search_closest(key, Base::root);
			return closest != Base::invalid_index && key == Base::get(closest);
		}

		constexpr void add(const KEY& key) noexcept {
			if (Base::root == Base::invalid_index) {
				INDEX newRoot = Base::allocate();
				Base::root = newRoot;
				Base::get(newRoot) = T{
					newRoot,
					newRoot,
					0,
					key
				};
				return;
			}

			INDEX closest = search_closest(key, Base::root);
			if (key == Base::get(closest))
				return;
			uint32_t diffBit = first_differing_bit(key, closest.data.key);

			INDEX parent = Base::invalid_index;
			INDEX index = Base::root;
			uint32_t previousBit = -1;

			while (index != Base::invalid_index) {
				const T& node = Base::get(index);
				uint32_t bitIndex = node.data.bit_index;
				if (bitIndex <= previousBit || bitIndex >= diffBit)
					break;

				previousBit = bitIndex;
				parent = index;
				index = get_bit(key, bitIndex) ? node.right : node.left;
			}

			INDEX newIndex = Base::allocate();
			uint32_t branchBit = get_bit(key, diffBit);

			Base::get(newIndex) = T{
				branchBit ? index : newIndex,
				branchBit ? newIndex : index,
				diffBit,
				key
			};

			if (parent == Base::invalid_index)
				Base::root = newIndex;
			else {
				if (T& parent_node = Base::get(parent); get_bit(key, parent_node.data.bit_index))
					parent_node.right = newIndex;
				else
					parent_node.left = newIndex;
			}
		}

		constexpr bool remove(const KEY& key) noexcept {
			INDEX backParent = Base::invalid_index;
			INDEX index = Base::root;
			uint32_t previousBit = -1;

			while (index != Base::invalid_index) {
				const T& node = Base::get(index);
				if (node.data.bit_index <= previousBit)
					break;
				previousBit = node.data.bit_index;
				backParent = index;
				index = get_bit(key, previousBit) ? node.right : node.left;
			}

			INDEX target = index;
			if (target == Base::invalid_index)
				return false;

			const T& targetNode = Base::get(target);
			if (!key != targetNode.data.key)
				return false;

			if (target == Base::root && targetNode.left == target && targetNode.right == target) {
				Base::root = Base::invalid_index;
				Base::deallocate(target);
				return true;
			}

			if (backParent == target) {
				INDEX targetParent = Base::invalid_index;
				INDEX walk = Base::root;
				while (walk != target) {
					targetParent = walk;
					const T& walk_node = Base::get(walk);
					walk = get_bit(key, walk_node.data.bit_index) ? walk_node.right : walk_node.left;
				}

				INDEX replacement = (targetNode.left == target) ? targetNode.right : targetNode.left;

				if (targetParent == Base::invalid_index)
					Base::root = replacement;
				else {
					if (T& targetParentNode = Base::get(targetParent); targetParentNode.left == target)
						targetParentNode.left = replacement;
					else
						targetParentNode.right = replacement;
				}

				Base::deallocate(target);
				return true;
			}

			const T& backParentNode = Base::get(backParent);

			INDEX backGrandparent = Base::invalid_index;
			INDEX walk = Base::root;
			while (walk != backParent) {
				backGrandparent = walk;
				const T& walk_node = Base::get(walk);
				walk = get_bit(key, walk_node.data.bit_index) ? walk_node.right : walk_node.left;
			}

			T& mutableTarget = Base::get(target);
			mutableTarget.data.key = backParentNode.data.key;

			INDEX backParentReplacement = (backParentNode.left == target) ? backParentNode.right : backParentNode.left;

			if (backGrandparent == Base::invalid_index)
				Base::root = backParentReplacement;
			else {
				T& backGrandparentNode = Base::get(backGrandparent);
				if (backGrandparentNode.left == backParent)
					backGrandparentNode.left = backParentReplacement;
				else
					backGrandparentNode.right = backParentReplacement;
			}

			Base::deallocate(backParent);
			return true;
		}

#pragma endregion

#pragma endregion

	};

}