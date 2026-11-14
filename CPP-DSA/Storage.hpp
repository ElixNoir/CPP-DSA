#pragma once

#include "DSA.hpp"

#include <cstring>

namespace DSA {

	namespace concepts {

		template <typename T>
		concept Storage = requires(T t, typename T::INDEX index) {

			typename T::INDEX;
			typename T::T;

			{ T::is_dynamic } -> std::same_as<const bool&>;
			{ T::maximum_capacity } -> std::convertible_to<size_t>;
			{ T::maximum_index } -> std::convertible_to<size_t>;

			{ t.get(index) } -> std::same_as<typename T::T&>;
			{ std::as_const(t).get(index) } -> std::same_as<const typename T::T&>;
			{ t.get_data() } -> std::same_as<typename T::T*>;
			{ std::as_const(t).get_data() } -> std::same_as<const typename T::T*>;
			{ t.get_raw_data() } -> std::same_as<std::byte*>;
			{ std::as_const(t).get_raw_data() } -> std::same_as<const std::byte*>;

		};

		template <typename T>
		concept DynamicStorage = Storage<T> && requires(T t) {

			requires T::is_dynamic == true;

		};

		template <typename T>
		concept StaticStorage = Storage<T> && requires(T t) {

			requires T::is_dynamic == false;

		};

		template <typename T, typename U = T>
		concept StoragePolicy = requires(T t) {

			typename T::INDEX;
			typename T::template STRUCTURE<U>;

			{ T::is_dynamic } -> std::same_as<const bool&>;
			{ T::maximum_capacity } -> std::convertible_to<size_t>;
			{ T::maximum_index } -> std::convertible_to<size_t>;

		};

		template <typename T, typename U = T>
		concept DynamicStoragePolicy = StoragePolicy<T, U> && requires(T t) {

			requires T::is_dynamic == true;

		};

		template <typename T, typename U = T>
		concept StaticStoragePolicy = StoragePolicy<T, U> && requires(T t) {

			requires T::is_dynamic == false;

		};

		template <typename BASE, typename T>
		concept NothrowResizableStorage =
			concepts::DynamicStorage<BASE>
			&& concepts::NothrowResizableAllocator<typename BASE::ALLOCATOR>
			&& std::is_nothrow_move_constructible_v<T>;

	}

	template <std::unsigned_integral _INDEX = size_t, template <concepts::AllocatorConfiguration> typename _ALLOCATOR_TEMPLATE = StandardAllocator>
	struct HeapStoragePolicy {

		using INDEX = _INDEX;

		template <concepts::AllocatorConfiguration Configuration>
		using ALLOCATOR_TEMPLATE = StandardAllocator<Configuration>;

		template <typename T>
		using ALLOCATOR = ALLOCATOR_TEMPLATE<
			AllocatorConfiguration<T, INDEX>
		>;

		using This = HeapStoragePolicy<INDEX, ALLOCATOR_TEMPLATE>;

		constexpr static bool is_dynamic = true;
		constexpr static INDEX maximum_capacity = std::numeric_limits<INDEX>::max();
		constexpr static INDEX maximum_index = maximum_capacity - 1;

		template <typename _T>
		class STRUCTURE {
		public:

			using T = _T;
			using ALLOCATOR = ALLOCATOR<T>;
			using INDEX = _INDEX;

			using This = STRUCTURE<T>;

			constexpr static bool is_dynamic = is_dynamic;
			constexpr static INDEX maximum_capacity = maximum_capacity;
			constexpr static INDEX maximum_index = maximum_index;

		protected:

			std::byte* data = nullptr;

		public:

#pragma region Constructors & Destructors

			STRUCTURE() noexcept = default;

			STRUCTURE(INDEX initialCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires is_dynamic
			{
				set_data(ALLOCATOR::allocate(initialCapacity));
			}

			STRUCTURE(std::initializer_list<T> initializerList)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				set_data(ALLOCATOR::allocate(initializerList.size()));
				std::memcpy(get_data(), initializerList.data(), initializerList.size() * sizeof(T));
			}

			STRUCTURE(std::initializer_list<T> initializerList)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires (!std::is_trivial_v<T>)
			{
				set_data(ALLOCATOR::allocate(initializerList.size()));
				T* data = get_data();
				for (auto& value : initializerList)
					*(data++) = std::move(value);
			}

