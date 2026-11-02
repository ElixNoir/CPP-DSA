#pragma once

#include "DSA.hpp"

namespace DSA {

	namespace concepts {

		template <typename T>
		concept Storage = requires(T t, typename T::INDEX index) {

			typename T::INDEX;
			typename T::T;

			{ t.get(index) } -> std::same_as<typename T::T&>;
			{ std::as_const(t).get(index) } -> std::same_as<const typename T::T&>;
			{ t.get_data() } -> std::same_as<typename T::T*>;
			{ std::as_const(t).get_data() } -> std::same_as<const typename T::T*>;
			{ t.get_raw_data() } -> std::same_as<std::byte*>;
			{ std::as_const(t).get_raw_data() } -> std::same_as<const std::byte*>;

		};

		template <typename T>
		concept StorageConfiguration = requires(T t) {

			typename T::INDEX;
			//typename T::STRUCTURE;

			{ T::maximum_capacity } -> std::convertible_to<std::size_t>;

		};

		template <typename T>
		concept DynamicStorage = Storage<T> && requires(T t, typename T::T* newData) {

			{ t.set_data(newData) };

		};

		template <typename T>
		concept DynamicStorageConfiguration = StorageConfiguration<T> && requires(T t) {

			typename T::template ALLOCATOR_TEMPLATE<void*, typename T::INDEX>;
			requires Allocator<typename T::template ALLOCATOR_TEMPLATE<void*, typename T::INDEX>>;

		};

		template <typename BASE, typename T>
		concept NothrowResizableStorage =
			concepts::DynamicStorage<BASE>
			&& concepts::NothrowResizableAllocator<typename BASE::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>;

	}

	template <std::unsigned_integral _INDEX = size_t, template <typename, std::unsigned_integral> typename _ALLOCATOR_TEMPLATE = StandardAllocator>
	struct HeapStorageConfiguration {

		template <typename T, std::unsigned_integral INDEX>
		using ALLOCATOR_TEMPLATE = _ALLOCATOR_TEMPLATE<T, INDEX>;

		using INDEX = _INDEX;

		constexpr static INDEX maximum_capacity = std::numeric_limits<INDEX>::max();

		template <typename _T>
			requires concepts::Allocator<ALLOCATOR_TEMPLATE<_T, INDEX>>
		class STRUCTURE {
		public:

			using INDEX = _INDEX;
			using T = _T;

			using ALLOCATOR = ALLOCATOR_TEMPLATE<T, INDEX>;

			constexpr static INDEX maximum_capacity = std::numeric_limits<INDEX>::max();

		protected:

			std::byte* data = nullptr;

#pragma region Methods

			constexpr void set_data(std::byte* newData) noexcept {
				data = newData;
			}

			constexpr void set_data(T* newData) noexcept {
				data = reinterpret_cast<std::byte*>(newData);
			}

#pragma endregion

		public:

#pragma region Constructors & Destructors

			STRUCTURE() noexcept = default;

			STRUCTURE(INDEX initialCapacity) {
				data = reinterpret_cast<std::byte*>(ALLOCATOR::allocate(initialCapacity));
			}

			STRUCTURE(const STRUCTURE& other, INDEX capacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				data = reinterpret_cast<std::byte*>(ALLOCATOR::allocate(capacity));
				std::memcpy(get_data(), other.get_data(), capacity);
			}

			~STRUCTURE() noexcept {
				ALLOCATOR::deallocate(get_data());
			}

#pragma endregion

#pragma region Methods

#pragma region Getters

			[[nodiscard]] constexpr T& get(INDEX index) noexcept {
				return get_data()[index];
			}

			[[nodiscard]] constexpr const T& get(INDEX index) const noexcept {
				return get_data()[index];
			}

			[[nodiscard]] constexpr T* get_data() noexcept {
				return reinterpret_cast<T*>(data);
			}

			[[nodiscard]] constexpr const T* get_data() const noexcept {
				return reinterpret_cast<const T*>(data);
			}

			[[nodiscard]] constexpr std::byte* get_raw_data() noexcept {
				return reinterpret_cast<std::byte*>(get_data());
			}

			[[nodiscard]] constexpr const std::byte* get_raw_data() const noexcept {
				return reinterpret_cast<const std::byte*>(get_data());
			}

			[[nodiscard]] constexpr T& operator[](INDEX index) noexcept {
				return get(index);
			}

			[[nodiscard]] constexpr const T& operator[](INDEX index) const noexcept {
				return get(index);
			}

#pragma endregion

#pragma region Memory Management

			void double_capacity(INDEX oldCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				grow(oldCapacity << 1, oldCapacity);
			}

			void grow(INDEX newCapacity, INDEX oldCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				if constexpr (concepts::ReallocatableAllocator<ALLOCATOR>)
					data = reinterpret_cast<std::byte*>(ALLOCATOR::reallocate(get_data(), newCapacity));
				else {
					T* oldData = get_data();
					data = reinterpret_cast<std::byte*>(ALLOCATOR::allocate(newCapacity));
					std::memcpy(get_data(), oldData, oldCapacity);
					ALLOCATOR::deallocate(oldData);
				}
			}

			void reserve(INDEX newCapacity, INDEX oldCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				if (newCapacity > oldCapacity)
					grow(newCapacity, oldCapacity);
			}

			void resize(INDEX newCapacity, INDEX oldCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				if (newCapacity > oldCapacity)
					grow(newCapacity, oldCapacity);
				else
					shrink(newCapacity);
			}

			void shrink(INDEX newCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				if constexpr (concepts::ReallocatableAllocator<ALLOCATOR>)
					data = reinterpret_cast<std::byte*>(ALLOCATOR::reallocate(get_data(), newCapacity));
				else {
					T* oldData = get_data();
					data = reinterpret_cast<std::byte*>(ALLOCATOR::allocate(newCapacity));
					std::memcpy(get_data(), oldData, newCapacity);
					ALLOCATOR::deallocate(oldData);
				}
			}

			constexpr void one(INDEX count) noexcept {
				std::memset(get_raw_data(), 0xFF, count * sizeof(T));
			}

			constexpr void one(INDEX begin, INDEX count) noexcept {
				std::memset(get_raw_data() + begin, 0xFF, count * sizeof(T));
			}

			constexpr void zero(INDEX count) noexcept {
				std::memset(get_raw_data(), 0, count * sizeof(T));
			}

			constexpr void zero(INDEX begin, INDEX count) noexcept {
				std::memset(get_raw_data() + begin, 0, count * sizeof(T));
			}

#pragma endregion

#pragma endregion

		};

	};

}