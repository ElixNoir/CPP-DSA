#pragma once

#include "Container.hpp"

#include <cstddef>
#include <cstdint>
#include <bit>
#include <limits>
#include <new>
#include <type_traits>
#include <cstring>
#include <algorithm>

namespace DSA {

	template <concepts::ContainerPolicy ContainerPolicy, size_t MINIMUM_BLOCK_SIZE = 16>
	class BuddyArena : public ContainerPolicy::template STRUCTURE<std::byte> {
	protected:

		using Base = ContainerPolicy::template STRUCTURE<std::byte>;
		using typename Base::ALLOCATOR;
		using typename Base::INDEX;

		struct Header {

			INDEX previous;
			INDEX next;

		};

		//static_assert(std::has_single_bit(ALIGNMENT));
		//static_assert(ALIGNMENT >= alignof(Header));
		//static_assert(ALIGNMENT >= alignof(INDEX));

		constexpr static INDEX invalid_index = Base::invalid_index;

		constexpr static bool is_dynamic = Base::is_dynamic;

		static_assert(std::has_single_bit(MINIMUM_BLOCK_SIZE));

		static_assert(std::has_single_bit(invalid_index));

		constexpr static uint8_t free_flag = 0x80;
		constexpr static uint8_t invalid_state = 0xFF;

		constexpr static size_t prefix_size = sizeof(Header) + sizeof(INDEX);

		static_assert(MINIMUM_BLOCK_SIZE >= prefix_size + 1, "Minimum block cannot accommodate allocation metadata");
		static_assert(MINIMUM_BLOCK_SIZE >= alignof(Header));
		static_assert(MINIMUM_BLOCK_SIZE >= alignof(INDEX));

#pragma region Properties

		constexpr static INDEX block_count = Base::maximum_capacity / MINIMUM_BLOCK_SIZE;
		constexpr static uint8_t order_count = std::countr_zero(Base::maximum_capacity) - std::countr_zero(MINIMUM_BLOCK_SIZE) + 1;

		INDEX free_lists[order_count];
		uint8_t block_states[block_count];

#pragma endregion

#pragma region Methods

#pragma region Block Utilities

		[[nodiscard]] constexpr static size_t block_size(size_t order) noexcept {
			return size_t{ 1 } << order;
		}

		[[nodiscard]] constexpr static size_t list_index(size_t order) noexcept {
			return order - get_minimum_order();
		}

		[[nodiscard]] constexpr static uint8_t encode_free(size_t order) noexcept {
			return static_cast<uint8_t>(order) | free_flag;
		}

		[[nodiscard]] constexpr static bool is_free(uint8_t state) noexcept {
			return (state & free_flag) != 0 && state != invalid_state;
		}

		[[nodiscard]] constexpr static size_t decode_order(uint8_t state) noexcept {
			return state & ~free_flag;
		}

		[[nodiscard]] constexpr static size_t block_slot(INDEX index) noexcept {
			return static_cast<size_t>(index) / MINIMUM_BLOCK_SIZE;
		}

		[[nodiscard]] constexpr Header& header_at(INDEX index) noexcept {
			return *std::launder(reinterpret_cast<Header*>(Base::get_data() + index));
		}

		[[nodiscard]] constexpr const Header& header_at(INDEX index) const noexcept {
			return *std::launder(reinterpret_cast<const Header*>(Base::get_data() + index));
		}

#pragma endregion

#pragma region Free Lists

		void insert_free(INDEX index, size_t order) noexcept {
			Header& header = header_at(index);

			INDEX& head = free_lists[list_index(order)];

			header.previous = invalid_index;
			header.next = head;

			if (head != invalid_index)
				header_at(head).previous = index;

			head = index;

			block_states[block_slot(index)] = encode_free(order);
		}

