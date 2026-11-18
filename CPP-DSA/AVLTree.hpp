
#pragma once

#include "BinaryTree.hpp"
#include "KeyValuePair.hpp"

namespace DSA {

	namespace concepts {

		template <typename T>
		concept AVLTreeNode = BinaryTreeNode<T> && requires(T t) {

			{ t.data } -> std::same_as<typename T::T>;
			{ t.height } -> std::same_as<uint16_t>;

		};

		template <typename T>
		concept ParentedAVLTreeNode = AVLTreeNode<T> && requires(T t) {

			{ t.parent } -> std::same_as<typename T::NODE_REFERENCE_TYPE>;

		};

	}

	template <concepts::KeyValuePair KEY_VALUE_PAIR, std::unsigned_integral NODE_REFERENCE_TYPE = uint32_t>
	struct AVLTreeNode : public BinaryTreeNode<NODE_REFERENCE_TYPE> {

		using T = KEY_VALUE_PAIR;

#pragma region Properties

		uint16_t height = 1;
		KEY_VALUE_PAIR data;

#pragma endregion

	};

	template <concepts::KeyValuePair KEY_VALUE_PAIR, std::unsigned_integral NODE_REFERENCE_TYPE = uint32_t>
	struct ParentedAVLTreeNode : public BinaryTreeNode<NODE_REFERENCE_TYPE> {

		using T = KEY_VALUE_PAIR;

#pragma region Properties

		NODE_REFERENCE_TYPE parent;
		uint16_t height = 1;
		KEY_VALUE_PAIR data;

#pragma endregion

	};

	template <concepts::ContainerPolicy ContainerPolicy, concepts::KeyValuePair KEY_VALUE_PAIR>
		requires std::is_nothrow_destructible_v<KEY_VALUE_PAIR>
	class AVLTree : public BinaryTree<ContainerPolicy, AVLTreeNode<KEY_VALUE_PAIR, typename ContainerPolicy::INDEX>> {
	public:

		using Base = BinaryTree<ContainerPolicy, AVLTreeNode<KEY_VALUE_PAIR, typename ContainerPolicy::INDEX>>;
		using typename Base::ALLOCATOR;
		using typename Base::INDEX;
		using typename Base::T;

		using KEY = typename T::KEY;
		using VALUE = typename T::VALUE;

		constexpr static INDEX invalid_index = Base::invalid_index;

	protected:

#pragma region Methods

#pragma region Helpers

		constexpr void replace_child(INDEX parent, INDEX oldChild, INDEX newChild) noexcept {
			if (parent == invalid_index)
				Base::set_root(newChild);
			else if (Base::get(parent).left == oldChild)
				Base::get(parent).left = newChild;
			else
				Base::get(parent).right = newChild;

			if (newChild != invalid_index)
				Base::get(newChild).parent = parent;
		}

#pragma endregion

#pragma region Rebalancing

		[[nodiscard]] constexpr int32_t balance_factor(INDEX index) const noexcept {
			const T& node = Base::get(index);
			return static_cast<int32_t>(height_of(node.left)) - static_cast<int32_t>(height_of(node.right));
		}

		[[nodiscard]] constexpr INDEX rebalance(INDEX index) noexcept {
			update_height(index);

			const T& node = Base::get(index);
			const int32_t balance = balance_factor(index);
			if (balance > 1) {
				const INDEX left_index = node.left;
				if (balance_factor(left_index) < 0)
					node.left = rotate_left(left_index);

				return rotate_right(index);
			}
			if (balance < -1) {
				const INDEX right_index = node.right;
				if (balance_factor(right_index) > 0)
					node.right = rotate_right(right_index);

				return rotate_left(index);
			}

			return index;
		}

		constexpr void rebalance_upward(INDEX index) noexcept {
			if constexpr (concepts::ParentedAVLTreeNode<T>) {
				while (index != invalid_index)
					index = parent_of(rebalance(index));
			}
			else {
				while (index != invalid_index) {
					const INDEX parent = parent_of(index);
					const INDEX newRoot = rebalance(index);

					if (parent == invalid_index)
						Base::set_root(newRoot);
					else if (Base::get(parent).left == index)
						Base::get(parent).left = newRoot;
					else
						Base::get(parent).right = newRoot;

					index = parent;
				}
			}
		}

#pragma endregion

#pragma region Rotation

		constexpr void update_height(INDEX index) noexcept {
			T& node = Base::get(index);
			const uint16_t left_height = height_of(node.left);
			const uint16_t right_height = height_of(node.right);
			node.height = 1 + (left_height > right_height ? left_height : right_height);
		}

