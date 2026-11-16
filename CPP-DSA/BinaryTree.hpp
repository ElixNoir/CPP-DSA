#pragma once

#include "Aggregation.hpp"
#include "Collection.hpp"

namespace DSA {

	namespace concepts {

		template <typename T>
		concept BinaryTreeNode = requires(T t) {

			typename T::NODE_REFERENCE_TYPE;
			typename T::T;

			{ t.left } -> std::same_as<typename T::NODE_REFERENCE_TYPE>;
			{ t.right } -> std::same_as<typename T::NODE_REFERENCE_TYPE>;
			{ t.data } -> std::same_as<typename T::T>;

		};

	}

	template <typename _T = EMPTY_STRUCT, std::unsigned_integral _NODE_REFERENCE_TYPE = uint32_t>
	struct BinaryTreeNode {

		using NODE_REFERENCE_TYPE = _NODE_REFERENCE_TYPE;
		using T = _T;

		NODE_REFERENCE_TYPE left;
		NODE_REFERENCE_TYPE right;
		T data;

	};

	struct BinaryTreeDerivation : public CollectionDerivation {

		constexpr static bool is_offset = true;
		constexpr static bool is_sparse = true;

	};

	template <concepts::ContainerPolicy ContainerPolicy, concepts::BinaryTreeNode TREE_NODE = BinaryTreeNode<EMPTY_STRUCT, typename ContainerPolicy::INDEX>>
		requires std::is_nothrow_destructible_v<TREE_NODE>
	class BinaryTree : public Collection<ContainerPolicy, TREE_NODE, BinaryTreeDerivation> {
	public:

		using Base = Collection<ContainerPolicy, TREE_NODE, BinaryTreeDerivation>;
		using typename Base::ALLOCATOR;
		using typename Base::INDEX;
		using typename Base::T;

		using Self = BinaryTree<ContainerPolicy, TREE_NODE>;

		constexpr static bool is_dynamic = Base::is_dynamic;

		constexpr static INDEX invalid_index = Base::maximum_capacity;

	protected:

#pragma region Properties

		INDEX root = invalid_index;
		INDEX free_root = invalid_index;
		INDEX allocated_top = 0;

#pragma endregion

#pragma region Methods

#pragma region Allocate & Deallocate

		[[nodiscard]] constexpr bool can_allocate(INDEX count = 1) const noexcept {
			return Base::can_add(count);
		}

		template <typename... ARGUMENTS>
		[[nodiscard]] constexpr INDEX allocate(ARGUMENTS&&... arguments)
			noexcept (std::is_nothrow_constructible_v<T, ARGUMENTS&&...>)
		{
			INDEX index = free_root;
			if (index != invalid_index) {
				T& node = Base::get(index);
				construct_at(&node.data, std::forward<ARGUMENTS>(arguments)...);
				free_root = node.right;
				node.left = invalid_index;
				node.right = invalid_index;
			}
			else {
				T& node = Base::get(allocated_top);
				construct_at(&node.data, std::forward<ARGUMENTS>(arguments)...);
				free_root = node.right;
				index = allocated_top++;
				node.left = invalid_index;
				node.right = invalid_index;
			}

			++Base::size;

			return index;
		}

		[[nodiscard]] constexpr bool can_deallocate(INDEX count = 1) const noexcept {
			return Base::can_remove(count);
		}

		constexpr void deallocate(INDEX index) noexcept {
			T& node = Base::get(index);
			destroy_at(&node.key_value_pair);
			node.right = free_root;
			free_root = index;
			--Base::size;
		}

#pragma endregion

#pragma region Resize

		template <ResizeOperation RESIZE_OPERATION = ResizeOperation::UNKNOWN, bool IS_DENSE = true>
		void helper_resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires is_dynamic
		{
			if constexpr (std::is_trivially_move_constructible_v<T> && IS_DENSE) {
				Base::resize_from_size(newCapacity, Base::get_allocated_top());
				return;
			}

			T* const oldData = Base::get_data();
			T* const newData = ALLOCATOR::allocate(newCapacity);

			const INDEX newSize = Aggregation::minimum(newCapacity, Base::get_size());
			const INDEX root = Base::get_root();
			if constexpr (std::is_nothrow_move_constructible_v<T>) {
				Base::pre_order([](INDEX index) {
					reconstruct_at(newData + index, std::move(Base::get(index)));
				}, root);
			}
			else {
				try {
					Base::pre_order([](INDEX index) {
						move_construct_at(newData + index, std::move(Base::get(index)));
						}, root);
				}
				catch (...) {
					Base::post_order([](INDEX index) {
						destroy_at(newData + index);
						}, root);

					ALLOCATOR::deallocate(newData);
					throw;
				}

				Base::post_order([](INDEX index) {
					destroy_at(oldData + index);
				}, root);
			}

			ALLOCATOR::deallocate(oldData);

			Base::set_capacity(newCapacity);
			Base::set_data(newData);
		}

#pragma endregion

#pragma endregion

	public:

#pragma region Constructors & Destructors

		using Base::Base;

		~BinaryTree() noexcept = default;

		~BinaryTree() noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			destroy();
		}

