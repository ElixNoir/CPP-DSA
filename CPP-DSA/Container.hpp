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

		template <
			template <typename, typename...> typename CONTAINER,
			typename T,
			typename... ARGS
		>
		struct RebindContainer<CONTAINER<T, ARGS...>> {
			template <typename NEW_T>
			using type = CONTAINER<NEW_T, ARGS...>;
		};

		template <typename CONTAINER, typename NEW_T>
		using RebindContainer_t =
			typename RebindContainer<CONTAINER>::template type<NEW_T>;

	}

	template <typename _T, std::unsigned_integral _INDEX = size_t, typename _ALLOCATOR = StandardAllocator<_T>>
		requires concepts::Allocator<_ALLOCATOR>
	struct DynamicContainer {
	protected:

		std::byte* data = nullptr;
		_INDEX capacity = 0;

	public:

		using ALLOCATOR = _ALLOCATOR;
		using INDEX = _INDEX;
		using T = _T;

		DynamicContainer() = default;

		DynamicContainer(INDEX initialCapacity) : capacity(initialCapacity) {
			data = reinterpret_cast<std::byte*>(ALLOCATOR::allocate(initialCapacity));
		}

#pragma region Methods

#pragma region Container

#pragma region Getters

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
			return get_data()[index];
		}

		[[nodiscard]] constexpr const T& operator[](INDEX index) const noexcept {
			return get_data()[index];
		}

#pragma endregion

#pragma endregion

#pragma region Memory Management

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

#pragma region Methods

#pragma region Container

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
			return get_data()[index];
		}

		[[nodiscard]] constexpr const T& operator[](INDEX index) const noexcept {
			return get_data()[index];
		}

#pragma endregion

#pragma endregion

#pragma endregion

	};

}