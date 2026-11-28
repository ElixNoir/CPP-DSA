#pragma once

#include "Collection.hpp"

namespace DSA {

	namespace concepts {

		template <typename T>
		concept DoublyLinkedNode = requires(T t) {

			typename T::NODE_REFERENCE_TYPE;

			{ t.next } -> std::same_as<typename T::NODE_REFERENCE_TYPE>;
			{ t.previous } -> std::same_as<typename T::NODE_REFERENCE_TYPE>;

		};

		template <typename T>
		concept DoublyLinkedValueNode = DoublyLinkedNode<T> && requires(T t) {

			typename T::T;

			{ t.value } -> std::same_as<typename T::T>;

		};

	}

	template <std::unsigned_integral NODE_REFERENCE_TYPE_ = uint32_t>
	struct DoublyLinkedNode {

		using NODE_REFERENCE_TYPE = NODE_REFERENCE_TYPE_;

#pragma region Properties

		NODE_REFERENCE_TYPE next;
		NODE_REFERENCE_TYPE previous;

#pragma endregion

	};

	template <typename T_, std::unsigned_integral NODE_REFERENCE_TYPE = uint32_t>
	struct DoublyLinkedValueNode : public DoublyLinkedNode<NODE_REFERENCE_TYPE> {

		using T = T_;

#pragma region Properties

		T value;

#pragma endregion

	};

	class DoublyLinkedListDerivation : CollectionDerivation {

		constexpr static bool is_offset = true;
		constexpr static bool is_sparse = true;

	};

	template <concepts::ContainerPolicy ContainerPolicy, concepts::DoublyLinkedNode LINKED_NODE = DoublyLinkedNode<typename ContainerPolicy::INDEX>>
	class DoublyLinkedList : public Collection<ContainerPolicy, LINKED_NODE, DoublyLinkedListDerivation> {
	public:

		using Base = Collection<ContainerPolicy, LINKED_NODE, DoublyLinkedListDerivation>;
		using typename Base::ALLOCATOR;
		using typename Base::INDEX;
		using typename Base::T;

		using Self = DoublyLinkedList;

		constexpr static bool is_dynamic = Base::is_dynamic;

		constexpr static INDEX invalid_index = Base::invalid_index;

	protected:

#pragma region Properties

		INDEX head = invalid_index;
		INDEX tail = invalid_index;
		INDEX free_head = invalid_index;
		INDEX allocated_top = 0;

#pragma endregion

#pragma region Methods

#pragma region Allocate & Deallocate