			explicit STRUCTURE(INDEX initialCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
			{
				set_data(ALLOCATOR::allocate(initialCapacity));
			}

			STRUCTURE& operator=(const STRUCTURE&) = delete;

			STRUCTURE(const STRUCTURE&) = delete;

			STRUCTURE(const STRUCTURE& other, INDEX capacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				set_data(ALLOCATOR::allocate(capacity));
				std::memcpy(get_data(), other.get_data(), capacity);
			}

			~STRUCTURE() noexcept {
				ALLOCATOR::deallocate(get_data());
			}

#pragma endregion

#pragma region Methods

#pragma region Getters

			[[nodiscard]] constexpr T* get_data() noexcept {
				return reinterpret_cast<T*>(data);
			}

			[[nodiscard]] constexpr const T* get_data() const noexcept {
				return reinterpret_cast<const T*>(data);
			}

			[[nodiscard]] constexpr std::byte* get_raw_data() noexcept {
				return data;
			}

			[[nodiscard]] constexpr const std::byte* get_raw_data() const noexcept {
				return data;
			}

			[[nodiscard]] constexpr T& get(INDEX index) noexcept {
				return get_data()[index];
			}

			[[nodiscard]] constexpr const T& get(INDEX index) const noexcept {
				return get_data()[index];
			}
			
#pragma endregion

#pragma region Setters

			constexpr void set_data(T* newData) noexcept {
				data = static_cast<std::byte*>(newData);
			}

			constexpr void one(INDEX count, INDEX begin = 0) noexcept {
				std::memset(get_raw_data() + begin, 0xFF, count * sizeof(T));
			}

			constexpr void one_raw(size_t count, INDEX begin = 0) noexcept {
				std::memset(get_raw_data() + begin, 0xFF, count);
			}

			constexpr void zero(INDEX count, INDEX begin = 0) noexcept {
				std::memset(get_raw_data() + begin, 0, count * sizeof(T));
			}

			constexpr void zero_raw(size_t count, INDEX begin = 0) noexcept {
				std::memset(get_raw_data() + begin, 0, count);
			}

#pragma endregion

#pragma endregion

		};

	};

	template <size_t CAPACITY, std::unsigned_integral _INDEX = smallest_uint_t<CAPACITY>>
	struct InlineStoragePolicy {

		using INDEX = _INDEX;

		template <concepts::AllocatorConfiguration Configuration>
		using ALLOCATOR_TEMPLATE = StandardAllocator<Configuration>;

		template <typename T>
		using ALLOCATOR = void;

		using This = InlineStoragePolicy<CAPACITY, INDEX>;

		constexpr static bool is_dynamic = false;
		constexpr static INDEX maximum_capacity = CAPACITY;
		constexpr static INDEX maximum_index = maximum_capacity - 1;

		template <typename _T>
		class STRUCTURE {
		public:

			using ALLOCATOR = void;
			using INDEX = _INDEX;
			using T = _T;

			using This = STRUCTURE<T>;

			constexpr static bool is_dynamic = is_dynamic;
			constexpr static INDEX maximum_capacity = maximum_capacity;
			constexpr static INDEX maximum_index = maximum_index;

		protected:

			alignas(T) std::byte data[CAPACITY * sizeof(T)];

		public:

#pragma region Constructors & Destructors

			STRUCTURE() noexcept = default;

			explicit STRUCTURE(INDEX) noexcept {}

			STRUCTURE(std::initializer_list<T> initializerList) noexcept
				requires std::is_trivial_v<T>
			{
				std::memcpy(get_data(), initializerList.data(), initializerList.size() * sizeof(T));
			}

			STRUCTURE(std::initializer_list<T> initializerList) noexcept
				requires !std::is_trivial_v<T>
			{
				T* data = get_data();
				for (auto& data : initializerList)
					*(data++) = data;
			}

#pragma endregion

#pragma region Methods

#pragma region Checks

			[[nodiscard]] constexpr bool contains_index(INDEX index) const noexcept {
				return index < get_capacity();
			}

#pragma endregion

#pragma region Getters

			[[nodiscard]] constexpr INDEX get_capacity() const noexcept {
				return CAPACITY;
			}

			[[nodiscard]] constexpr T* get_data() noexcept {
				return reinterpret_cast<T*>(data);
			}

			[[nodiscard]] constexpr const T* get_data() const noexcept {
				return reinterpret_cast<const T*>(data);
			}

			[[nodiscard]] constexpr std::byte* get_raw_data() noexcept {
				return data;
			}

			[[nodiscard]] constexpr const std::byte* get_raw_data() const noexcept {
				return data;
			}

			[[nodiscard]] constexpr T& get(INDEX index) noexcept {
				return get_data()[index];
			}

			[[nodiscard]] constexpr const T& get(INDEX index) const noexcept {
				return get_data()[index];
			}

#pragma endregion

#pragma region Setters

			constexpr void one() noexcept {
				one(get_capacity());
			}

			constexpr void one_raw() noexcept {
				one_raw(get_capacity());
			}

			constexpr void zero() noexcept {
				zero(get_capacity());
			}

			constexpr void zero_raw() noexcept {
				zero_raw(get_capacity());
			}

			constexpr void one(INDEX count) noexcept {
				std::memset(get_raw_data(), 0xFF, count * sizeof(T));
			}

			constexpr void one(INDEX count, INDEX begin = 0) noexcept {
				std::memset(get_raw_data() + begin, 0xFF, count * sizeof(T));
			}

			constexpr void one_raw(size_t count, INDEX begin = 0) noexcept {
				std::memset(get_raw_data() + begin, 0xFF, count);
			}

			constexpr void zero(INDEX count) noexcept {
				std::memset(get_raw_data(), 0, count * sizeof(T));
			}

			constexpr void zero(INDEX count, INDEX begin = 0) noexcept {
				std::memset(get_raw_data() + begin, 0, count * sizeof(T));
			}

			constexpr void zero_raw(size_t count, INDEX begin = 0) noexcept {
				std::memset(get_raw_data() + begin, 0, count);
			}

#pragma endregion

#pragma endregion

		};

	};

}