		constexpr void destroy() noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>) {
				Base::post_order([](INDEX index) {
					destroy_at(Base::get_data() + index);
				}, root);
			}
			/*
			while (root != invalid_index) {
				T& node = Base::get(root);
				if (node.left != invalid_index) {
					INDEX left_index = node.left;
					T& left = Base::get(left_index);
					node.left = left.right;
					left.right = root;
					root = left_index;
				}
				else {
					INDEX index = root;
					root = node.right;
					deallocate(index);
					//destroy_at(Base::get_data() + root);
					//root = node.right;
				}
			}
			*/
		}

#pragma endregion

#pragma region Methods

#pragma region Checks

		[[nodiscard]] constexpr bool can_insert(INDEX count = 1) const noexcept {
			return can_allocate(count);
		}

		[[nodiscard]] constexpr bool can_upsert(INDEX count = 1) const noexcept {
			return can_allocate(count);
		}

#pragma endregion

#pragma region Getters

		[[nodiscard]] constexpr INDEX get_root() const noexcept {
			return root;
		}

		[[nodiscard]] constexpr INDEX get_free_root() const noexcept {
			return free_root;
		}

		[[nodiscard]] constexpr INDEX get_allocated_top() const noexcept {
			return allocated_top;
		}

#pragma endregion

#pragma region Memory Management

		constexpr void double_capacity()
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			grow(capacity << 1);
		}

		constexpr void empty() noexcept {
			destroy();
			Base::set_size(0);
			root = invalid_index;
			free_root = invalid_index;
			allocated_top = 0;
		}

		constexpr void expand(float ratio = 1.5)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			grow(Math::ceiling(capacity * ratio));
		}

		constexpr void grow(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires is_dynamic
		{
			helper_resize<ResizeOperation::GROW>(newCapacity);
		}

		constexpr void reserve(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity > capacity)
				grow(newCapacity);
		}

		constexpr void resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity >= capacity)
				grow(newCapacity);
			else
				shrink(newCapacity);
		}

		constexpr void shrink(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires is_dynamic
		{
			helper_resize<ResizeOperation::SHRINK>(newCapacity);
		}

#pragma endregion

#pragma region Setters

		constexpr void set_root(INDEX newRoot) noexcept {
			root = newRoot;
		}

		constexpr void set_free_root(INDEX newFreeRoot) noexcept {
			free_root = newFreeRoot;
		}

		constexpr void set_allocated_top(INDEX newFreeTop) noexcept {
			allocated_top = newFreeTop;
		}

#pragma endregion

#pragma region Traversal

		template <std::invocable<INDEX> OPERATION>
		constexpr void in_order(OPERATION& operation, INDEX index)
			noexcept (std::is_nothrow_invocable_v<OPERATION&, INDEX>)
		{
			if (index == invalid_index)
				return;

			T& node = Base::get(index);

			in_order(operation, node.left);
			std::invoke(operation, index);
			in_order(operation, node.right);
		}

		template <std::predicate<INDEX> PREDICATE>
		constexpr bool in_order(PREDICATE& predicate, INDEX index)
			noexcept (std::is_nothrow_invocable_v<PREDICATE&, INDEX>)
		{
			if (index == invalid_index)
				return true;

			T& node = Base::get(index);

			if (!in_order(predicate, node.left))
				return false;

			if (!std::invoke(predicate, index))
				return false;

			return in_order(predicate, node.right);
		}

		template <std::invocable<INDEX> OPERATION>
		constexpr void post_order(OPERATION& operation, INDEX index)
			noexcept (std::is_nothrow_invocable_v<OPERATION&, INDEX>)
		{
			if (index == invalid_index)
				return;

			T& node = Base::get(index);

			in_order(operation, node.left);
			in_order(operation, node.right);
			std::invoke(operation, index);
		}

		template <std::predicate<INDEX> PREDICATE>
		constexpr bool post_order(PREDICATE& predicate, INDEX index)
			noexcept (std::is_nothrow_invocable_v<PREDICATE&, INDEX>)
		{
			if (index == invalid_index)
				return true;

			T& node = Base::get(index);

			if (!post_order(predicate, node.left))
				return false;

			if (!post_order(predicate, node.right))
				return false;

			return std::invoke(predicate, index);
		}

		template <std::invocable<INDEX> OPERATION>
		constexpr void pre_order(OPERATION& operation, INDEX index)
			noexcept (std::is_nothrow_invocable_v<OPERATION&, INDEX>)
		{
			if (index == invalid_index)
				return;

			T& node = Base::get(index);

			std::invoke(operation, index);
			in_order(operation, node.left);
			in_order(operation, node.right);
		}

		template <std::predicate<INDEX> PREDICATE>
		constexpr bool pre_order(PREDICATE& predicate, INDEX index)
			noexcept (std::is_nothrow_invocable_v<PREDICATE&, INDEX>)
		{
			if (index == invalid_index)
				return true;

			T& node = Base::get(index);

			if (!std::invoke(predicate, index))
				return false;

			if (!pre_order(predicate, node.left))
				return false;

			return pre_order(predicate, node.right);
		}

#pragma endregion

#pragma endregion

	};

}