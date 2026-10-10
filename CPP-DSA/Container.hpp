#pragma once

#include "Allocator.hpp"
#include "StandardAllocator.hpp"
#include "DSA.hpp"

namespace DSA {

	namespace concepts {

		template <typename T>
		concept Container = requires(T container) {

			typename T::T;
			typename T::INDEX;

			{ container.get_capacity() } -> std::same_as<typename T::INDEX>;
			{ container.get_data() } -> std::same_as<typename T::T*>;

		};

		template <typename T>
		struct ContainerTraits {
			constexpr static bool dynamic = false;
		};

		template <typename T>
		concept DynamicContainer = Container<T> && ContainerTraits<T>::dynamic && requires(T container) {

			typename T::ALLOCATOR;

		};

		template <typename T>
		concept RawContainer = Container<T> && requires(T container) {

			{ container.get_raw_data() } -> std::same_as<std::byte*>;

		};

		template <typename T>
		concept ResizableContainer = DynamicContainer<T> && requires(T container, size_t newCapacity) {

			{ container.resize(newCapacity) };

		};

		template <typename T>
		concept StaticContainer = Container<T> && !ContainerTraits<T>::dynamic && requires(T container) {
			
			T::capacity;

		};

	}

	namespace traits {

		template <typename CONTAINER>
		struct RebindContainer;

		template <typename T>
		struct container_replace_first_arg;

		template <template <typename, auto> typename T, typename First, auto Value>
		struct container_replace_first_arg<T<First, Value>> {
			template <typename U>
			using type = T<U, Value>;
		};

		template <template <typename...> typename T, typename First, typename... Rest>
		struct container_replace_first_arg<T<First, Rest...>> {
			template <typename U>
			using type = T<U, Rest...>;
		};

		template <typename T, typename U>
		using container_replace_first_arg_t = typename container_replace_first_arg<T>::template type<U>;

	}

	template <typename _T, std::unsigned_integral _INDEX = size_t, typename _ALLOCATOR = StandardAllocator<_T>>
		requires concepts::Allocator<_ALLOCATOR>
	struct DynamicContainer {
	protected:

		std::byte* data = nullptr;
		_INDEX capacity = 0;

#pragma region Methods

		constexpr void set_capacity(std::byte* newCapacity) noexcept {
			capacity = newCapacity;
		}

		constexpr void set_data(std::byte* newData) noexcept {
			data = newData;
		}

#pragma endregion

	public:

		using ALLOCATOR = _ALLOCATOR;
		using INDEX = _INDEX;
		using T = _T;

		DynamicContainer() = default;

		DynamicContainer(INDEX initialCapacity) : capacity(initialCapacity) {
			data = reinterpret_cast<std::byte*>(ALLOCATOR::allocate(initialCapacity));
		}

#pragma region Methods

#pragma region Getters

		constexpr T& get(INDEX index) noexcept {
			return get_data()[index];
		}

		constexpr const T& get(INDEX index) const noexcept {
			return get_data()[index];
		}

		[[nodiscard]] constexpr INDEX get_capacity() const noexcept {
			return capacity;
		}

		[[nodiscard]] constexpr T* get_data() noexcept {
			return reinterpret_cast<T*>(data);
		}

		[[nodiscard]] constexpr const T* get_data() const noexcept {
			return reinterpret_cast<const T*>(data);
		}

		[[nodiscard]] constexpr operator T* () noexcept {
			return get_data();
		}

		[[nodiscard]] constexpr operator const T* () const noexcept {
			return get_data();
		}

		[[nodiscard]] constexpr std::byte* get_raw_data() noexcept {
			return reinterpret_cast<std::byte*>(data);
		}

		[[nodiscard]] constexpr const std::byte* get_raw_data() const noexcept {
			return reinterpret_cast<const std::byte*>(data);
		}

		[[nodiscard]] constexpr operator std::byte* () noexcept {
			return get_raw_data();
		}

		[[nodiscard]] constexpr operator const std::byte* () const noexcept {
			return get_raw_data();
		}

		[[nodiscard]] constexpr T& operator[](INDEX index) noexcept {
			return get(index);
		}

		[[nodiscard]] constexpr const T& operator[](INDEX index) const noexcept {
			return get(index);
		}

#pragma endregion

#pragma region Memory Management

#pragma region Assignment

		DynamicContainer& operator=(DynamicContainer&& other) noexcept {
			if (this == &other)
				return *this;

			ALLOCATOR::deallocate(data);

			data = std::exchange(other.data, nullptr);
			capacity = std::exchange(other.capacity, 0);

			return *this;
		}

#pragma endregion

