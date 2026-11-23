#pragma once

#include "Collection.hpp"

namespace DSA {

	namespace concepts {

		template <typename T>
		concept LinkedNode = requires(T t) {

			typename T::NODE_REFERENCE_TYPE;

			{ t.next } -> std::same_as<typename T::NODE_REFERENCE_TYPE>;

		};

		template <typename T>
		concept LinkedValueNode = LinkedNode<T> && requires(T t) {

			typename T::T;

			{ t.value } -> std::same_as<typename T::T>;

		};

	}

	template <std::unsigned_integral _NODE_REFERENCE_TYPE = uint32_t>
	struct LinkedNode {

		using NODE_REFERENCE_TYPE = _NODE_REFERENCE_TYPE;

#pragma region Properties

		NODE_REFERENCE_TYPE next;

#pragma endregion

	};

	template <typename _T, std::unsigned_integral NODE_REFERENCE_TYPE = uint32_t>
	struct LinkedValueNode : public LinkedNode<NODE_REFERENCE_TYPE> {

		using T = _T;

#pragma region Properties

		T value;

#pragma endregion

	};

	class LinkedListDerivation : CollectionDerivation {

		constexpr static bool is_offset = true;
		constexpr static bool is_sparse = true;

	};

	template <concepts::ContainerPolicy ContainerPolicy, concepts::LinkedNode LINKED_NODE = LinkedNode<typename ContainerPolicy::INDEX>>
	class LinkedList : public Collection<ContainerPolicy, LINKED_NODE, LinkedListDerivation> {
	public:

		using Base = Collection<ContainerPolicy, LINKED_NODE, LinkedListDerivation>;
		using typename Base::ALLOCATOR;
		using typename Base::INDEX;
		using typename Base::T;

		using Self = LinkedList<ContainerPolicy, LINKED_NODE>;

		constexpr static bool is_dynamic = Base::is_dynamic;

		constexpr static INDEX invalid_index = Base::invalid_index;

	protected:

#pragma region Properties

		INDEX head = invalid_index;
		INDEX free_head = invalid_index;
		INDEX allocated_top = 0;

#pragma endregion

#pragma region Methods

#pragma region Allocate & Deallocate

		[[nodiscard]] constexpr bool can_allocate(INDEX count = 1) const noexcept {
			return Base::can_add(count);
		}

		[[nodiscard]] constexpr INDEX allocate() noexcept {
			INDEX index = free_head;
			if (index != invalid_index)
				free_head = next_of(index);
			else
				index = allocated_top++;

			++Base::size;

			return index;
		}

		template <typename... ARGUMENTS>
		[[nodiscard]] constexpr INDEX allocate(ARGUMENTS&&... arguments)
			noexcept (std::is_nothrow_constructible_v<T, ARGUMENTS&&...>)
			requires concepts::LinkedValueNode<T>
		{
			INDEX index = free_head;
			if (index != invalid_index) {
				T& node = Base::get(index);
				construct_at(&node.value, std::forward<ARGUMENTS>(arguments)...);
				free_head = node.next;
				node.next = invalid_index;
			}
			else {
				T& node = Base::get(allocated_top);
				construct_at(&node.value, std::forward<ARGUMENTS>(arguments)...);
				index = allocated_top++;
				node.next = invalid_index;
			}

			++Base::size;

			return index;
		}

		[[nodiscard]] constexpr bool can_deallocate(INDEX count = 1) const noexcept {
			return Base::can_remove(count);
		}

		constexpr void deallocate(INDEX index) noexcept {
			T& node = Base::get(index);
			if constexpr (concepts::LinkedValueNode<T>)
				destroy_at(&node.value);
			node.next = free_head;
			free_head = index;
			--Base::size;
		}

#pragma endregion

#pragma region Resize

		template <ResizeOperation RESIZE_OPERATION = ResizeOperation::UNKNOWN, bool IS_DENSE = true>
		void helper_resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			if constexpr (std::is_trivially_copyable_v<T> && IS_DENSE) {
				Base::resize_from_size<RESIZE_OPERATION>(newCapacity, get_allocated_top());
				return;
			}

			T* const newData = ALLOCATOR::allocate(newCapacity);
			T* const oldData = Base::get_data();
			const INDEX targetSize = Base::get_target_size();

