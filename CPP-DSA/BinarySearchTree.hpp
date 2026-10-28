#pragma once

#include "IncrementalContainer.hpp"
#include "KeyValuePair.hpp"

namespace DSA {

	template <concepts::KeyValuePair KEY_VALUE_PAIR, std::unsigned_integral INDEX = size_t>
	struct BinarySearchTreeNode {

		INDEX left;
		INDEX right;
		KEY_VALUE_PAIR key_value_pair;

	};

	template <typename ContainerConfiguration, concepts::KeyValuePair KeyValuePair>
	using BinarySearchTreeNodeContainer = IncrementalContainer<ContainerConfiguration, BinarySearchTreeNode<KeyValuePair, ContainerConfiguration::template CONTAINER<KeyValuePair>::INDEX>>;

	template <typename ContainerConfiguration, concepts::KeyValuePair KeyValuePair>
		requires std::is_nothrow_destructible_v<KeyValuePair>
	class BinarySearchTree : public BinarySearchTreeNodeContainer<ContainerConfiguration, KeyValuePair> {
	public:
		
		using Base = BinarySearchTreeNodeContainer<ContainerConfiguration, KeyValuePair>;

		using typename Base::INDEX;
		using typename Base::T;
		using KEY = T::KEY;
		using VALUE = T::VALUE;

		constexpr static INDEX invalid_index = ContainerConfiguration::maximum_capacity;

	protected:

		using Base::size;

		INDEX root = 0;
		INDEX free_root = 0;
		INDEX top = 0;

	public:

#pragma region Methods

#pragma region Add & Remove

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

			return add(std::move(keyValuePair), root);
		}

		[[nodiscard]] INDEX add(const T& keyValuePair, INDEX rootIndex) 
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

		[[nodiscard]] INDEX add(T&& keyValuePair, INDEX rootIndex) 
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

#pragma region Allocate & Deallocate

		[[nodiscard]] constexpr bool can_allocate(INDEX count = 1) const noexcept {
			return Base::can_add(count);
		}

		[[nodiscard]] constexpr INDEX allocate(const T& keyValuePair) 
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			INDEX index = free_root;
			if (index != invalid_index) {
				T& node = Base::get(index);
				copy_construct_at(&node.key_value_pair, keyValuePair);
				free_root = node.right;
				node.left = invalid_index;
				node.right = invalid_index;
			}
			else {
				T& node = Base::get(top);
				copy_construct_at(&node.key_value_pair, keyValuePair);
				free_root = node.right;
				node.left = invalid_index;
				node.right = invalid_index;
				index = top++;
			}

			size++;

			return index;
		}

		[[nodiscard]] constexpr INDEX allocate(T&& keyValuePair) 
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			INDEX index = free_root;
			if (index != invalid_index) {
				T& node = Base::get(index);
				move_construct_at(&node.key_value_pair, std::move(keyValuePair));
				free_root = node.right;
				node.left = invalid_index;
				node.right = invalid_index;
			}
			else {
				T& node = Base::get(top);
				move_construct_at(&node.key_value_pair, std::move(keyValuePair));
				free_root = node.right;
				node.left = invalid_index;
				node.right = invalid_index;
				index = top++;
			}

			size++;

			return index;
		}

		[[nodiscard]] constexpr bool can_deallocate(INDEX count = 1) const noexcept {
			return Base::can_remove(count);
		}

		constexpr void deallocate(INDEX index) noexcept {
			T& node = Base::get(index);
			destroy_at(&node.key_value_pair);
			node.left = invalid_index;
			node.right = free_root;
			free_root = index;
			size--;
		}

#pragma endregion

#pragma region Insert, & Upsert

		[[nodiscard]] constexpr bool can_insert(INDEX count = 1) const noexcept {
			return can_add(count);
		}

		[[nodiscard]] INDEX insert(const T& keyValuePair) 
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			return add(keyValuePair);
		}

		[[nodiscard]] INDEX insert(const T& keyValuePair, INDEX rootIndex) 
			noexcept (std::is_nothrow_copy_constructible_v<T>)
		{
			return add(keyValuePair, rootIndex);
		}

		[[nodiscard]] INDEX insert(T&& keyValuePair) 
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			return add(std::move(keyValuePair));
		}

		[[nodiscard]] INDEX insert(T&& keyValuePair, INDEX rootIndex) 
			noexcept (std::is_nothrow_move_constructible_v<T>)
		{
			return add(std::move(keyValuePair), rootIndex);
		}

		[[nodiscard]] constexpr bool can_upsert(INDEX count = 1) const noexcept {
			return can_add(count);
		}

		[[nodiscard]] INDEX upsert(const T& keyValuePair) noexcept(
			std::is_nothrow_copy_assignable_v<T>
			&& std::is_nothrow_copy_constructible_v<T>
		) {
			INDEX index = root;
			if (index == invalid_index) {
				root = allocate(keyValuePair);
				return root;
			}

			while (true) {
				T& node = Base::get(index);
				KEY& currentKey = node.key_value_pair.key;
				if (currentKey == keyValuePair.key) {
					node.key_value_pair.value = keyValuePair.value;
					return index;
				}
				else if (currentKey > keyValuePair.key) {
					INDEX leftIndex = node.left;
					if (leftIndex != invalid_index)
						index = leftIndex;
					else {
						INDEX newIndex = allocate(keyValuePair);
						node.left = newIndex;
						return newIndex;
					}
				}
				else {
					INDEX rightIndex = node.right;
					if (rightIndex != invalid_index)
						index = rightIndex;
					else {
						INDEX newIndex = allocate(keyValuePair);
						node.right = newIndex;
						return newIndex;
					}
				}
			}
		}

		[[nodiscard]] INDEX upsert(T&& keyValuePair) noexcept(
			std::is_nothrow_move_assignable_v<T>
			&& std::is_nothrow_move_constructible_v<T>
		) {
			INDEX index = root;
			if (index == invalid_index) {
				root = allocate(std::move(keyValuePair));
				return root;
			}

			while (true) {
				T& node = Base::get(index);
				KEY& currentKey = node.key_value_pair.key;
				if (currentKey == keyValuePair.key) {
					node.key_value_pair.value = keyValuePair.value;
					return index;
				}
				else if (currentKey > keyValuePair.key) {
					INDEX leftIndex = node.left;
					if (leftIndex != invalid_index)
						index = leftIndex;
					else {
						INDEX newIndex = allocate(std::move(keyValuePair));
						node.left = newIndex;
						return newIndex;
					}
				}
				else {
					INDEX rightIndex = node.right;
					if (rightIndex != invalid_index)
						index = rightIndex;
					else {
						INDEX newIndex = allocate(std::move(keyValuePair));
						node.right = newIndex;
						return newIndex;
					}
				}
			}
		}

