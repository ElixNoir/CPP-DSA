#pragma once

#include "Collection.hpp"
#include "LinkedListIterator.hpp"
#include "Math.hpp"

namespace DSA {

	template <concepts::StorageConfiguration ContainerConfiguration, typename T>
		requires std::is_nothrow_destructible_v<T>
	class LinkedList : public Collection<ContainerConfiguration, LinkedListNode<T, typename ContainerConfiguration::INDEX>> {
	public:

		using Base = Collection<ContainerConfiguration, LinkedListNode<T, typename ContainerConfiguration::INDEX>>;
		using typename Base::INDEX;
		using typename Base::T;

		using VALUE = typename T::T;

	protected:

		INDEX free_head = Base::maximum_index;

#pragma region Methods

		constexpr INDEX helper_allocate() noexcept {
			INDEX index = free_head;
			if (index != Base::maximum_index)
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
				destroy_at(&node.value);
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

		LinkedList() noexcept
			requires (!concepts::DynamicStorage<Base>)
		{
			Base::get(0).next = Base::maximum_index;
		};

		// `initialCapacity` must be at least 1, since the head node is always allocated.
		LinkedList(INDEX initialCapacity)
			noexcept (concepts::NothrowResizableStorage<Base, T>)
			requires concepts::DynamicStorage<Base>
		: Base(initialCapacity)
		{
			Base::get(0).next = Base::maximum_index;
		};

		~LinkedList() noexcept
			requires (!std::is_trivially_destructible_v<T>)
		{
			destroy();
		}

		~LinkedList() noexcept = default;

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

#pragma endregion

#pragma region Add & Remove

		constexpr INDEX add(const VALUE& value)
			noexcept (concepts::is_nothrow_copyable_v<VALUE> && concepts::is_nothrow_moveable_v<T>)
		{
			INDEX index = helper_allocate();
			move_construct_at(Base::get_data() + index, std::move(Base::get(0)));
			T& head = Base::get(0);
			head.value = value;
			head.next = index;
			return index;
		}

		constexpr INDEX add(VALUE&& value)
			noexcept (concepts::is_nothrow_moveable_v<T>)
		{
			INDEX index = helper_allocate();
			move_construct_at(Base::get_data() + index, std::move(Base::get(0)));
			T& head = Base::get(0);
			head.value = std::move(value);
			head.next = index;
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

		constexpr void remove(const INDEX index) noexcept {
			destroy_at(&Base::get(index).value);
			Base::get(index).next = free_head;
			free_head = index;
		}

#pragma endregion

#pragma region Prepend

		constexpr INDEX prepend(const VALUE& value)
			noexcept (concepts::is_nothrow_copyable_v<VALUE> && concepts::is_nothrow_moveable_v<T>)
		{
			return add(value);
		}

		constexpr INDEX prepend(VALUE&& value)
			noexcept (concepts::is_nothrow_moveable_v<T>)
		{
			return add(std::move(value));
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
			requires concepts::DynamicStorage<Base>
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
			requires concepts::DynamicContainer<Base>
		{

		}
		*/

#pragma endregion

#pragma region Iteration
		
		/*
		LinkedListIterator<T> begin() {
			return LinkedListIterator<T>(Base::get_data(), 0, Base::get_size());
		}

		LinkedListIterator<const T> end() {
			return LinkedListIterator<const T>(Base::get_data(), 0, Base::get_size());
		}
		*/

#pragma endregion

#pragma endregion

	};

}