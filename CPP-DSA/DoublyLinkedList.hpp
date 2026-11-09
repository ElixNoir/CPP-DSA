#pragma once

#include "Collection.hpp"
#include "DoublyLinkedListNode.hpp"
#include "Math.hpp"

namespace DSA {

	template <concepts::StoragePolicy StoragePolicy, typename T>
		requires std::is_nothrow_destructible_v<T>
	class DoublyLinkedList : public Collection<StoragePolicy, DoublyLinkedListNode<T, typename StoragePolicy::INDEX>> {
	public:

		using Base = Collection<StoragePolicy, DoublyLinkedListNode<T, typename StoragePolicy::INDEX>>;
		using typename Base::INDEX;
		using typename Base::T;

		using VALUE = typename T::T;

		constexpr static INDEX invalid_index = Base::maximum_index;

	protected:

		INDEX free_head = invalid_index;
		INDEX tail_index = 0;

#pragma region Methods

		constexpr INDEX helper_allocate() noexcept {
			INDEX index = free_head;
			if (index != invalid_index)
				free_head = Base::get(index).next;
			else
				index = Base::size++;
			return index;
		}

		void helper_destroy() noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			const INDEX size = Base::get_size();

			INDEX next = 0;
			for (INDEX counter = 0; counter < size; ++counter) {
				T& node = Base::get(next);
				next = node.next;
				destroy_at(&node.data);
			}
		}

		void helper_resize(INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicContainer<Base>
		{
			using ALLOCATOR = Base::ALLOCATOR;

			const INDEX size = Base::get_size();

			T* const oldData = Base::get_data();
			T* const newData = ALLOCATOR::allocate(newCapacity);

			INDEX counter;
			INDEX index = 0;

			try {
				for (counter = 0; counter < size; ++counter) {
					T& node = oldData[index];
					move_construct_at(newData + index, std::move(node));
					index = node.next;
				}
			}
			catch (...) {
				if constexpr (!std::is_trivially_destructible_v<T>) {
					index = 0;
					for (INDEX counter2 = 0; counter2 < counter; ++counter2) {
						T& node = oldData[index];
						const INDEX next = node.next;
						destroy_at(newData + index);
						index = next;
					}
				}

				ALLOCATOR::deallocate(newData);
				throw;
			}

			if constexpr (!std::is_trivially_destructible_v<T>) {
				index = 0;
				for (counter = 0; counter < size; ++counter) {
					T& node = oldData[index];
					const INDEX next = node.next;
					destroy_at(oldData + index);
					index = next;
				}
			}

			ALLOCATOR::deallocate(oldData);

			Base::set_data(newData);
			if constexpr (concepts::Container<Base>)
				Base::set_capacity(newCapacity);
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		DoublyLinkedList() noexcept
			requires (!Base::is_dynamic)
		{
			T& head = Base::get(0);
			head.next = invalid_index;
			head.previous = 0;
		};

		// `initialCapacity` must be at least 1, since the head node is always allocated.
		DoublyLinkedList(INDEX initialCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires Base::is_dynamic
		: Base(initialCapacity)
		{
			T& head = Base::get(0);
			head.next = invalid_index;
			head.previous = 0;
		};

		~DoublyLinkedList() noexcept = default;

		~DoublyLinkedList() noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			destroy();
		}

		void destroy() noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				helper_destroy();
		}

#pragma endregion

#pragma region Methods

#pragma region Getters

		constexpr INDEX get_free_head_index() const noexcept {
			return free_head;
		}

		constexpr INDEX get_head_index() const noexcept {
			return 0;
		}

		constexpr INDEX get_tail_index() const noexcept {
			return tail_index;
		}

#pragma endregion

#pragma region Add & Remove

		constexpr INDEX add(const VALUE& data)
			noexcept (concepts::is_nothrow_copyable_v<VALUE> && concepts::is_nothrow_moveable_v<T>)
		{
			INDEX index = helper_allocate();
			move_construct_at(Base::get_data() + index, std::move(Base::get(0)));
			T& head = Base::get(0);
			head.data = data;
			head.next = index;
			tail_index = index;
			return index;
		}

		constexpr INDEX add(VALUE&& data)
			noexcept (concepts::is_nothrow_moveable_v<T>)
		{
			INDEX index = helper_allocate();
			move_construct_at(Base::get_data() + index, std::move(Base::get(0)));
			T& head = Base::get(0);
			head.data = std::move(data);
			head.next = index;
			tail_index = index;
			return index;
		}

		/*
		template <typename... Arguments>
		constexpr INDEX emplace(Arguments&&... arguments)
			noexcept(std::is_nothrow_constructible_v<T, Arguments...>)
		{
			INDEX index = helper_allocate();
			reconstruct_at(Base::get_data() + index, std::move(Base::get(0)));
			construct_at(Base::get_data(), std::forward<Arguments>(arguments)...);
			return index;
		}
		*/

		constexpr void remove_head() noexcept {
			T& head = Base::get(0);
			const INDEX nextIndex = head.next;
			destroy_at(&head.data);
			if (nextIndex == invalid_index)
				return;
			const INDEX nextNode = Base::get(nextIndex);
			reconstruct_at(Base::get_data(), std::move(nextNode));
			Base::get(nextNode.next).previous = 0;
		}

		constexpr void remove(const INDEX index) noexcept {
			if (index == 0) {
				remove_head();
				return;
			}

			T& node = Base::get(index);
			destroy_at(&node.data);
			Base::get(node.previous).next = node.next;
			if (index == tail_index) {
				Base::get(node.next).previous = node.previous;
				tail_index = node.previous;
			}
			node.next = free_head;
			free_head = index;
		}

#pragma endregion

#pragma region Append & Prepend

		constexpr INDEX append(const VALUE& data)
			noexcept (concepts::is_nothrow_copyable_v<VALUE> && concepts::is_nothrow_moveable_v<T>)
		{
			INDEX index = helper_allocate();
			copy_construct_at(Base::get_data() + index, data);
			T& tail = Base::get(tail_index);
			tail.next = index;
			tail_index = index;
			return index;
		}

		constexpr INDEX append(VALUE&& data)
			noexcept (concepts::is_nothrow_moveable_v<T>)
		{
			INDEX index = helper_allocate();
			move_construct_at(Base::get_data() + index, std::move(data));
			T& tail = Base::get(tail_index);
			tail.next = index;
			tail_index = index;
			return index;
		}

		constexpr INDEX prepend(const VALUE& data)
			noexcept (concepts::is_nothrow_copyable_v<VALUE> && concepts::is_nothrow_moveable_v<T>)
		{
			return add(data);
		}

		constexpr INDEX prepend(VALUE&& data)
			noexcept (concepts::is_nothrow_moveable_v<T>)
		{
			return add(std::move(data));
		}

#pragma endregion

#pragma region Memory Management

		constexpr void empty() noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				helper_destroy();
			Base::empty();
		}