		void remove_free(INDEX index, size_t order) noexcept {
			Header& header = header_at(index);

			INDEX& head = free_lists[list_index(order)];

			if (header.previous != invalid_index)
				header_at(header.previous).next = header.next;
			else
				head = header.next;

			if (header.next != invalid_index)
				header_at(header.next).previous = header.previous;

			header.previous = invalid_index;
			header.next = invalid_index;
		}

#pragma endregion

#pragma region Metadata

		[[nodiscard]] constexpr INDEX get_header_index(INDEX payload_index) const noexcept {
			const INDEX capacity = Base::get_capacity();
			if (payload_index < sizeof(INDEX) || payload_index >= capacity)
				return invalid_index;

			INDEX header_index;

			std::memcpy(&header_index, Base::get_data() + payload_index - sizeof(INDEX), sizeof(INDEX));

			if (header_index >= capacity || header_index % MINIMUM_BLOCK_SIZE != 0)
				return invalid_index;

			const uint8_t state = block_states[block_slot(header_index)];

			if (state == invalid_state || is_free(state))
				return invalid_index;

			const size_t order = decode_order(state);
			const size_t size = block_size(order);

			if (header_index > capacity - size)
				return invalid_index;

			if (payload_index < header_index + prefix_size || payload_index >= static_cast<size_t>(header_index) + size)
				return invalid_index;

			// Confirm that this is the actual payload index.
			if (header_at(header_index).next != payload_index)
				return invalid_index;

			return header_index;
		}

#pragma endregion

#pragma endregion

	public:

#pragma region Constructors & Destructors

		BuddyArena() noexcept {
			for (size_t i = 0; i < order_count; ++i)
				free_lists[i] = invalid_index;
			//Base::zero(order_count);

			const INDEX block_count = block_count;
			for (size_t i = 0; i < block_count; ++i)
				block_states[i] = invalid_state;

			::new (Base::get_data()) Header{ invalid_index, invalid_index };

			insert_free(0, get_maximum_order());
		}

		BuddyArena(const BuddyArena&) = delete;
		BuddyArena& operator=(const BuddyArena&) = delete;

		BuddyArena(BuddyArena&&) = delete;
		BuddyArena& operator=(BuddyArena&&) = delete;

#pragma endregion

#pragma region Methods

#pragma region Allocate & Deallocate

		[[nodiscard]] constexpr size_t get_allocated_block_size(INDEX payload_index) const noexcept {
			const INDEX header_index = get_header_index(payload_index);
			if (header_index == invalid_index)
				return 0;

			return block_size(decode_order(block_states[block_slot(header_index)]));
		}

		template <typename T>
		[[nodiscard]] constexpr bool could_allocate_with_alignment() const noexcept {
			return could_allocate_with_alignment(alignof(T));
		}

		[[nodiscard]] constexpr bool could_allocate_with_alignment(size_t alignment) const noexcept {
			return alignment <= Base::get_capacity(); // && std::has_single_bit(alignment);
		}

		template <typename T>
		[[nodiscard]] constexpr INDEX allocate() noexcept {
			return allocate(sizeof(T), alignof(T));
		}

