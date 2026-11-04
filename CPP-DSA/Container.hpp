#pragma once

#include "Allocator.hpp"
#include "StandardAllocator.hpp"
#include "Storage.hpp"

namespace DSA {

	namespace concepts {

		template <typename T>
		concept Container = Storage<T> && requires(T t) {

			{ t.get_capacity() } -> std::same_as<typename T::INDEX>;

		};

		template <typename T>
		concept ContainerConfiguration = StorageConfiguration<T>;

		template <typename T>
		concept DynamicContainer = DynamicStorage<T> && requires(T t, typename T::INDEX newCapacity) {

			{ t.set_capacity(newCapacity) };

		};

		template <typename T>
		concept DynamicContainerConfiguration = DynamicStorageConfiguration<T>;

		template <typename T>
		concept ResizableContainer = DynamicContainer<T> && requires(T t, size_t newCapacity) {

			{ t.resize(newCapacity) };

		};

		template <typename T>
		concept StaticContainer = Container<T> && requires(T t) {

			requires !requires { typename T::ALLOCATOR; };

		};

	}

	template <std::unsigned_integral _INDEX = size_t, template <typename, std::unsigned_integral> typename _ALLOCATOR_TEMPLATE = StandardAllocator>
	struct DynamicContainerConfiguration {

		template <typename T, std::unsigned_integral INDEX>
		using ALLOCATOR_TEMPLATE = _ALLOCATOR_TEMPLATE<T, INDEX>;

		using INDEX = _INDEX;

		constexpr static INDEX maximum_index = std::numeric_limits<INDEX>::max();

		template <typename _T>
			requires concepts::Allocator<ALLOCATOR_TEMPLATE<_T, INDEX>>
		class STRUCTURE : public HeapStorageConfiguration<INDEX, ALLOCATOR_TEMPLATE>::template STRUCTURE<_T> {
		public:

			using Base = typename HeapStorageConfiguration<INDEX, ALLOCATOR_TEMPLATE>::template STRUCTURE<_T>;
			using INDEX = _INDEX;
			using T = _T;

			using ALLOCATOR = ALLOCATOR_TEMPLATE<T, INDEX>;

			constexpr static INDEX maximum_index = std::numeric_limits<INDEX>::max();

		protected:

			INDEX capacity = 0;

#pragma region Methods

			constexpr void set_capacity(INDEX newCapacity) noexcept {
				capacity = newCapacity;
			}

#pragma endregion

		public:

#pragma region Constructors & Destructors

			STRUCTURE() noexcept = default;

			STRUCTURE(INDEX initialCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
			: Base(initialCapacity), capacity(initialCapacity) {}

			STRUCTURE(const STRUCTURE& other)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			: Base(other), capacity(other.capacity) {}

#pragma endregion

#pragma region Methods

#pragma region Checks

			[[nodiscard]] constexpr bool contains_index(INDEX index) const noexcept {
				return index < get_capacity();
			}

#pragma endregion

#pragma region Getters

			[[nodiscard]] constexpr INDEX default_capacity() const noexcept {
				return get_capacity();
			}

			[[nodiscard]] constexpr INDEX get_capacity() const noexcept {
				return capacity;
			}

#pragma endregion

#pragma region Memory Management

			void double_capacity()
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				Base::double_capacity(get_capacity());
			}

			void grow(INDEX newCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				Base::grow(newCapacity, get_capacity());
			}

			void reserve(INDEX newCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				Base::reserve(newCapacity, get_capacity());
			}

			void resize(INDEX newCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				Base::resize(newCapacity, get_capacity());
			}

			void shrink(INDEX newCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				Base::shrink(newCapacity);
			}

			constexpr void one() noexcept {
				std::memset(Base::get_raw_data(), 0xFF, get_capacity() * sizeof(T));
			}

			constexpr void zero() noexcept {
				std::memset(Base::get_raw_data(), 0, get_capacity() * sizeof(T));
			}

#pragma endregion

#pragma endregion

		};

	};

	template <size_t CAPACITY>
	struct StaticContainerConfiguration {

		using _INDEX = smallest_uint_t<CAPACITY - 1>;

		using INDEX = _INDEX;

		constexpr static INDEX maximum_index = std::numeric_limits<INDEX>::max();

		template <typename _T>
		class alignas(_T) STRUCTURE {
		public:

			using INDEX = _INDEX;
			using T = _T;

			constexpr static INDEX maximum_index = std::numeric_limits<INDEX>::max();

		protected:

			alignas(T) std::byte data[CAPACITY * sizeof(T)];

		public:

#pragma region Constructors & Destructors

			STRUCTURE() = default;

			STRUCTURE(const STRUCTURE& other)
				noexcept
				requires std::is_trivial_v<T>
			{
				std::memcpy(get_data(), other.get_data(), CAPACITY);
			}

			template <size_t OTHER_CAPACITY>
			STRUCTURE(const STRUCTURE& other)
				noexcept
				requires std::is_trivial_v<T>
			{
				std::memcpy(get_data(), other.get_data(), OTHER_CAPACITY);
			}

#pragma endregion

#pragma region Methods

#pragma region Checks

			[[nodiscard]] constexpr bool contains_index(INDEX index) const noexcept {
				return index < get_capacity();
			}

#pragma endregion

#pragma region Getters

			[[nodiscard]] constexpr INDEX default_capacity() const noexcept {
				return get_capacity();
			}

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

			[[nodiscard]] constexpr INDEX get_capacity() const noexcept {
				return CAPACITY;
			}

#pragma endregion

#pragma region Memory Management

			constexpr void one() noexcept {
				std::memset(get_raw_data(), 0xFF, get_capacity() * sizeof(T));
			}

			constexpr void one(INDEX count) noexcept {
				std::memset(get_raw_data(), 0xFF, count * sizeof(T));
			}

			constexpr void one(INDEX begin, INDEX count) noexcept {
				std::memset(get_raw_data() + begin, 0xFF, count * sizeof(T));
			}

			constexpr void zero() noexcept {
				std::memset(get_raw_data(), 0, get_capacity() * sizeof(T));
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