		constexpr void normalize()
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires Base::is_dynamic
		{
			using ALLOCATOR = Base::ALLOCATOR;

			const INDEX size = Base::get_size();

			T* const oldData = Base::get_data();
			T* const newData = ALLOCATOR::allocate(size);

			INDEX counter;
			INDEX index = 0;

			try {
				for (counter = 0; counter < size; ++counter) {
					T& node = oldData[index];
					move_construct_at(newData + counter, std::move(node));
					index = node.next;
				}
			}
			catch (...) {
				if constexpr (!std::is_trivially_destructible_v<T>) {
					index = 0;
					for (INDEX counter2 = 0; counter2 < counter; ++counter2) {
						T& node = oldData[index];
						const INDEX next = node.next;
						destroy_at(newData + counter2);
						index = next;
					}
				}

				ALLOCATOR::deallocate(newData);
				throw;
			}

			if constexpr (!std::is_trivially_destructible_v<T>) {
				index = 0;
				for (counter = 0; counter < size; ++counter) {
					T& node = oldData[index];
					const INDEX next = node.next;
					destroy_at(oldData + index);
					index = next;
				}
			}

			ALLOCATOR::deallocate(oldData);

			Base::set_data(newData);
			if constexpr (concepts::Container<Base>)
				Base::set_capacity(size);
		}

		/*
		void trim()
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::HasCapacity<Base>
		{

		}
		*/

#pragma endregion

#pragma endregion

	};

}
