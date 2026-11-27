#pragma once

#include "Traits.hpp"

#include <array>
#include <tuple>
#include <utility>

namespace DSA {

	template <std::unsigned_integral REPRESENTATION, size_t... FIELD_WIDTHS>
		requires ((sizeof...(FIELD_WIDTHS) > 0)
			&& ((FIELD_WIDTHS > 0) && ...)
			&& ((FIELD_WIDTHS + ...) <= sizeof(REPRESENTATION) * 8))
	struct Bitfield {

		static constexpr size_t field_count = sizeof...(FIELD_WIDTHS);
		static constexpr size_t total_bits = sizeof(REPRESENTATION) * 8;
		static constexpr size_t used_bits = (FIELD_WIDTHS + ...);

#pragma region Properties

		REPRESENTATION raw;

#pragma endregion

#pragma region Constructors & Destructors

		constexpr Bitfield() noexcept = default;
		constexpr explicit Bitfield(REPRESENTATION newRaw) noexcept : raw(newRaw) {}

		template <typename... ARGUMENTS>
			requires (sizeof...(ARGUMENTS) == field_count
		&& (std::convertible_to<ARGUMENTS, smallest_uint_t<FIELD_WIDTHS>> && ...))
			constexpr Bitfield(ARGUMENTS... arguments) noexcept {
			[&] <size_t... IS>(std::index_sequence<IS...>) {
				raw = (((static_cast<REPRESENTATION>(arguments) & mask_of<IS>()) << offset_of<IS>()) | ...);
			}(std::make_index_sequence<field_count>{});
		}

#pragma endregion

#pragma region Methods

#pragma region Getters

		template <size_t INDEX>
			requires (INDEX < field_count)
		static consteval REPRESENTATION mask_of() noexcept {
			constexpr size_t width = width_of<INDEX>();
			if constexpr (width == total_bits)
				return ~REPRESENTATION{ 0 };
			else
				return (REPRESENTATION{ 1 } << width) - 1;
		}

		template <size_t INDEX>
		static consteval size_t offset_of() noexcept {
			constexpr std::array<size_t, field_count> widths{ FIELD_WIDTHS... };
			size_t sum = 0;
			for (size_t index = 0; index < INDEX; ++index)
				sum += widths[index];

			return sum;
		}

		template <size_t INDEX>
		static consteval size_t width_of() noexcept {
			return std::get<INDEX>(std::tuple{ FIELD_WIDTHS... });
		}

		template <size_t INDEX>
			requires (INDEX < field_count)
		[[nodiscard]] constexpr auto get() const noexcept {
			return static_cast<smallest_uint_t<width_of<INDEX>()>>((raw >> offset_of<INDEX>()) & mask_of<INDEX>());
		}

#pragma endregion

#pragma region Setters

		template <size_t INDEX>
			requires (INDEX < field_count)
		constexpr void set(smallest_uint_t<width_of<INDEX>()> value) noexcept {
			constexpr REPRESENTATION shifted_mask = mask_of<INDEX>() << offset_of<INDEX>();
			const REPRESENTATION clean_bits = (static_cast<REPRESENTATION>(value) & mask_of<INDEX>()) << offset_of<INDEX>();
			raw = static_cast<REPRESENTATION>((raw & ~shifted_mask) | clean_bits);
		}

		friend constexpr auto operator<=>(const Bitfield&, const Bitfield&) noexcept = default;
		friend constexpr bool operator==(const Bitfield&, const Bitfield&) noexcept = default;

#pragma endregion

#pragma endregion

	};

	//static_assert(sizeof(Bitfield<8, 12, 20, 32>) == sizeof(uint64_t));
	//static_assert(alignof(Bitfield<8, 12, 20, 32>) == alignof(uint64_t));

}