#pragma once

#include "Container.hpp"

namespace DSA {

	namespace concepts {

		template <typename T>
		concept CollectionDerivation = ContainerDerivation<T> && requires(T t) {

			{ T::is_sparse } -> std::same_as<const bool&>;

		};

		template <typename T>
		concept Collection = Container<T> && CollectionDerivation<T> && requires(T t, typename T::INDEX index, typename T::T data) {

			{ t.add(data) };
			{ t.remove(index) };

			{ t.can_add(index) } -> std::same_as<bool>;
			{ t.can_remove(index) } -> std::same_as<bool>;
			{ t.is_empty() } -> std::same_as<bool>;
			{ t.is_full() } -> std::same_as<bool>;

			{ t.get_size() } -> std::same_as<typename T::INDEX>;

		};

		template <typename T>
		concept CompleteCollection = Collection<T> && requires(T t, typename T::INDEX index, typename T::T* source) {

			{ t.add_many(source) };
			{ t.remove_many(index) };

		};

	}

	struct CollectionDerivation : public ContainerDerivation {

		constexpr static bool is_sparse = false;

	};

	template <concepts::ContainerPolicy ContainerPolicy, typename T_, typename D = CollectionDerivation>
	class Collection : public ContainerPolicy::template STRUCTURE<T_> {
	public:

		using Base = ContainerPolicy::template STRUCTURE<T_>;
		using typename Base::ALLOCATOR;
		using typename Base::INDEX;
		using typename Base::T;

		constexpr static bool is_dynamic = Base::is_dynamic;

		constexpr static bool is_offset = D::is_offset;
		constexpr static bool is_sparse = D::is_sparse;

	protected:

#pragma region Properties

		INDEX size = 0;

#pragma endregion

#pragma region Methods

		template <ResizeOperation RESIZE_OPERATION>
		[[nodiscard]] constexpr INDEX get_target_size(const INDEX newCapacity) {
			if constexpr (RESIZE_OPERATION == ResizeOperation::GROW)
				return get_size();
			else
				return Aggregation::minimum(newCapacity, get_size());
		}