		void double_capacity() noexcept(
			concepts::NothrowResizableAllocator<ALLOCATOR>
		) requires (
			std::is_trivial_v<T>
		) {
			grow(capacity << 1);
		}

		void grow(INDEX newCapacity) noexcept(
			concepts::NothrowResizableAllocator<ALLOCATOR>
		) requires (
			std::is_trivial_v<T>
		) {
			if constexpr (concepts::ReallocatableAllocator<ALLOCATOR>)
				data = reinterpret_cast<std::byte*>(ALLOCATOR::reallocate(get_data(), newCapacity));
			else {
				T* oldData = get_data();
				data = reinterpret_cast<std::byte*>(ALLOCATOR::allocate(newCapacity));
				std::memcpy(get_data(), oldData, capacity);
				ALLOCATOR::deallocate(oldData);
			}

			capacity = newCapacity;
		}

		void reserve(INDEX newCapacity) noexcept(
			concepts::NothrowResizableAllocator<ALLOCATOR>
		) requires (
			std::is_trivial_v<T>
		) {
			if (newCapacity > capacity)
				grow(newCapacity);
		}

		void resize(INDEX newCapacity) noexcept(
			concepts::NothrowResizableAllocator<ALLOCATOR>
		) requires (
			std::is_trivial_v<T>
		) {
			if (newCapacity > capacity)
				grow(newCapacity);
			else
				shrink(newCapacity);
		}

		void shrink(INDEX newCapacity) noexcept(
			concepts::NothrowResizableAllocator<ALLOCATOR>
		) requires (
			std::is_trivial_v<T>
		) {
			if constexpr (concepts::ReallocatableAllocator<ALLOCATOR>)
				data = reinterpret_cast<std::byte*>(ALLOCATOR::reallocate(get_data(), newCapacity));
			else {
				T* oldData = get_data();
				data = reinterpret_cast<std::byte*>(ALLOCATOR::allocate(newCapacity));
				std::memcpy(get_data(), oldData, newCapacity);
				ALLOCATOR::deallocate(oldData);
			}

			capacity = newCapacity;
		}

		constexpr void one() const noexcept {
			std::memset(data, 0xFF, capacity);
		}

		constexpr void zero() const noexcept {
			std::memset(data, 0, capacity);
		}

#pragma endregion

#pragma endregion

	};

	template <typename T, typename INDEX, typename ALLOCATOR>
		requires concepts::Allocator<ALLOCATOR>
	struct concepts::ContainerTraits<DynamicContainer<T, INDEX, ALLOCATOR>> {
		constexpr static bool dynamic = true;
	};

	template <typename _T, size_t CAPACITY>
	struct alignas(_T) StaticContainer {
	protected:

		constexpr static size_t capacity = CAPACITY;

		alignas(_T) std::byte data[CAPACITY * sizeof(_T)];

	public:

		using INDEX = smallest_uint_t<CAPACITY>;
		using T = _T;

#pragma region Constructors & Destructors

		StaticContainer() = default;

#pragma endregion

#pragma region Methods

#pragma region Getters

		constexpr T& get(INDEX index) noexcept {
			return get_data()[index];
		}

		constexpr const T& get(INDEX index) const noexcept {
			return get_data()[index];
		}

		[[nodiscard]] constexpr INDEX get_capacity() const noexcept {
			return CAPACITY;
		}

		[[nodiscard]] constexpr T* get_data() noexcept {
			return reinterpret_cast<T*>(data);
		}

		[[nodiscard]] constexpr const T* get_data() const noexcept {
			return reinterpret_cast<const T*>(data);
		}

		[[nodiscard]] constexpr operator T* () noexcept {
			return get_data();
		}

		[[nodiscard]] constexpr operator const T* () const noexcept {
			return get_data();
		}

		[[nodiscard]] constexpr std::byte* get_raw_data() noexcept {
			return reinterpret_cast<std::byte*>(data);
		}

		[[nodiscard]] constexpr const std::byte* get_raw_data() const noexcept {
			return reinterpret_cast<const std::byte*>(data);
		}

		[[nodiscard]] constexpr operator std::byte* () noexcept {
			return get_raw_data();
		}

		[[nodiscard]] constexpr operator const std::byte* () const noexcept {
			return get_raw_data();
		}

		[[nodiscard]] constexpr T& operator[](INDEX index) noexcept {
			return get(index);
		}

		[[nodiscard]] constexpr const T& operator[](INDEX index) const noexcept {
			return get(index);
		}

#pragma endregion

#pragma region Memory Management

		constexpr void one() const noexcept {
			std::memset(data, 0xFF, capacity);
		}

		constexpr void zero() const noexcept {
			std::memset(data, 0, capacity);
		}

#pragma endregion

#pragma endregion

	};

}