		[[nodiscard]] constexpr INDEX rotate_left(INDEX index) noexcept {
			if constexpr (concepts::ParentedAVLTreeNode<T>) {
				T& node = Base::get(index);
				const INDEX parent = node.parent;
				const INDEX rightIndex = node.right;

				const INDEX rightLeftIndex = Base::get(rightIndex).left;

				replace_child(parent, index, rightIndex);

				Base::get(rightIndex).left = index;
				node.parent = rightIndex;
				node.right = rightLeftIndex;

				if (rightLeftIndex != invalid_index)
					Base::get(rightLeftIndex).parent = index;

				update_height(index);
				update_height(rightIndex);

				return rightIndex;
			}
			else {
				T& node = Base::get(index);
				const INDEX rightIndex = node.right;

				T& rightNode = Base::get(rightIndex);
				const INDEX rightLeftIndex = rightNode.left;

				rightNode.left = index;
				node.right = rightLeftIndex;

				update_height(index);
				update_height(rightIndex);

				return rightIndex;
			}
		}

		[[nodiscard]] constexpr INDEX rotate_right(INDEX index) noexcept {
			if constexpr (concepts::ParentedAVLTreeNode<T>) {
				T& node = Base::get(index);
				const INDEX parent = node.parent;
				const INDEX leftIndex = node.left;

				const INDEX leftRightIndex = Base::get(leftIndex).right;

				replace_child(parent, index, leftIndex);

				Base::get(leftIndex).right = index;
				node.parent = leftIndex;
				node.left = leftRightIndex;

				if (leftRightIndex != invalid_index)
					Base::get(leftRightIndex).parent = index;

				update_height(index);
				update_height(leftIndex);

				return leftIndex;
			}
			else {
				T& node = Base::get(index);
				const INDEX leftIndex = node.left;

				T& leftNode = Base::get(leftIndex);
				const INDEX leftRightIndex = leftNode.right;

				leftNode.right = index;
				node.left = leftRightIndex;

				update_height(index);
				update_height(leftIndex);

				return leftIndex;
			}
		}

#pragma endregion

#pragma endregion

	public:

#pragma region Constructors & Destructors

		using Base::Base;

#pragma endregion

#pragma region Methods

#pragma region Getters

		[[nodiscard]] constexpr uint16_t get_height() const noexcept {
			const INDEX root = Base::get_root();
			return root == invalid_index ? 0 : height_of(root);
		}

		[[nodiscard]] constexpr uint16_t height_of(INDEX index) const noexcept {
			return Base::get(index).height;
		}

		[[nodiscard]] constexpr INDEX parent_of(INDEX index) const noexcept {
			if constexpr (concepts::ParentedAVLTreeNode<T>)
				return Base::get(index).parent;
			else {
				if (index == Base::get_root() || index == invalid_index)
					return invalid_index;

				const KEY& key = Base::get(index).data.key;
				INDEX current = Base::get_root();
				INDEX parent;
				while (current != invalid_index && current != index) {
					parent = current;
					const T& node = Base::get(current);
					current = key < node.data.key ? node.left : node.right;
				}

				return current == index ? parent : invalid_index;
			}
		}

#pragma endregion

#pragma region Add & Remove

		[[nodiscard]] constexpr INDEX add(const T& keyValuePair)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			INDEX current = Base::get_root();
			INDEX parent = invalid_index;

			while (current != invalid_index) {
				parent = current;
				const T& node = Base::get(current);
				current = keyValuePair.key < node.data.key ? node.left : node.right;
			}

			const INDEX newIndex = Base::allocate(std::forward<T>(keyValuePair));

			if (parent == invalid_index)
				Base::set_root(newIndex);
			else if (Base::get(newIndex).data.key < Base::get(parent).data.key)
				Base::get(parent).left = newIndex;
			else
				Base::get(parent).right = newIndex;

			rebalance_upward(parent);

			return newIndex;
		}

		[[nodiscard]] constexpr INDEX add(T&& keyValuePair)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			INDEX current = Base::get_root();
			INDEX parent = invalid_index;

			while (current != invalid_index) {
				parent = current;
				const T& node = Base::get(current);
				current = keyValuePair.key < node.data.key ? node.left : node.right;
			}

			const INDEX newIndex = Base::allocate(std::forward<T>(keyValuePair));

			if (parent == invalid_index)
				Base::set_root(newIndex);
			else if (Base::get(newIndex).data.key < Base::get(parent).data.key)
				Base::get(parent).left = newIndex;
			else
				Base::get(parent).right = newIndex;

			rebalance_upward(parent);