#pragma endregion

#pragma region Traversal

		[[nodiscard]] constexpr bool can_traverse() const noexcept {
			return root != invalid_index;
		}

		template <std::invocable<INDEX> CALLBACK>
		constexpr void traverse_level_order(CALLBACK callback) noexcept(
			std::is_nothrow_invocable_v<CALLBACK, INDEX>
		) {
			traverse_level_order(callback, root);
		}

		template <std::invocable<INDEX> CALLBACK>
		constexpr void traverse_level_order(CALLBACK callback, INDEX index) noexcept(
			std::is_nothrow_invocable_v<CALLBACK, INDEX>
		) {
			INDEX head = invalid_index;
			INDEX top = invalid_index;

			while (head <= top) {
				callback(index);
				top++;

				INDEX left = Base::get(index).left;
				if (left != invalid_index) {
					callback(left);
					top++;
				}

				INDEX right = Base::get(index).right;
				if (right != invalid_index) {
					callback(right);
					top++;
				}

				index = Base::get(++head);
			}
		}

		template <std::invocable<INDEX> CALLBACK>
		constexpr void traverse_post_order(CALLBACK callback) noexcept(
			std::is_nothrow_invocable_v<CALLBACK, INDEX>
		) {
			traverse_post_order(callback, root);
		}

		template <std::invocable<INDEX> CALLBACK>
		constexpr void traverse_post_order(CALLBACK callback, INDEX rootIndex) noexcept(
			std::is_nothrow_invocable_v<CALLBACK, INDEX>
		) {
			INDEX top = invalid_index;
			while (rootIndex != invalid_index) {
				callback(rootIndex);

				INDEX right = Base::get(rootIndex).right;
				if (right != invalid_index) {
					callback(right);
					top++;
				}

				INDEX left = Base::get(rootIndex).left;
				if (left != invalid_index) {
					callback(left);
					top++;
				}

				rootIndex = Base::get(--top);
			}
		}

		template <std::invocable<INDEX> CALLBACK>
		constexpr void traverse_pre_order(CALLBACK callback) noexcept(
			std::is_nothrow_invocable_v<CALLBACK, INDEX>
		) {
			traverse_pre_order(callback, root);
		}

		template <std::invocable<INDEX> CALLBACK>
		constexpr void traverse_pre_order(CALLBACK callback, INDEX rootIndex) noexcept(
			std::is_nothrow_invocable_v<CALLBACK, INDEX>
		) {
			INDEX top = invalid_index;
			while (rootIndex != invalid_index) {
				callback(rootIndex);

				INDEX left = Base::get(rootIndex).left;
				if (left != invalid_index) {
					callback(left);
					top++;
				}

				INDEX right = Base::get(rootIndex).right;
				if (right != invalid_index) {
					callback(right);
					top++;
				}

				rootIndex = Base::get(--top);
			}
		}

#pragma endregion

#pragma region Search

		[[nodiscard]] constexpr INDEX ceiling(const KEY& key) const noexcept {
			return ceiling(key, root);
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
			return find(key, root);
		}

		[[nodiscard]] constexpr INDEX find(const KEY& key, INDEX rootIndex) const noexcept {
			while (rootIndex != invalid_index) {
				const T& node = Base::get(rootIndex);
				if (node.key_value_pair.key == key)
					return rootIndex;
				rootIndex = (node.key_value_pair.key > key) ? node.left : node.right;
			}
			return invalid_index;
		}

		[[nodiscard]] constexpr INDEX find_parent_for(const KEY& key) const noexcept {
			return find_parent_for(key, root);
		}

		[[nodiscard]] constexpr INDEX find_parent_for(const KEY& key, INDEX rootIndex) const noexcept {
			INDEX parentIndex = invalid_index;
			while (rootIndex != invalid_index) {
				const T& node = Base::get(rootIndex);
				parentIndex = rootIndex;
				if (key <= node.key_value_pair.key)
					rootIndex = node.left;
				else
					rootIndex = node.right;
			}
			return parentIndex;
		}

		[[nodiscard]] constexpr INDEX floor(const KEY& key) const noexcept {
			return floor(key, root);
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

	};

}