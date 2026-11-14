#pragma once

#include <cstddef>
#include <cstdint>
#include <bit>
#include <limits>
#include <new>
#include <type_traits>

template <size_t TOTAL_SIZE, size_t MIN_BLOCK_SIZE = 16>
class BuddyArena {
protected:

	static_assert(TOTAL_SIZE != 0);
	static_assert(MIN_BLOCK_SIZE != 0);

	static_assert(std::has_single_bit(TOTAL_SIZE));
	static_assert(std::has_single_bit(MIN_BLOCK_SIZE));
	static_assert(MIN_BLOCK_SIZE <= TOTAL_SIZE);

	static constexpr size_t MAX_ORDER = std::countr_zero(TOTAL_SIZE);
	static constexpr size_t MIN_ORDER = std::countr_zero(MIN_BLOCK_SIZE);

	static constexpr size_t ORDER_COUNT = MAX_ORDER - MIN_ORDER + 1;

	struct Header {

		Header* previous;
		Header* next;
		size_t order;
		bool free;

	};

	alignas(TOTAL_SIZE) std::byte data[TOTAL_SIZE];

	Header* free_lists[ORDER_COUNT]{};

	[[nodiscard]] constexpr static size_t block_size(size_t order) noexcept {
		return size_t{ 1 } << order;
	}

	[[nodiscard]] constexpr size_t list_index(size_t order) const noexcept {
		return order - MIN_ORDER;
	}

	[[nodiscard]] Header* header_at(size_t offset) noexcept {
		return std::launder(reinterpret_cast<Header*>(data + offset));
	}

	[[nodiscard]] size_t offset_of(const Header* header) const noexcept {
		return reinterpret_cast<const std::byte*>(header) - data;
	}

	void insert_free(Header* header) noexcept {
		Header*& head = free_lists[list_index(header->order)];

		header->previous = nullptr;
		header->next = head;
		header->free = true;

		if (head)
			head->previous = header;

		head = header;
	}

	void remove_free(Header* header) noexcept {
		Header*& head = free_lists[list_index(header->order)];

		if (header->previous)
			header->previous->next = header->next;
		else
			head = header->next;

		if (header->next)
			header->next->previous = header->previous;

		header->previous = nullptr;
		header->next = nullptr;
		header->free = false;
	}

	[[nodiscard]] static constexpr bool valid_alignment(size_t alignment) noexcept {
		return alignment != 0 && std::has_single_bit(alignment);
	}

	[[nodiscard]] static constexpr size_t
		align_up(size_t value, size_t alignment) noexcept {
		return (value + alignment - 1) & ~(alignment - 1);
	}

public:

	BuddyArena() noexcept {
		Header* initial = ::new (data) Header{
			nullptr,
			nullptr,
			MAX_ORDER,
			true
		};

		insert_free(initial);
	}

	BuddyArena(const BuddyArena&) = delete;
	BuddyArena& operator=(const BuddyArena&) = delete;

	BuddyArena(BuddyArena&&) = delete;
	BuddyArena& operator=(BuddyArena&&) = delete;

#pragma region Methods

#pragma region Getters

	[[nodiscard]] static constexpr size_t get_capacity() noexcept {
		return TOTAL_SIZE;
	}

	[[nodiscard]] static constexpr size_t get_minimum_block_size() noexcept {
		return MIN_BLOCK_SIZE;
	}

#pragma endregion

	[[nodiscard]] void* allocate(size_t bytes, size_t alignment = alignof(std::max_align_t)) noexcept {
		if (!valid_alignment(alignment) || alignment > TOTAL_SIZE)
			return nullptr;

		constexpr size_t PREFIX = sizeof(Header) + sizeof(Header*);

		// Account for alignment padding and allocation metadata.
		if (bytes > TOTAL_SIZE - PREFIX)
			return nullptr;

		const size_t required = PREFIX + bytes + alignment - 1;
		if (required < PREFIX)
			return nullptr;

		size_t order = std::max(MIN_ORDER, static_cast<size_t>(std::bit_width(required - 1)));
		if (order > MAX_ORDER)
			return nullptr;

		// Find the smallest available block large enough.
		size_t available_order = order;

		while (available_order <= MAX_ORDER && free_lists[list_index(available_order)] == nullptr)
			++available_order;

		if (available_order > MAX_ORDER)
			return nullptr;

		Header* header = free_lists[list_index(available_order)];
		remove_free(header);

		// Split until the requested block size is reached.
		while (available_order > order) {
			--available_order;

			const size_t half_size = block_size(available_order);
			const size_t right_offset = offset_of(header) + half_size;

			Header* right = ::new (data + right_offset) Header{
				nullptr,
				nullptr,
				available_order,
				true
			};

			header->order = available_order;

			insert_free(right);
		}

		header->free = false;

		const std::size_t effective_alignment = std::max(alignment, alignof(Header*));
		const std::size_t payload_offset = align_up(header_offset + PREFIX, effective_alignment);
		auto* payload = arena + payload_offset;

		std::memcpy(payload - sizeof(Header*), &header, sizeof(header));

		return payload;
	}

	template <typename T>
	[[nodiscard]] void* allocate() noexcept {
		return allocate(sizeof(T), alignof(T));
	}

	void deallocate(void* pointer) noexcept {
		if (!pointer)
			return;

		auto* payload = static_cast<std::byte*>(pointer);

		Header* header = reinterpret_cast<Header**>(payload)[-1];

		// Coalesce with free buddies whenever possible.
		while (header->order < MAX_ORDER) {
			const size_t size = block_size(header->order);
			const size_t offset = offset_of(header);
			const size_t buddy_offset = offset ^ size;

			Header* buddy = header_at(buddy_offset);

			if (!buddy->free ||
				buddy->order != header->order)
				break;

			remove_free(buddy);

			// The lower-addressed block becomes the new parent.
			if (buddy_offset < offset)
				header = buddy;

			++header->order;
		}

		insert_free(header);
	}

	[[nodiscard]] bool owns(const void* pointer) const noexcept {
		if (!pointer)
			return false;
		const auto* address = static_cast<const std::byte*>(pointer);
		return address >= data && address < data + TOTAL_SIZE;
	}

#pragma endregion

};
