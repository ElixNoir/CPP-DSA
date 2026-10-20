#pragma once

#include "Allocator.hpp"
#include "StandardAllocator.hpp"
#include "DSA.hpp"

namespace DSA {

	namespace concepts {

		template <typename T>
		concept Container = requires(T t, typename T::INDEX index) {

			typename T::INDEX;
			typename T::T;

			{ t.get_capacity() } -> std::same_as<typename T::INDEX>;
			{ t.get(index) } -> std::same_as<typename T::T&>;
			{ std::as_const(t).get(index) } -> std::same_as<const typename T::T&>;
			{ t.get_data() } -> std::same_as<typename T::T*>;
			{ std::as_const(t).get_data() } -> std::same_as<const typename T::T*>;
			{ t.get_raw_data() } -> std::same_as<std::byte*>;
			{ std::as_const(t).get_raw_data() } -> std::same_as<const std::byte*>;

		};

		template <typename T>
		concept DynamicContainer = Container<T> && requires(T t) {

			typename T::ALLOCATOR;

		};

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

		constexpr static _INDEX maximum_capacity = std::numeric_limits<_INDEX>::max();

		template <typename _T>
			requires concepts::Allocator<ALLOCATOR_TEMPLATE<_T, _INDEX>>
		class CONTAINER {
		public:

			using INDEX = _INDEX;
			using T = _T;

			using ALLOCATOR = ALLOCATOR_TEMPLATE<T, INDEX>;

		protected:

			std::byte* data = nullptr;
			INDEX capacity = 0;

		public:

#pragma region Constructors & Destructors

			CONTAINER() noexcept = default;

			CONTAINER(INDEX initialCapacity) : capacity(initialCapacity) {
				data = reinterpret_cast<std::byte*>(ALLOCATOR::allocate(initialCapacity));
			}

			~CONTAINER() noexcept {
				ALLOCATOR::deallocate(get_data());
			}

#pragma endregion

#pragma region Methods

#pragma region Assignment

			CONTAINER& operator=(CONTAINER&& other) noexcept {
				if (this == &other)
					return *this;

				ALLOCATOR::deallocate(data);

				data = std::exchange(other.get_raw_data(), nullptr);
				capacity = std::exchange(other.capacity, 0);

				return *this;
			}

#pragma endregion

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

			[[nodiscard]] constexpr INDEX get_capacity() const noexcept {
				return capacity;
			}

#pragma endregion

#pragma region Memory Management

			void double_capacity()
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				grow(get_capacity() << 1);
			}

			void grow(INDEX newCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				if constexpr (concepts::ReallocatableAllocator<ALLOCATOR>)
					data = reinterpret_cast<std::byte*>(ALLOCATOR::reallocate(get_data(), newCapacity));
				else {
					T* oldData = get_data();
					data = reinterpret_cast<std::byte*>(ALLOCATOR::allocate(newCapacity));
					std::memcpy(get_data(), oldData, get_capacity());
					ALLOCATOR::deallocate(oldData);
				}
			}

			void reserve(INDEX newCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				if (newCapacity > get_capacity())
					grow(newCapacity);
			}

			void resize(INDEX newCapacity)
				noexcept (concepts::NothrowResizableAllocator<ALLOCATOR>)
				requires std::is_trivial_v<T>
			{
				if (newCapacity > get_capacity())
					grow(newCapacity);
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

			constexpr void one() noexcept {
				std::memset(get_raw_data(), 0xFF, capacity * sizeof(T));
			}

			constexpr void zero() noexcept {
				std::memset(get_raw_data(), 0, capacity * sizeof(T));
			}

			constexpr void one(INDEX begin, INDEX count) noexcept {
				std::memset(get_raw_data() + begin, 0xFF, count * sizeof(T));
			}

			constexpr void zero(INDEX begin, INDEX count) noexcept {
				std::memset(get_raw_data() + begin, 0, count * sizeof(T));
			}

#pragma endregion

#pragma endregion

		};

	};

	template <size_t CAPACITY>
		requires (CAPACITY > 0)
	struct StaticContainerConfiguration {

		using _INDEX = smallest_uint_t<CAPACITY - 1>;

		template <typename T, std::unsigned_integral INDEX>
		using ALLOCATOR_TEMPLATE = void;

		constexpr static _INDEX maximum_capacity = CAPACITY;

		template <typename _T>
		class alignas(_T) CONTAINER {
		public:

			using INDEX = _INDEX;
			using T = _T;

			constexpr static INDEX maximum_capacity = CAPACITY;

		protected:

			alignas(T) std::byte data[CAPACITY * sizeof(T)];

		public:

#pragma region Constructors & Destructors

			CONTAINER() = default;

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

			[[nodiscard]] constexpr INDEX get_capacity() const noexcept {
				return CAPACITY;
			}

#pragma endregion

#pragma region Memory Management

			constexpr void one() noexcept {
				std::memset(get_raw_data(), 0xFF, get_capacity() * sizeof(T));
			}

			constexpr void zero() noexcept {
				std::memset(get_raw_data(), 0, get_capacity() * sizeof(T));
			}

			constexpr void one(INDEX begin, INDEX count) noexcept {
				std::memset(get_raw_data() + begin, 0xFF, count * sizeof(T));
			}

			constexpr void zero(INDEX begin, INDEX count) noexcept {
				std::memset(get_raw_data() + begin, 0, count * sizeof(T));
			}

#pragma endregion

#pragma endregion

		};

	};

	template <std::unsigned_integral _INDEX = size_t, template <typename, std::unsigned_integral> typename _ALLOCATOR_TEMPLATE = StandardAllocator>
	struct AlternateDynamicContainerConfiguration {

		template <typename T, std::unsigned_integral INDEX>
		using ALLOCATOR_TEMPLATE = _ALLOCATOR_TEMPLATE<T, INDEX>;

		constexpr static _INDEX maximum_capacity = std::numeric_limits<_INDEX>::max();

		template <typename _T>
			requires concepts::Allocator<ALLOCATOR_TEMPLATE<_T, _INDEX>>
		class CONTAINER {
		public:

			using INDEX = _INDEX;
			using T = _T;

			using ALLOCATOR = ALLOCATOR_TEMPLATE<T, INDEX>;

		protected:

			std::byte* data = nullptr;

		public:

#pragma region Constructors & Destructors

			CONTAINER() noexcept = default;

			CONTAINER(INDEX initialCapacity) {
				data = reinterpret_cast<std::byte*>(ALLOCATOR::allocate(initialCapacity));
			}

			~CONTAINER() noexcept {
				ALLOCATOR::deallocate(get_data());
			}

#pragma endregion

#pragma region Methods

#pragma region Assignment

			CONTAINER& operator=(CONTAINER&& other) noexcept {
				if (this == &other)
					return *this;

				ALLOCATOR::deallocate(data);

				data = std::exchange(other.get_raw_data(), nullptr);

				return *this;
			}

#pragma endregion

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

			constexpr void one(INDEX begin, INDEX count) noexcept {
				std::memset(get_raw_data() + begin, 0xFF, count * sizeof(T));
			}

			constexpr void zero(INDEX begin, INDEX count) noexcept {
				std::memset(get_raw_data() + begin, 0, count * sizeof(T));
			}

#pragma endregion

#pragma endregion

		};

	};

}