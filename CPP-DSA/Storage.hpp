#pragma once

#include "DSA.hpp"
#include "Parameter.hpp"

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

		template <typename T, typename U = T>
		concept StoragePolicy = requires(T t) {

			typename T::INDEX;
			typename T::template STRUCTURE<U>;

			{ T::is_dynamic } -> std::same_as<const bool&>;
			{ T::maximum_capacity } -> std::convertible_to<size_t>;
			{ T::maximum_index } -> std::convertible_to<size_t>;

		};

		template <typename T>
		concept DynamicStorage = Storage<T> && HasMutableData<T>;

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

			using Capacity = Parameter<INDEX, !concepts::HasCapacity<This>>;
			using NonExplicitCapacity = Parameter<INDEX, concepts::HasCapacity<This>>;

			std::byte* data = nullptr;

#pragma region Methods

			[[nodiscard]] constexpr INDEX extract_capacity(Capacity explicitCapacity = {}) const noexcept {
				if constexpr (concepts::HasCapacity<This>)
					return this->get_capacity();
				else
					return explicitCapacity.data;
			}

			constexpr void set_data(T* newData) noexcept {
				data = reinterpret_cast<std::byte*>(newData);
			}

#pragma endregion

		public:

#pragma region Constructors & Destructors

			STRUCTURE() noexcept = default;

			STRUCTURE(std::initializer_list<T> initializerList) noexcept
				requires std::is_trivial_v<T>
			{
				set_data(ALLOCATOR::allocate(initializerList.size()));
				std::memcpy(get_data(), initializerList.data(), initializerList.size() * sizeof(T));
			}

			STRUCTURE(std::initializer_list<T> initializerList) noexcept
				requires !std::is_trivial_v<T>
			{
				set_data(ALLOCATOR::allocate(initializerList.size()));
				T* data = get_data();
				for (auto& data : initializerList)
					*(data++) = data;
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

#pragma region Memory Management

			void double_capacity(INDEX capacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				grow(capacity << 1, capacity);
			}

			void grow(INDEX newCapacity, INDEX oldCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				if constexpr (concepts::ReallocatableAllocator<ALLOCATOR>)
					set_data(ALLOCATOR::reallocate(get_data(), newCapacity));
				else {
					T* oldData = get_data();
					set_data(ALLOCATOR::allocate(newCapacity));
					std::memcpy(get_data(), oldData, oldCapacity);
					ALLOCATOR::deallocate(oldData);
				}
			}

			void no_copy_resize(INDEX newCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
			{
				if constexpr (concepts::ReallocatableAllocator<ALLOCATOR>)
					set_data(ALLOCATOR::reallocate(get_data(), newCapacity));
				else {
					ALLOCATOR::deallocate(get_data());
					set_data(ALLOCATOR::allocate(newCapacity));
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
					set_data(ALLOCATOR::reallocate(get_data(), newCapacity));
				else {
					T* oldData = get_data();
					set_data(ALLOCATOR::allocate(newCapacity));
					std::memcpy(get_data(), oldData, newCapacity);
					ALLOCATOR::deallocate(oldData);
				}
			}

#pragma endregion

#pragma region Setters

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

	template <size_t CAPACITY, std::unsigned_integral _INDEX = smallest_uint_t<CAPACITY>>
	struct InlineStoragePolicy {

		using INDEX = _INDEX;

		template <concepts::AllocatorConfiguration Configuration>
		using ALLOCATOR_TEMPLATE = void;

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

			using Capacity = Parameter<INDEX, !concepts::HasCapacity<This>>;
			using NonExplicitCapacity = Parameter<INDEX, concepts::HasCapacity<This>>;

			alignas(T) std::byte data[CAPACITY * sizeof(T)];

#pragma region Methods

			[[nodiscard]] constexpr INDEX extract_capacity(NonExplicitCapacity explicitCapacity = {}) const noexcept {
				return this->get_capacity();
			}

#pragma endregion

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