		[[nodiscard]] constexpr bool can_allocate(const INDEX count = 1) const noexcept {
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
			requires concepts::DoublyLinkedValueNode<T>
		{
			INDEX index = free_head;
			if (index != invalid_index) {
				T& node = Base::get(index);
				construct_at(&node.value, std::forward<ARGUMENTS>(arguments)...);
				free_head = node.next;
				node.next = invalid_index;
				node.previous = invalid_index;
			}
			else {
				T& node = Base::get(allocated_top);
				construct_at(&node.value, std::forward<ARGUMENTS>(arguments)...);
				index = allocated_top++;
				node.next = invalid_index;
				node.previous = invalid_index;
			}

			++Base::size;

			return index;
		}

		[[nodiscard]] constexpr bool can_deallocate(const INDEX count = 1) const noexcept {
			return Base::can_remove(count);
		}

		constexpr void deallocate(const INDEX index) noexcept {
			T& node = Base::get(index);
			if constexpr (concepts::DoublyLinkedValueNode<T>)
				destroy_at(&node.value);
			node.next = free_head;
			free_head = index;
			--Base::size;
		}

#pragma endregion

#pragma region Resize

		template <ResizeOperation RESIZE_OPERATION, bool IS_DENSE = true>
			requires (RESIZE_OPERATION != ResizeOperation::UNKNOWN)
		constexpr void helper_resize(const INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			if constexpr (std::is_trivially_copyable_v<T> && IS_DENSE) {
				Base::template resize_from_size<RESIZE_OPERATION>(newCapacity, get_allocated_top());
				return;
			}

			T* const newData = ALLOCATOR::allocate(newCapacity);
			T* const oldData = Base::get_data();
			const INDEX targetSize = Base::get_target_size();

			if constexpr (std::is_nothrow_move_constructible_v<T>) {
				INDEX count = targetSize;
				if constexpr (RESIZE_OPERATION == ResizeOperation::SHRINK) {
					forward_order([count, newData](INDEX index) -> bool {
						if (count == 0)
							return false;
						reconstruct_at(newData + index, std::move(Base::get(index)));
						--count;
						return true;
					}, head);
				}
				else {
					forward_order([newData](INDEX index) {
						reconstruct_at(newData + index, std::move(Base::get(index)));
					}, head);
				}
			}
			else {
				INDEX count = targetSize;
				if constexpr (RESIZE_OPERATION == ResizeOperation::SHRINK) {
					try {
						forward_order([count, newData](INDEX index) -> bool {
							if (count == 0)
								return false;
							move_construct_at(newData + index, std::move(Base::get(index)));
							--count;
							return true;
						}, head);
					}
					catch (...) {
						forward_order([count, newData, targetSize](INDEX index) -> bool {
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
						forward_order([newData](INDEX index) {
							move_construct_at(newData + index, std::move(Base::get(index)));
						}, head);
					}
					catch (...) {
						forward_order([newData](INDEX index) {
							destroy_at(newData + index);
						}, head);

						ALLOCATOR::deallocate(newData);
						throw;
					}
				}

				if constexpr (!std::is_trivially_destructible_v<T>) {
					forward_order([oldData](INDEX index) {
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

		~DoublyLinkedList() noexcept = default;

		~DoublyLinkedList() noexcept
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

		[[nodiscard]] constexpr INDEX next_of(const INDEX index) const noexcept {
			return Base::get(index).next;
		}

		[[nodiscard]] constexpr INDEX previous_of(const INDEX index) const noexcept {
			return Base::get(index).previous;
		}

		[[nodiscard]] constexpr auto value_of(const INDEX index) const noexcept
			requires concepts::DoublyLinkedValueNode<T>
		{
			return Base::get(index).value;
		}

#pragma endregion

#pragma region Add & Remove

		[[nodiscard]] constexpr INDEX add(const T::T& value)
			noexcept (concepts::is_nothrow_copyable_v<T>)
			requires concepts::DoublyLinkedValueNode<T>
		{
			const INDEX index = allocate(value);
			if (head == invalid_index)
				tail = index;
			else
				Base::get(index).next = head;
			head = index;

			return index;
		}

		[[nodiscard]] constexpr INDEX add(T::T&& value)
			noexcept (concepts::is_nothrow_moveable_v<T>)
			requires concepts::DoublyLinkedValueNode<T>
		{
			const INDEX index = allocate(std::move(value));
			if (head == invalid_index)
				tail = index;
			else
				Base::get(index).next = head;
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
			if (nextHead == invalid_index)
				tail = invalid_index;
		}

		[[nodiscard]] constexpr bool can_remove_next_of(const INDEX index) noexcept {
			return next_of(index) != invalid_index;
		}

		constexpr void remove_next_of(const INDEX index) noexcept {
			const INDEX nextIndex = next_of(index);
			const INDEX nextNextIndex = next_of(nextIndex);
			deallocate(nextIndex);
			Base::get(index).next = nextNextIndex;
		}

		[[nodiscard]] constexpr bool can_remove_previous_of(const INDEX index) noexcept {
			return previous_of(index) != invalid_index;
		}

		constexpr void remove_previous_of(const INDEX index) noexcept {
			const INDEX previousIndex = previous_of(index);
			const INDEX previousPreviousIndex = previous_of(previousIndex);
			deallocate(previousIndex);
			Base::get(index).previous = previousPreviousIndex;
		}

		[[nodiscard]] constexpr bool can_remove_tail() noexcept {
			return Base::can_remove();
		}

		constexpr void remove_tail() noexcept {
			const INDEX nextTail = tail.previous;
			deallocate(tail);
			tail = nextTail;
			if (nextTail == invalid_index)
				head = invalid_index;
		}

#pragma endregion

#pragma region Append, Pop, & Prepend

		[[nodiscard]] constexpr INDEX append(const T::T& value)
			noexcept (concepts::is_nothrow_moveable_v<T>)
			requires concepts::DoublyLinkedValueNode<T>
		{
			const INDEX index = allocate(value);
			if (tail == invalid_index)
				head = index;
			else
				Base::get(index).previous = tail;
			tail = index;

			return index;
		}

		[[nodiscard]] constexpr INDEX append(T::T&& value)
			noexcept (concepts::is_nothrow_moveable_v<T>)
			requires concepts::DoublyLinkedValueNode<T>
		{
			const INDEX index = allocate(std::move(value));
			if (tail == invalid_index)
				head = index;
			else
				Base::get(index).previous = tail;
			tail = index;

			return index;
		}

		[[nodiscard]] constexpr INDEX append_to(const T::T& value, const INDEX index)
			noexcept (concepts::is_nothrow_copyable_v<T>)
			requires concepts::DoublyLinkedValueNode<T>
		{
			const INDEX newIndex = allocate(value);
			const INDEX nextIndex = next_of(index);
			Base::get(newIndex).next = nextIndex;
			Base::get(newIndex).previous = index;
			Base::get(index).next = newIndex;
			if (nextIndex == invalid_index)
				tail = newIndex;
			else
				Base::get(nextIndex).previous = newIndex;
			return newIndex;
		}

		[[nodiscard]] constexpr INDEX append_to(T::T&& value, const INDEX index)
			noexcept (concepts::is_nothrow_moveable_v<T>)
			requires concepts::DoublyLinkedValueNode<T>
		{
			const INDEX newIndex = allocate(std::move(value));
			const INDEX nextIndex = next_of(index);
			Base::get(newIndex).next = nextIndex;
			Base::get(newIndex).previous = index;
			Base::get(index).next = newIndex;
			if (nextIndex == invalid_index)
				tail = newIndex;
			else
				Base::get(nextIndex).previous = newIndex;
			return newIndex;
		}

		[[nodiscard]] constexpr INDEX prepend(const T::T& value)
			noexcept (concepts::is_nothrow_moveable_v<T>)
			requires concepts::DoublyLinkedValueNode<T>
		{
			return add(value);
		}

		[[nodiscard]] constexpr INDEX prepend(T::T&& value)
			noexcept (concepts::is_nothrow_moveable_v<T>)
			requires concepts::DoublyLinkedValueNode<T>
		{
			return add(std::move(value));
		}

		[[nodiscard]] constexpr INDEX prepend_to(const T::T& value, const INDEX index)
			noexcept (concepts::is_nothrow_copyable_v<T>)
			requires concepts::DoublyLinkedValueNode<T>
		{
			const INDEX newIndex = allocate(value);
			const INDEX previousIndex = previous_of(index);
			Base::get(newIndex).next = index;
			Base::get(newIndex).previous = previousIndex;
			Base::get(index).previous = newIndex;
			if (previousIndex == invalid_index)
				head = newIndex;
			else
				Base::get(previousIndex).next = newIndex;
			return newIndex;
		}

		[[nodiscard]] constexpr INDEX prepend_to(T::T&& value, const INDEX index)
			noexcept (concepts::is_nothrow_moveable_v<T>)
			requires concepts::DoublyLinkedValueNode<T>
		{
			const INDEX newIndex = allocate(std::move(value));
			const INDEX previousIndex = previous_of(index);
			Base::get(newIndex).previous = previousIndex;
			Base::get(newIndex).next = index;
			Base::get(index).previous = newIndex;
			if (previousIndex == invalid_index)
				head = newIndex;
			else
				Base::get(previousIndex).next = newIndex;
			return newIndex;
		}

		[[nodiscard]] constexpr bool can_pop() noexcept {
			return can_remove_head();
		}

		constexpr void pop() noexcept {
			remove_head();
		}

		[[nodiscard]] constexpr auto pop()
			noexcept (std::is_nothrow_move_constructible_v<T>)
			requires concepts::DoublyLinkedValueNode<T>
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
			requires concepts::DoublyLinkedValueNode<T>
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
		
		template <concepts::DoublyLinkedNode T_>
		class Iterator {
		public:

			using INDEX = Base::INDEX;
			using T = T_;

#pragma region C++ Iterator Traits

			using iterator_category = std::bidirectional_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = T*;
			using reference = T&;

#pragma endregion

		protected:

#pragma region Properties

			T* data;
			INDEX index;

#pragma endregion

		public:

#pragma region Constructors & Destructors

			constexpr Iterator(T* data, INDEX first)
				noexcept : data(data), index(first) {}

#pragma endregion

#pragma region Operators

			[[nodiscard]] constexpr T& operator*() const noexcept {
				return data[index];
			}

			constexpr Iterator& operator++() noexcept {
				index = data[index].next;
				return *this;
			}
			
			constexpr Iterator& operator--() noexcept {
				index = data[index].previous;
				return *this;
			}

			[[nodiscard]] constexpr bool operator!=(const Iterator& other) const noexcept {
				return index != other.index;
			}

#pragma endregion

		};
		
		constexpr Iterator<T> begin() noexcept {
			return Iterator<T>(Base::get_data() + head);
		}

		constexpr Iterator<const T> begin() const noexcept {
			return Iterator<const T>(Base::get_data() + head);
		}
		
		constexpr Iterator<T> end() noexcept {
			return Iterator<T>(Base::get_data() + tail);
		}

		constexpr Iterator<const T> end() const noexcept {
			return Iterator<const T>(Base::get_data() + tail);
		}

		constexpr Iterator<T> iterator_at(const INDEX index) noexcept {
			return Iterator<T>(Base::get_data() + index);
		}

		constexpr Iterator<const T> iterator_at(const INDEX index) const noexcept {
			return Iterator<const T>(Base::get_data() + index);
		}

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

		constexpr void expand(const float ratio = 1.5)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			grow(Math::ceiling(capacity * ratio));
		}

		constexpr void grow(const INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			helper_resize<ResizeOperation::GROW>(newCapacity);
		}

		constexpr void reserve(const INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity > capacity)
				grow(newCapacity);
		}

		constexpr void resize(const INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity >= capacity)
				grow(newCapacity);
			else
				shrink(newCapacity);
		}

		constexpr void shrink(const INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires is_dynamic
		{
			helper_resize<ResizeOperation::SHRINK>(newCapacity);
		}

#pragma endregion

#pragma region Setters

		constexpr void set_allocated_top(const INDEX newAllocatedTop) noexcept {
			allocated_top = newAllocatedTop;
		}

		constexpr void set_free_head(const INDEX newFreeHead) noexcept {
			free_head = newFreeHead;
		}

		constexpr void set_head(const INDEX newHead) noexcept {
			head = newHead;
		}

#pragma endregion

#pragma region Traverse

		template <std::invocable<INDEX> OPERATION>
		constexpr void backward_order(OPERATION& operation, INDEX index)
			noexcept (std::is_nothrow_invocable_v<OPERATION&, INDEX>)
		{
			while (index != invalid_index) {
				const INDEX previousIndex = previous_of(index);
				operation(index);
				index = previousIndex;
			}
		}

		template <std::predicate<INDEX> PREDICATE>
		constexpr void backward_order(PREDICATE& predicate, INDEX index)
			noexcept (std::is_nothrow_invocable_v<PREDICATE&, INDEX>)
		{
			while (index != invalid_index) {
				const INDEX previousIndex = previous_of(index);
				if (!predicate(index))
					return;
				index = previousIndex;
			}
		}

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