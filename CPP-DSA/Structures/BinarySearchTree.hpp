#pragma once

#include "BinaryTree.hpp"
#include "KeyValuePair.hpp"

namespace DSA {

	namespace concepts {

		template <typename T>
		concept BinarySearchTreeNode = BinaryTreeValueNode<T>;

	}

	template <concepts::KeyValuePair KEY_VALUE_PAIR, std::unsigned_integral NODE_REFERENCE_TYPE = uint32_t>
	using BinarySearchTreeNode = BinaryTreeValueNode<KEY_VALUE_PAIR, NODE_REFERENCE_TYPE>;

	template <concepts::ContainerPolicy ContainerPolicy, concepts::KeyValuePair KEY_VALUE_PAIR>
		requires std::is_nothrow_destructible_v<KEY_VALUE_PAIR>
	class BinarySearchTree : public BinaryTree<ContainerPolicy, BinarySearchTreeNode<KEY_VALUE_PAIR, typename ContainerPolicy::INDEX>> {
	public:
		
		using Base = BinaryTree<ContainerPolicy, BinarySearchTreeNode<KEY_VALUE_PAIR, typename ContainerPolicy::INDEX>>;
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

		[[nodiscard]] constexpr INDEX add(const T& keyValuePair)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			const INDEX root = Base::get_root();
			if (root == invalid_index) {
				root = Base::allocate(keyValuePair);
				return root;
			}

			return add(keyValuePair, root);
		}

		[[nodiscard]] constexpr INDEX add(const T& keyValuePair, INDEX rootIndex)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			INDEX parentIndex = find_parent_for(keyValuePair.key, rootIndex);
			INDEX newIndex = allocate(keyValuePair);
			T& parent = Base::get(parentIndex);
			if (keyValuePair.key < parent.key_value_pair.key)
				parent.left = newIndex;
			else
				parent.right = newIndex;

			return newIndex;
		}

		[[nodiscard]] constexpr INDEX add(T&& keyValuePair)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			const INDEX root = Base::get_root();
			if (root == invalid_index) {
				root = Base::allocate(std::move(keyValuePair));
				return root;
			}

			return add(std::move(keyValuePair), root);
		}

		[[nodiscard]] constexpr INDEX add(T&& keyValuePair, INDEX rootIndex)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			INDEX parentIndex = find_parent_for(keyValuePair.key, rootIndex);
			INDEX newIndex = allocate(std::move(keyValuePair));
			T& parent = Base::get(parentIndex);
			if (keyValuePair.key < parent.key_value_pair.key)
				parent.left = newIndex;
			else
				parent.right = newIndex;

			return newIndex;
		}

		constexpr void remove(INDEX index) noexcept {
			const T& node = Base::get(index);

			INDEX parentIndex = invalid_index;
			INDEX leftIndex = node.left;
			INDEX rightIndex = node.right;

			if (leftIndex == invalid_index || rightIndex == invalid_index) {
				INDEX replacementIndex = leftIndex != invalid_index ? leftIndex : rightIndex;
				if (parentIndex == invalid_index)
					Base::set_root(replacementIndex);
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

		constexpr void remove(const KEY& key) noexcept {
			INDEX index = find(key);
			if (index == invalid_index)
				return;

			remove(index);
		}

#pragma endregion

#pragma region Ceiling, Find, Floor, Insert, & Upsert

		[[nodiscard]] constexpr INDEX ceiling(const KEY& key) const noexcept {
			return ceiling(key, Base::get_root());
		}

		[[nodiscard]] constexpr INDEX ceiling(const KEY& key, INDEX rootIndex) const noexcept {
			INDEX candidate = rootIndex;
			while (rootIndex != invalid_index) {
				const T& node = Base::get(rootIndex);
				if (key == node.value.key)
					return rootIndex;
				else if (key > node.value.key) {
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
				if (node.value.key == key)
					return rootIndex;
				rootIndex = (node.value.key > key) ? node.left : node.right;
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
				if (key <= node.value.key)
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
				if (key == node.value.key)
					return rootIndex;
				else if (key < node.value.key) {
					candidateIndex = rootIndex;
					rootIndex = node.left;
				}
				else
					rootIndex = node.right;
			}

			return candidateIndex;
		}

		[[nodiscard]] constexpr INDEX insert(const T& keyValuePair)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			return add(keyValuePair);
		}

		[[nodiscard]] constexpr INDEX insert(const T& keyValuePair, INDEX rootIndex)
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			return add(keyValuePair, rootIndex);
		}

		[[nodiscard]] constexpr INDEX insert(T&& keyValuePair)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			return add(std::move(keyValuePair));
		}

		[[nodiscard]] constexpr INDEX insert(T&& keyValuePair, INDEX rootIndex)
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			return add(std::move(keyValuePair), rootIndex);
		}

		[[nodiscard]] constexpr INDEX upsert(const T& keyValuePair)
			noexcept(std::is_nothrow_copy_assignable_v<T>
				&& std::is_nothrow_copy_constructible_v<T>)
		{
			INDEX index = Base::get_root();
			if (index == invalid_index) {
				const INDEX newRoot = Base::allocate(keyValuePair);
				Base::set_root(newRoot);
				return newRoot;
			}

			while (true) {
				T& node = Base::get(index);
				KEY& currentKey = node.value.key;
				if (currentKey == keyValuePair.key) {
					node.value.value = keyValuePair.value;
					return index;
				}
				else if (currentKey > keyValuePair.key) {
					INDEX leftIndex = node.left;
					if (leftIndex != invalid_index)
						index = leftIndex;
					else {
						INDEX newIndex = Base::allocate(keyValuePair);
						node.left = newIndex;
						return newIndex;
					}
				}
				else {
					INDEX rightIndex = node.right;
					if (rightIndex != invalid_index)
						index = rightIndex;
					else {
						INDEX newIndex = Base::allocate(keyValuePair);
						node.right = newIndex;
						return newIndex;
					}
				}
			}
		}

		[[nodiscard]] constexpr INDEX upsert(T&& keyValuePair)
			noexcept(std::is_nothrow_move_assignable_v<T>
				&& std::is_nothrow_move_constructible_v<T>)
		{
			INDEX index = Base::get_root();
			if (index == invalid_index) {
				const INDEX newRoot = Base::allocate(std::move(keyValuePair));
				Base::set_root(newRoot);
				return newRoot;
			}

			while (true) {
				T& node = Base::get(index);
				KEY& currentKey = node.value.key;
				if (currentKey == keyValuePair.key) {
					node.value.value = keyValuePair.value;
					return index;
				}
				else if (currentKey > keyValuePair.key) {
					INDEX leftIndex = node.left;
					if (leftIndex != invalid_index)
						index = leftIndex;
					else {
						INDEX newIndex = Base::allocate(std::move(keyValuePair));
						node.left = newIndex;
						return newIndex;
					}
				}
				else {
					INDEX rightIndex = node.right;
					if (rightIndex != invalid_index)
						index = rightIndex;
					else {
						INDEX newIndex = Base::allocate(std::move(keyValuePair));
						node.right = newIndex;
						return newIndex;
					}
				}
			}
		}

#pragma endregion

#pragma endregion

	};

}