			if constexpr (std::is_nothrow_move_constructible_v<T>) {
				INDEX count = targetSize;
				if constexpr (RESIZE_OPERATION == ResizeOperation::SHRINK) {
					forward_order([](INDEX index) -> bool {
						if (count == 0)
							return false;
						reconstruct_at(newData + index, std::move(Base::get(index)));
						--count;
						return true;
						}, head);
				}
				else {
					forward_order([](INDEX index) {
						reconstruct_at(newData + index, std::move(Base::get(index)));
						}, head);
				}
			}
			else {
				INDEX count = targetSize;
				if constexpr (RESIZE_OPERATION == ResizeOperation::SHRINK) {
					try {
						forward_order([](INDEX index) -> bool {
							if (count == 0)
								return false;
							move_construct_at(newData + index, std::move(Base::get(index)));
							--count;
							return true;
							}, head);
					}
					catch (...) {
						forward_order([](INDEX index) -> bool {
							if (count == targetSize)
								return false;
							destroy_at(newData + index);
							++count;
							return true;
							}, head);

						ALLOCATOR::deallocate(newData);
						throw;
					}
				}
				else {
					try {
						forward_order([](INDEX index) {
							move_construct_at(newData + index, std::move(Base::get(index)));
							}, head);
					}
					catch (...) {
						forward_order([](INDEX index) {
							destroy_at(newData + index);
							}, head);

						ALLOCATOR::deallocate(newData);
						throw;
					}
				}

				if constexpr (!std::is_trivially_destructible_v<T>) {
					forward_order([](INDEX index) {
						destroy_at(oldData + index);
						}, head);
				}
			}

			ALLOCATOR::deallocate(oldData);

			Base::set_capacity(newCapacity);
			Base::set_data(newData);
			if constexpr (RESIZE_OPERATION == ResizeOperation::SHRINK)
				Base::set_size(targetSize);
		}

#pragma endregion

#pragma endregion

	public:

#pragma region Constructors & Destructors

		using Base::Base;

		~LinkedList() noexcept = default;

		~LinkedList() noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			destroy();
		}

		constexpr void destroy() noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>) {
				forward_order([](const INDEX index) {
					destroy_at(Base::get_data() + index);
				}, head);
			}
		}

#pragma endregion

#pragma region Methods

#pragma region Getters

		[[nodiscard]] constexpr INDEX get_allocated_top() const noexcept {
			return allocated_top;
		}

		[[nodiscard]] constexpr INDEX get_free_head() const noexcept {
			return free_head;
		}

		[[nodiscard]] constexpr INDEX get_head() const noexcept {
			return head;
		}

		[[nodiscard]] constexpr INDEX next_of(INDEX index) const noexcept {
			return Base::get(index).next;
		}

		[[nodiscard]] constexpr auto value_of(INDEX index) const noexcept
			requires concepts::LinkedValueNode<T>
		{
			return Base::get(index).value;
		}

#pragma endregion

#pragma region Add & Remove

		[[nodiscard]] constexpr INDEX add(const T::T& value)
			noexcept (concepts::is_nothrow_copyable_v<T>)
			requires concepts::LinkedValueNode<T>
		{
			const INDEX index = allocate(value);
			T& node = Base::get(index);
			/*
			if (head == invalid_index)
				tail = index;
			*/
			head = index;
			return index;
		}

		[[nodiscard]] constexpr INDEX add(T::T&& value)
			noexcept (concepts::is_nothrow_moveable_v<T>)
			requires concepts::LinkedValueNode<T>
		{
			const INDEX index = allocate(std::move(value));
			T& node = Base::get(index);
			/*
			if (head == invalid_index)
				tail = index;
			*/
			head = index;
			return index;
		}

		[[nodiscard]] constexpr bool can_remove_head() noexcept {
			return Base::can_remove();
		}

		constexpr void remove_head() noexcept {
			const INDEX nextHead = head.next;
			deallocate(head);
			head = nextHead;
		}

		[[nodiscard]] constexpr bool can_remove_next_of(INDEX index) noexcept {
			return next_of(index) != invalid_index;
		}

		[[nodiscard]] constexpr void remove_next_of(INDEX index) noexcept {
			const INDEX nextIndex = next_of(index);
			const INDEX nextNextIndex = next_of(nextIndex);
			deallocate(nextIndex);
			Base::get(index).next = nextNextIndex;
		}

#pragma endregion

#pragma region Append, Pop, & Prepend

		[[nodiscard]] constexpr INDEX append_to(const T::T& value, const INDEX index)
			noexcept (concepts::is_nothrow_copyable_v<T>)
			requires concepts::LinkedValueNode<T>
		{
			const INDEX newIndex = allocate(value);
			const INDEX nextIndex = next_of(index);
			Base::get(newIndex).next = nextIndex;
			Base::get(index).next = newIndex;
			return newIndex;
		}

		[[nodiscard]] constexpr INDEX append_to(T::T&& value, const INDEX index)
			noexcept (concepts::is_nothrow_moveable_v<T>)
			requires concepts::LinkedValueNode<T>
		{
			const INDEX newIndex = allocate(std::move(value));
			const INDEX nextIndex = next_of(index);
			Base::get(newIndex).next = nextIndex;
			Base::get(index).next = newIndex;
			return newIndex;
		}

		[[nodiscard]] constexpr INDEX prepend(const T::T& value)
			noexcept (concepts::is_nothrow_moveable_v<T>)
			requires concepts::LinkedValueNode<T>
		{
			return add(value);
		}

		[[nodiscard]] constexpr INDEX prepend(T::T&& value)
			noexcept (concepts::is_nothrow_moveable_v<T>)
			requires concepts::LinkedValueNode<T>
		{
			return add(std::move(value));
		}

		[[nodiscard]] constexpr bool can_pop() noexcept {
			return can_remove_head();
		}

		constexpr void pop() noexcept {
			remove_head();
		}

		[[nodiscard]] constexpr auto pop()
			noexcept (std::is_nothrow_move_constructible_v<T>)
			requires concepts::LinkedValueNode<T>
		{
			const auto value = std::move(head.value);
			remove_head();
			return value;
		}

		[[nodiscard]] constexpr bool can_pop_next_of(INDEX index) noexcept {
			return can_remove_next_of(index);
		}

		constexpr void pop_next_of(INDEX index) noexcept {
			remove_next_of(index);
		}

		[[nodiscard]] constexpr auto pop_next_of(INDEX index)
			noexcept (std::is_nothrow_move_constructible_v<T>)
			requires concepts::LinkedValueNode<T>
		{
			const INDEX nextIndex = next_of(index);
			const auto value = std::move(value_of(nextIndex));
			const INDEX nextNextIndex = next_of(nextIndex);
			deallocate(nextIndex);
			Base::get(index).next = nextNextIndex;
			return value;
		}

