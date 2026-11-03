#pragma once

#include "Collection.hpp"
#include "Math.hpp"

namespace DSA {

	template <typename T, std::unsigned_integral INDEX = size_t>
	struct LinkedListNode {

		INDEX next;
		T value;

	};

	template <concepts::StorageConfiguration ContainerConfiguration, typename T>
		requires std::is_nothrow_destructible_v<T>
	class LinkedList : public Collection<ContainerConfiguration, LinkedListNode<T, typename ContainerConfiguration::INDEX>> {
	public:

		using Base = Collection<ContainerConfiguration, LinkedListNode<T, typename ContainerConfiguration::INDEX>>;
		using typename Base::INDEX;
		using typename Base::T;

	protected:

#pragma region Methods

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

#pragma region Add & Remove

		//constexpr void add(const T& value) noexcept {}

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

#pragma endregion

	};

}