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

    template <concepts::Container CONTAINER>
    using BinarySearchTreeNodeContainer = traits::container_replace_first_arg_t<
        CONTAINER,
        BinarySearchTreeNode<
            traits::normalize_KeyValuePair_t<typename CONTAINER::T>,
            typename CONTAINER::INDEX
        >
    >;

	template <concepts::Container CONTAINER>
        requires std::is_nothrow_destructible_v<typename CONTAINER::T>
	class BinarySearchTree : public IncrementalContainer<BinarySearchTreeNodeContainer<CONTAINER>> {
	public:
        
        using Base = IncrementalContainer<BinarySearchTreeNodeContainer<CONTAINER>>;

        using INDEX = typename CONTAINER::INDEX;
        using T = traits::normalize_KeyValuePair_t<typename CONTAINER::T>;
        using KEY = decltype(T::key);
        using VALUE = decltype(T::value);
        using NODE = BinarySearchTreeNode<T, INDEX>;

        constexpr static INDEX InvalidIndex = concepts::StaticContainer<CONTAINER>
            ? Base::capacity
            : std::numeric_limits<INDEX>::max();

	protected:

        using Base::size;

		INDEX root = 0;
		INDEX free_root = 0;
		INDEX top = 0;

#pragma region Methods

#pragma endregion

	public:

#pragma region Methods

#pragma region Add & Remove

        [[nodiscard]] constexpr bool can_add(INDEX count = 1) const noexcept {
            return (count < InvalidIndex - size) && Base::can_add(count);
        }

        [[nodiscard]] INDEX add(const T& keyValuePair) noexcept(
            std::is_nothrow_copy_constructible_v<T>
        ) {
            if (root == InvalidIndex) {
                root = allocate(keyValuePair);
                return root;
            }

            return add(keyValuePair, root);
        }

        [[nodiscard]] INDEX add(T&& keyValuePair) noexcept(
            std::is_nothrow_move_constructible_v<T>
        ) {
            if (root == InvalidIndex) {
                root = allocate(std::move(keyValuePair));
                return root;
            }

            return add(keyValuePair, root);
        }

        [[nodiscard]] INDEX add(const T& keyValuePair, INDEX rootIndex) noexcept(
            std::is_nothrow_copy_constructible_v<T>
        ) {
            INDEX newIndex = allocate(keyValuePair);
            INDEX parentIndex = find_parent_for(keyValuePair.key, rootIndex);
            NODE& parent = Base::get(parentIndex);
            if (keyValuePair.key < parent.key_value_pair.key)
                parent.left = newIndex;
            else
                parent.right = newIndex;
            return newIndex;
        }

        [[nodiscard]] INDEX add(T&& keyValuePair, INDEX rootIndex) noexcept(
            std::is_nothrow_move_constructible_v<T>
        ) {
            INDEX newIndex = allocate(std::move(keyValuePair));
            INDEX parentIndex = find_parent_for(keyValuePair.key, rootIndex);
            NODE& parent = Base::get(parentIndex);
            if (keyValuePair.key < parent.key_value_pair.key)
                parent.left = newIndex;
            else
                parent.right = newIndex;
            return newIndex;
        }

		void remove(INDEX index) noexcept {
			const NODE& node = Base::get(index);

			INDEX parentIndex = InvalidIndex;
			INDEX leftIndex = node.left;
			INDEX rightIndex = node.right;
			if (leftIndex == InvalidIndex || rightIndex == InvalidIndex) {
				INDEX replacementIndex = leftIndex != InvalidIndex ? leftIndex : rightIndex;
				if (parentIndex == InvalidIndex)
                    root = replacementIndex;
				else if (Base::get(parentIndex).left == index)
                    Base::get(parentIndex).left = replacementIndex;
				else
                    Base::get(parentIndex).right = replacementIndex;
			}

			parentIndex = index;
			leftIndex = Base::get(rightIndex).left;
			while (leftIndex != InvalidIndex) {
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
            if (index == InvalidIndex)
                return;
			remove(index);
		}

#pragma endregion

#pragma region Allocate & Deallocate

		[[nodiscard]] constexpr bool can_allocate(INDEX count = 1) const noexcept {
			return Base::can_add(count);
		}

        [[nodiscard]] constexpr INDEX allocate(const T& keyValuePair) noexcept(
            std::is_nothrow_copy_constructible_v<T>
        ) {
            INDEX index = free_root;
            if (index != InvalidIndex) {
                NODE& node = Base::get(index);
                copy_construct_at(&node.key_value_pair, keyValuePair);
                free_root = node.right;
                node.left = InvalidIndex;
                node.right = InvalidIndex;
            }
            else {
                NODE& node = Base::get(top);
                copy_construct_at(&node.key_value_pair, keyValuePair);
                free_root = node.right;
                node.left = InvalidIndex;
                node.right = InvalidIndex;
                index = top++;
            }

            size++;

            return index;
        }

		[[nodiscard]] constexpr INDEX allocate(T&& keyValuePair) noexcept(
            std::is_nothrow_move_constructible_v<T>
        ) {
			INDEX index = free_root;
            if (index != InvalidIndex) {
                NODE& node = Base::get(index);
                move_construct_at(&node.key_value_pair, std::move(keyValuePair));
                free_root = node.right;
                node.left = InvalidIndex;
                node.right = InvalidIndex;
            }
            else {
                NODE& node = Base::get(top);
                move_construct_at(&node.key_value_pair, std::move(keyValuePair));
                free_root = node.right;
                node.left = InvalidIndex;
                node.right = InvalidIndex;
                index = top++;
            }

			size++;

			return index;
		}

		[[nodiscard]] constexpr bool can_deallocate(INDEX count = 1) const noexcept {
			return Base::can_remove(count);
		}

		constexpr void deallocate(INDEX index) noexcept {
			NODE& node = Base::get(index);
            destroy_at(&node.key_value_pair);
			node.left = InvalidIndex;
			node.right = free_root;
			free_root = index;
			size--;
		}

#pragma endregion

#pragma region Insert, & Upsert

        [[nodiscard]] constexpr bool can_insert(INDEX count = 1) const noexcept {
            return can_add(count);
        }

		[[nodiscard]] INDEX insert(const T& keyValuePair) noexcept(
            std::is_nothrow_copy_constructible_v<T>
        ) {
            return add(keyValuePair);
		}

		[[nodiscard]] INDEX insert(const T& keyValuePair, INDEX rootIndex) noexcept(
            std::is_nothrow_copy_constructible_v<T>
        ) {
            return add(keyValuePair, rootIndex);
		}

        [[nodiscard]] INDEX insert(T&& keyValuePair) noexcept(
            std::is_nothrow_move_constructible_v<T>
        ) {
            return add(std::move(keyValuePair));
        }

        [[nodiscard]] INDEX insert(T&& keyValuePair, INDEX rootIndex) noexcept(
            std::is_nothrow_move_constructible_v<T>
        ) {
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
            if (index == InvalidIndex) {
                root = allocate(keyValuePair);
                return root;
            }

            while (true) {
                NODE& node = Base::get(index);
                KEY& currentKey = node.key_value_pair.key;
                if (currentKey == keyValuePair.key) {
                    node.key_value_pair.value = keyValuePair.value;
                    return index;
                }
                else if (currentKey > keyValuePair.key) {
                    INDEX leftIndex = node.left;
                    if (leftIndex != InvalidIndex)
                        index = leftIndex;
                    else {
                        INDEX newIndex = allocate(keyValuePair);
                        node.left = newIndex;
                        return newIndex;
                    }
                }
                else {
                    INDEX rightIndex = node.right;
                    if (rightIndex != InvalidIndex)
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
            if (index == InvalidIndex) {
                root = allocate(std::move(keyValuePair));
                return root;
            }

            while (true) {
                NODE& node = Base::get(index);
                KEY& currentKey = node.key_value_pair.key;
                if (currentKey == keyValuePair.key) {
                    node.key_value_pair.value = keyValuePair.value;
                    return index;
                }
                else if (currentKey > keyValuePair.key) {
                    INDEX leftIndex = node.left;
                    if (leftIndex != InvalidIndex)
                        index = leftIndex;
                    else {
                        INDEX newIndex = allocate(std::move(keyValuePair));
                        node.left = newIndex;
                        return newIndex;
                    }
                }
                else {
                    INDEX rightIndex = node.right;
                    if (rightIndex != InvalidIndex)
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
            return root != InvalidIndex;
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
            INDEX head = InvalidIndex;
            INDEX top = InvalidIndex;

            while (head <= top) {
                callback(index);
                top++;

                INDEX left = Base::get(index).left;
                if (left != InvalidIndex) {
                    callback(left);
                    top++;
                }

                INDEX right = Base::get(index).right;
                if (right != InvalidIndex) {
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
            INDEX top = InvalidIndex;
            while (rootIndex != InvalidIndex) {
                callback(rootIndex);

                INDEX right = Base::get(rootIndex).right;
                if (right != InvalidIndex) {
                    callback(right);
                    top++;
                }

                INDEX left = Base::get(rootIndex).left;
                if (left != InvalidIndex) {
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
            INDEX top = InvalidIndex;
            while (rootIndex != InvalidIndex) {
                callback(rootIndex);

                INDEX left = Base::get(rootIndex).left;
                if (left != InvalidIndex) {
                    callback(left);
                    top++;
                }

                INDEX right = Base::get(rootIndex).right;
                if (right != InvalidIndex) {
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
            while (rootIndex != InvalidIndex) {
                const NODE& node = Base::get(rootIndex);
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
            while (rootIndex != InvalidIndex) {
                const NODE& node = Base::get(rootIndex);
                if (node.key_value_pair.key == key)
                    return rootIndex;
                rootIndex = (node.key_value_pair.key > key) ? node.left : node.right;
            }
            return InvalidIndex;
        }

        [[nodiscard]] constexpr INDEX find_parent_for(const KEY& key) const noexcept {
            return find_parent_for(key, root);
        }

        [[nodiscard]] constexpr INDEX find_parent_for(const KEY& key, INDEX rootIndex) const noexcept {
            INDEX parentIndex = InvalidIndex;
            while (rootIndex != InvalidIndex) {
                const NODE& node = Base::get(rootIndex);
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
            while (rootIndex != InvalidIndex) {
                const NODE& node = Base::get(rootIndex);
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