		template <typename T>
		[[nodiscard]] constexpr INDEX allocate(size_t count, size_t alignment = alignof(T)) noexcept {
			const INDEX capacity = Base::get_capacity();
			if (!std::has_single_bit(alignment))
				return invalid_index;

			const size_t effective_alignment = alignment > alignof(INDEX) ? alignment : alignof(INDEX);

			const size_t payload_size = count * sizeof(T); // do not handle the case where count is 0
			const size_t padding = effective_alignment - 1;

			if (capacity <= prefix_size + padding)
				return invalid_index;
			if (payload_size > capacity - prefix_size - padding)
				return invalid_index;

			const INDEX maximum_order = get_maximum_order();
			const INDEX minimum_order = get_minimum_order();

			size_t order = std::bit_width(prefix_size + padding + payload_size - 1);
			if (order < minimum_order)
				order = minimum_order;
			if (order > maximum_order)
				return invalid_index;

			// Find the first available block.
			size_t available_order = order;
			while (available_order <= maximum_order && free_lists[list_index(available_order)] == invalid_index)
				++available_order;
			if (available_order > maximum_order)
				return invalid_index;

			const INDEX block_index = free_lists[list_index(available_order)];

			remove_free(block_index, available_order);

			INDEX current_index = block_index;

			std::byte* const data = Base::get_data();
			while (available_order > order) { // split larger blocks
				--available_order;

				block_states[block_slot(current_index)] = invalid_state; // the parent is no longer the beginning of a block

				const INDEX right_index = static_cast<INDEX>(current_index + block_size(available_order));
				// Construct the right child's header.
				::new (data + right_index) Header{
					invalid_index,
					invalid_index
				};

				insert_free(right_index, available_order);

				block_states[block_slot(current_index)] = encode_free(available_order);
			}

			// determine payload position

			const size_t start = static_cast<size_t>(current_index) + prefix_size;
			const INDEX payload_index = static_cast<INDEX>(start + (size_t{ 0 } - start) & (effective_alignment - 1));

			// Store the block's starting index immediately before
			// the payload.
			std::memcpy(data + payload_index - sizeof(INDEX), &current_index, sizeof(INDEX));

			// While allocated, Header::next identifies the payload.
			header_at(current_index).previous = invalid_index;
			header_at(current_index).next = payload_index;

			block_states[block_slot(current_index)] = static_cast<uint8_t>(order);

			return payload_index;
		}

		constexpr void deallocate(INDEX payload_index) noexcept {
			const INDEX header_index = get_header_index(payload_index);
			if (header_index == invalid_index)
				return;

			size_t order = decode_order(block_states[block_slot(header_index)]);
			INDEX current_index = header_index;

			const INDEX maximum_order = get_maximum_order();
			while (order < maximum_order) { // coallesce buddies
				const INDEX buddy_index = static_cast<INDEX>(current_index ^ block_size(order));

				if (block_states[block_slot(buddy_index)] != encode_free(order))
					break;

				remove_free(buddy_index, order);

				block_states[block_slot(current_index)] = invalid_state;
				block_states[block_slot(buddy_index)] = invalid_state;

				if (buddy_index < current_index)
					current_index = buddy_index;

				++order;
			}

			insert_free(current_index, order);
		}

#pragma endregion

#pragma region Checks

		[[nodiscard]] constexpr bool owns(INDEX index) const noexcept {
			return index < Base::get_capacity();
		}

		[[nodiscard]] constexpr bool owns(const void* pointer) const noexcept {
			const std::byte* const data = Base::get_data();
			const auto* const byte_ptr = static_cast<const std::byte*>(pointer);
			return std::less_equal<const std::byte*>{}(data, byte_ptr) && std::less<const std::byte*>{}(byte_ptr, data + Base::get_capacity());
		}

#pragma endregion

#pragma region Getters

		[[nodiscard]] constexpr INDEX get_maximum_order() const noexcept {
			return std::countr_zero(Base::get_capacity());
		}

		[[nodiscard]] constexpr INDEX get_minimum_order() const noexcept {
			return std::countr_zero(MINIMUM_BLOCK_SIZE);
		}

#pragma region Conversion

		template <typename T = void>
		[[nodiscard]] constexpr T* get_pointer(INDEX index) noexcept {
			return static_cast<T*>(Base::get_data() + index);
		}

		template <typename T = void>
		[[nodiscard]] constexpr const T* get_pointer(INDEX index) const noexcept {
			return static_cast<const T*>(Base::get_data() + index);
		}

		[[nodiscard]] constexpr INDEX get_index(const void* pointer) const noexcept {
			return static_cast<INDEX>(static_cast<const std::byte*>(pointer) - Base::get_data());
		}

#pragma endregion

#pragma endregion

#pragma endregion

	};

}