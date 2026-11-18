#pragma once

#include "BinarySearchTree.hpp"

namespace DSA {

	namespace concepts {

		template <typename T>
		concept AVLTreeNode = BinaryTreeNode<T> && requires(T t) {

			{ t.height } -> std::same_as<uint16_t>;
			{ t.data } -> std::same_as<typename T::T>;

		};

	}

	template <concepts::KeyValuePair KEY_VALUE_PAIR, std::unsigned_integral NODE_REFERENCE_TYPE = uint32_t>
	struct AVLTreeNode : public BinaryTreeNode<NODE_REFERENCE_TYPE> {

		using T = KEY_VALUE_PAIR;

#pragma region Properties

		NODE_REFERENCE_TYPE height = 0;
		KEY_VALUE_PAIR data;

#pragma endregion

	};

	template <concepts::ContainerPolicy ContainerPolicy, concepts::KeyValuePair KEY_VALUE_PAIR>
		requires std::is_nothrow_destructible_v<KEY_VALUE_PAIR>
	class AVLTree : public BinaryTree<ContainerPolicy, AVLTreeNode<KEY_VALUE_PAIR, typename ContainerPolicy::INDEX>> { //! incomplete
	public:

		using Base = BinaryTree<ContainerPolicy, AVLTreeNode<KEY_VALUE_PAIR, typename ContainerPolicy::INDEX>>;
		using typename Base::ALLOCATOR;
		using typename Base::INDEX;
		using typename Base::T;
		using KEY = T::KEY;
		using VALUE = T::VALUE;

		constexpr static INDEX invalid_index = Base::invalid_index;

#pragma region Constructors & Destructors

		using Base::Base;

#pragma endregion

#pragma region Methods

#pragma region Add & Remove

		/*
		
		[[nodiscard]] INDEX insert(
			INDEX currentIndex,
			const T& keyValuePair,
			INDEX& insertedIndex)
		{
			if (currentIndex == invalid_index) {
				insertedIndex = allocate(keyValuePair);
				return insertedIndex;
			}

			T& node = Base::get(currentIndex);

			if (keyValuePair.key < node.key_value_pair.key) {
				INDEX leftIndex = insert(
					node.left, keyValuePair, insertedIndex);
				Base::get(currentIndex).left = leftIndex;
			}
			else if (node.key_value_pair.key < keyValuePair.key) {
				INDEX rightIndex = insert(
					node.right, keyValuePair, insertedIndex);
				Base::get(currentIndex).right = rightIndex;
			}
			else {
				insertedIndex = currentIndex;
				return currentIndex;
			}

			return rebalance(currentIndex);
		}
		
		[[nodiscard]] INDEX erase(
			INDEX currentIndex,
			const KEY& key)
		{
			if (currentIndex == invalid_index)
				return invalid_index;

			T& node = Base::get(currentIndex);

			if (key < node.key_value_pair.key) {
				INDEX leftIndex = erase(node.left, key);
				Base::get(currentIndex).left = leftIndex;
			}
			else if (node.key_value_pair.key < key) {
				INDEX rightIndex = erase(node.right, key);
				Base::get(currentIndex).right = rightIndex;
			}
			else {
				INDEX leftIndex = node.left;
				INDEX rightIndex = node.right;

				if (leftIndex == invalid_index ||
					rightIndex == invalid_index) {
					INDEX replacementIndex =
						leftIndex != invalid_index
						? leftIndex
						: rightIndex;

					deallocate(currentIndex);
					return replacementIndex;
				}

				INDEX successorIndex = invalid_index;
				INDEX newRightIndex = extract_min(rightIndex, successorIndex);

				T& successor = Base::get(successorIndex);
				successor.left = leftIndex;
				successor.right = newRightIndex;

				deallocate(currentIndex);
				return rebalance(successorIndex);
			}

			return rebalance(currentIndex);
		}

		*/

		[[nodiscard]] constexpr bool can_add(INDEX count = 1) const noexcept {
			return (count < invalid_index - size) && Base::can_add(count);
		}

		[[nodiscard]] INDEX add(const T& keyValuePair)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			if (root == invalid_index) {
				root = allocate(keyValuePair);
				return root;
			}

			return add(keyValuePair, root);
		}

		[[nodiscard]] INDEX add(T&& keyValuePair)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			if (root == invalid_index) {
				root = allocate(std::move(keyValuePair));
				return root;
			}

			return add(keyValuePair, root);
		}

		[[nodiscard]] INDEX add(const T& keyValuePair, INDEX rootIndex)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			INDEX newIndex = allocate(keyValuePair);
			INDEX parentIndex = find_parent_for(keyValuePair.key, rootIndex);
			T& parent = Base::get(parentIndex);
			if (keyValuePair.key < parent.key_value_pair.key)
				parent.left = newIndex;
			else
				parent.right = newIndex;
			return newIndex;
		}

		[[nodiscard]] INDEX add(T&& keyValuePair, INDEX rootIndex)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			INDEX newIndex = allocate(std::move(keyValuePair));
			INDEX parentIndex = find_parent_for(keyValuePair.key, rootIndex);
			T& parent = Base::get(parentIndex);
			if (keyValuePair.key < parent.key_value_pair.key)
				parent.left = newIndex;
			else
				parent.right = newIndex;
			return newIndex;
		}

		void remove(INDEX index) noexcept {
			const T& node = Base::get(index);

			INDEX parentIndex = invalid_index;
			INDEX leftIndex = node.left;
			INDEX rightIndex = node.right;
			if (leftIndex == invalid_index || rightIndex == invalid_index) {
				INDEX replacementIndex = leftIndex != invalid_index ? leftIndex : rightIndex;
				if (parentIndex == invalid_index)
					root = replacementIndex;
				else if (Base::get(parentIndex).left == index)
					Base::get(parentIndex).left = replacementIndex;
				else
					Base::get(parentIndex).right = replacementIndex;
			}

			parentIndex = index;
			leftIndex = Base::get(rightIndex).left;
			while (leftIndex != invalid_index) {
				parentIndex = rightIndex;
				rightIndex = leftIndex;
				leftIndex = Base::get(rightIndex).left;
			}

			Base::get(index) = Base::get(rightIndex);

			INDEX replacementIndex = Base::get(rightIndex).right;
			if (parentIndex == index)
				Base::get(parentIndex).right = replacementIndex;
			else
				Base::get(parentIndex).left = replacementIndex;

			deallocate(rightIndex);
		}

		void remove(const KEY& key) noexcept {
			INDEX index = find(key);
			if (index == invalid_index)
				return;
			remove(index);
		}

#pragma endregion

#pragma region Insert, & Upsert

#pragma endregion

#pragma endregion

	};

}