		template <ResizeOperation RESIZE_OPERATION>
			requires (RESIZE_OPERATION != ResizeOperation::UNKNOWN)
		constexpr void helper_resize(const INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires (is_dynamic && !is_offset)
		{
			resize_from_size<RESIZE_OPERATION>(newCapacity, Base::get_size());
		}

		template <ResizeOperation RESIZE_OPERATION>
			requires (RESIZE_OPERATION != ResizeOperation::UNKNOWN)
		constexpr void resize_from_size(const INDEX newCapacity, const INDEX size)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires (is_dynamic && !is_offset)
		{
			if constexpr (std::is_trivially_copyable_v<T>) {
				Base::template resize_from_capacity<RESIZE_OPERATION>(newCapacity, size);
				return;
			}

			T* const newData = ALLOCATOR::allocate(newCapacity);
			T* const oldData = Base::get_data();

			if constexpr (std::is_nothrow_move_constructible_v<T>) {
				if constexpr (RESIZE_OPERATION == ResizeOperation::GROW)
					reconstruct_range(newData, newData + size, oldData);
				else {
					reconstruct_range(newData, newData + newCapacity, oldData);
					Base::set_size(newCapacity);
				}
			}
			else {
				try {
					if constexpr (RESIZE_OPERATION == ResizeOperation::GROW)
						move_construct_range(newData, newData + size, oldData);
					else {
						move_construct_range(newData, newData + newCapacity, oldData);
						destroy_range<Direction::Backward>(oldData, oldData + size);
						Base::set_size(newCapacity);
					}
				}
				catch (...) {
					destroy_range<Direction::Backward>(newData, newData + newCapacity);
					ALLOCATOR::deallocate(newData);
					throw;
				}
			}

			ALLOCATOR::deallocate(oldData);

			Base::set_data(newData);
			Base::set_capacity(newCapacity);
		}

#pragma endregion

	public:

#pragma region Constructors & Destructors

		Collection() : Base() {}

		Collection(const INDEX initialCapacity)
			requires is_dynamic
		: Base(initialCapacity) {}

		Collection(std::initializer_list<T> initializerList) noexcept : Base(initializerList), size(initializerList.size()) {}

		constexpr Collection& operator=(const Collection& other) noexcept
			requires (is_dynamic && std::is_trivial_v<T> && !is_sparse)
		{
			T* const data = Base::get_data();
			const INDEX size = get_size();
			const T* const otherData = other.get_data();
			const INDEX otherSize = other.get_size();

			if (size >= otherSize)
				std::memcpy(data, otherData, otherSize);
			else {
				Base::no_move_resize(otherSize);
				std::memcpy(data, otherData, otherSize);
			}

			set_size(otherSize);

			return *this;
		}

		constexpr Collection& operator=(const Collection& other) noexcept
			requires (!is_dynamic && std::is_trivial_v<T> && !is_sparse)
		{
			std::memcpy(Base::get_data(), other.get_data(), get_size());
			set_size(other.get_size());

			return *this;
		}

		Collection(const Collection& other)
			noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
			requires (!is_offset && !is_sparse)
		: Base(other), size(other.get_size()) {}

		~Collection() noexcept = default;

		~Collection() noexcept
			requires (!std::is_trivially_destructible_v<T> && !is_sparse)
		{
			destroy();
		}

		constexpr void destroy() noexcept
			requires (!is_offset && !is_sparse)
		{
			const T* data = Base::get_data();
			destroy_range<Direction::Backward>(data, data + get_size());
		}

#pragma endregion

#pragma region Methods

#pragma region Checks

		[[nodiscard]] constexpr bool can_add(const INDEX count = 1) const noexcept {
			const INDEX capacity = Base::get_capacity();
			return count <= capacity - get_size();
		}

		[[nodiscard]] constexpr bool can_remove(const INDEX count = 1) const noexcept {
			return get_size() >= count;
		}

		[[nodiscard]] constexpr bool is_empty() const noexcept {
			return get_size() == 0;
		}

		[[nodiscard]] constexpr bool is_full() const noexcept {
			const INDEX capacity = Base::get_capacity();
			return get_size() == capacity;
		}

#pragma endregion

#pragma region Getters

		[[nodiscard]] constexpr INDEX get_size() const noexcept {
			return size;
		}

#pragma endregion

#pragma region Memory Management

		constexpr void double_capacity()
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires (is_dynamic && !is_offset && !is_sparse)
		{
			const INDEX capacity = Base::get_capacity();
			grow(capacity << 1);
		}

		constexpr void empty() noexcept
			requires (!is_offset && !is_sparse)
		{
			destroy();
			Base::set_size(0);
		}

		constexpr void expand(const float ratio = 1.5)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires (is_dynamic && !is_offset && !is_sparse)
		{
			const INDEX capacity = Base::get_capacity();
			grow(Math::ceiling(capacity * ratio));
		}

		constexpr void grow(const INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires (is_dynamic && !is_offset && !is_sparse)
		{
			helper_resize<ResizeOperation::GROW>(newCapacity, Base::get_size());
		}

		constexpr void reserve(const INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires (is_dynamic && !is_offset && !is_sparse)
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity > capacity)
				grow(newCapacity);
		}

		constexpr void resize(const INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires (is_dynamic && !is_offset && !is_sparse)
		{
			const INDEX capacity = Base::get_capacity();
			if (newCapacity >= capacity)
				grow(newCapacity);
			else
				shrink(newCapacity);
		}

		constexpr void shrink(const INDEX newCapacity)
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires (is_dynamic && !is_offset && !is_sparse)
		{
			helper_resize<ResizeOperation::SHRINK>(newCapacity, Base::get_size());
		}

		constexpr void shrink_to_fit()
			noexcept (concepts::NothrowResizableStorage<Base>)
			requires (is_dynamic && !is_offset && !is_sparse)
		{
			shrink(Base::get_size());
		}

#pragma endregion

#pragma region Setters

		constexpr void fill_to_size(const T& value) noexcept {
			return std::fill_n(Base::get_data(), Base::get_capacity(), value);
		}

		constexpr void set_size(const INDEX newSize) noexcept {
			size = newSize;
		}

#pragma endregion

#pragma endregion

	};

}