			return newIndex;
		}

		constexpr void remove(INDEX index) noexcept {
			T& node = Base::get(index);

			const INDEX parent = parent_of(index);
			const INDEX left = node.left;
			const INDEX right = node.right;

			if (left == invalid_index || right == invalid_index) { // <= 1
				replace_child(parent, index, left & right);
				Base::deallocate(index);

				rebalance_upward(parent);

				return;
			}

			const INDEX successor = right;
			while (Base::get(successor).left != invalid_index)
				successor = Base::get(successor).left;

			T& successorNode = Base::get(successor);
			INDEX rebalance_start;

			if (successorNode.parent != index) {
				rebalance_start = successorNode.parent;
				replace_child(rebalance_start, successor, successorNode.right);
				successorNode.right = right;

				Base::get(right).parent = successor;
			}
			else
				rebalance_start = successor;

			replace_child(parent, index, successor);

			successorNode.left = left;
			Base::get(left).parent = successor;

			Base::deallocate(index);

			rebalance_upward(rebalance_start);
		}

#pragma endregion

#pragma region Find

		[[nodiscard]] constexpr INDEX ceiling(const KEY& key) const noexcept {
			return ceiling(key, Base::get_root());
		}

		[[nodiscard]] constexpr INDEX ceiling(const KEY& key, INDEX rootIndex) const noexcept {
			INDEX candidate = rootIndex;
			while (rootIndex != invalid_index) {
				const T& node = Base::get(rootIndex);
				if (key == node.key_value_pair.key)
					return rootIndex;
				else if (key > node.key_value_pair.key) {
					candidate = rootIndex;
					rootIndex = node.right;
				}
				else
					rootIndex = node.left;
			}

			return candidate;
		}

		[[nodiscard]] constexpr INDEX find(const KEY& key) const noexcept {
			return find(key, Base::get_root());
		}

		[[nodiscard]] constexpr INDEX find(const KEY& key, INDEX rootIndex) const noexcept {
			while (rootIndex != invalid_index) {
				const T& node = Base::get(rootIndex);
				if (node.keyValuePair.key == key)
					return rootIndex;
				rootIndex = (node.keyValuePair.key > key) ? node.left : node.right;
			}

			return invalid_index;
		}

		[[nodiscard]] constexpr INDEX find_parent_for(const KEY& key) const noexcept {
			return find_parent_for(key, Base::get_root());
		}

		[[nodiscard]] constexpr INDEX find_parent_for(const KEY& key, INDEX rootIndex) const noexcept {
			INDEX parentIndex = invalid_index;
			while (rootIndex != invalid_index) {
				const T& node = Base::get(rootIndex);
				parentIndex = rootIndex;
				if (key <= node.keyValuePair.key)
					rootIndex = node.left;
				else
					rootIndex = node.right;
			}

			return parentIndex;
		}

		[[nodiscard]] constexpr INDEX floor(const KEY& key) const noexcept {
			return floor(key, Base::get_root());
		}

		[[nodiscard]] constexpr INDEX floor(const KEY& key, INDEX rootIndex) const noexcept {
			INDEX candidateIndex = rootIndex;
			while (rootIndex != invalid_index) {
				const T& node = Base::get(rootIndex);
				if (key == node.key_value_pair.key)
					return rootIndex;
				else if (key < node.key_value_pair.key) {
					candidateIndex = rootIndex;
					rootIndex = node.left;
				}
				else
					rootIndex = node.right;
			}

			return candidateIndex;
		}

#pragma endregion

#pragma region Extract Minimum, Insert, & Upsert

		[[nodiscard]] constexpr INDEX extract_minimum(INDEX index, INDEX& extractedIndex) noexcept {
			INDEX parent = invalid_index;

			while (Base::get(index).left != invalid_index) {
				parent = index;
				index = Base::get(index).left;
			}

			extractedIndex = index;

			const T& node = Base::get(index);
			const INDEX replacement = node.right;

			if (parent != invalid_index)
				Base::get(parent).left = replacement;

			node.left = invalid_index;
			node.right = invalid_index;

			return replacement;
		}

		[[nodiscard]] constexpr INDEX insert(const T& keyValuePair) {
			return add(keyValuePair);
		}

		[[nodiscard]] constexpr INDEX insert(T&& keyValuePair) {
			return add(std::move(keyValuePair));
		}

		[[nodiscard]] constexpr INDEX upsert(const T& keyValuePair) {
			const INDEX existing_index = find(keyValuePair.key);
			if (existing_index != invalid_index) {
				Base::get(existing_index).data = keyValuePair;
				return existing_index;
			}

			return add(keyValuePair);
		}

		[[nodiscard]] constexpr INDEX upsert(T&& keyValuePair) {
			const INDEX existing_index = find(keyValuePair.key);
			if (existing_index != invalid_index) {
				Base::get(existing_index).data = std::move(keyValuePair);
				return existing_index;
			}

			return add(std::move(keyValuePair));
		}

#pragma endregion

#pragma endregion

	};

}