#pragma endregion

#pragma region Iteration

		/*
		constexpr Iterator<T> begin() noexcept {
			return Iterator<T>(Base::get_data());
		}

		constexpr Iterator<const T> begin() const noexcept {
			return Iterator<const T>(Base::get_data());
		}

		constexpr Iterator<T> end() noexcept {
			return Iterator<T>(Base::get_data() + Base::get_size());
		}

		constexpr Iterator<const T> end() const noexcept {
			return Iterator<const T>(Base::get_data() + Base::get_size());
		}

		constexpr Iterator<T> iterator_at(INDEX index) noexcept {
			return Iterator<T>(Base::get_data() + index);
		}

		constexpr Iterator<const T> iterator_at(INDEX index) const noexcept {
			return Iterator<const T>(Base::get_data() + index);
		}
		*/

#pragma endregion

#pragma region Memory Management

		constexpr void double_capacity()
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			grow(capacity << 1);
		}

		constexpr void empty() noexcept {
			destroy();
			Base::set_size(0);
			head = invalid_index;
			free_head = invalid_index;
			allocated_top = 0;
		}

		constexpr void expand(float ratio = 1.5)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			grow(Math::ceiling(capacity * ratio));
		}

		constexpr void grow(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			helper_resize<ResizeOperation::GROW>(newCapacity);
		}

		constexpr void reserve(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity > capacity)
				grow(newCapacity);
		}

		constexpr void resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity >= capacity)
				grow(newCapacity);
			else
				shrink(newCapacity);
		}

		constexpr void shrink(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			helper_resize<ResizeOperation::SHRINK>(newCapacity);
		}

#pragma endregion

#pragma region Setters

		constexpr void set_allocated_top(INDEX newAllocatedTop) noexcept {
			allocated_top = newAllocatedTop;
		}

		constexpr void set_free_head(INDEX newFreeHead) noexcept {
			free_head = newFreeHead;
		}

		constexpr void set_head(INDEX newHead) noexcept {
			head = newHead;
		}

#pragma endregion

#pragma region Seek

		[[nodiscard]] constexpr INDEX seek(INDEX nth) const noexcept {
			return seek(nth, head);
		}

		[[nodiscard]] constexpr INDEX seek(INDEX nth, INDEX begin) const noexcept {
			INDEX current = begin;
			for (INDEX index = 0; index < nth && current != invalid_index; ++index)
				current = next_of(current);

			return current;
		}

		[[nodiscard]] constexpr INDEX seek_previous_of(INDEX index) const noexcept {
			return seek_previous_of(index, head);
		}

		[[nodiscard]] constexpr INDEX seek_previous_of(INDEX index, INDEX begin) const noexcept {
			INDEX current = begin;
			while (current != invalid_index) {
				const INDEX nextIndex = next_of(current);
				if (nextIndex == index)
					return nextIndex;

				current = nextIndex;
			}

			return invalid_index;
		}

#pragma endregion

#pragma region Traverse

		template <std::invocable<INDEX> OPERATION>
		constexpr void forward_order(OPERATION& operation, INDEX index)
			noexcept (std::is_nothrow_invocable_v<OPERATION&, INDEX>)
		{
			while (index != invalid_index) {
				const INDEX nextIndex = next_of(index);
				operation(index);
				index = nextIndex;
			}
		}

		template <std::predicate<INDEX> PREDICATE>
		constexpr void forward_order(PREDICATE& predicate, INDEX index)
			noexcept (std::is_nothrow_invocable_v<PREDICATE&, INDEX>)
		{
			while (index != invalid_index) {
				const INDEX nextIndex = next_of(index);
				if (!predicate(index))
					return;
				index = nextIndex;
			}
		}

#pragma endregion

#pragma endregion

	};

}
