#pragma once

#include "Memory.hpp"

namespace Algorithms {

	namespace Sorting {

		enum Comparison : uint8_t {
			
			EQUAL,
			NOT_EQUAL,

			GREATER_THAN,
			GREATER_THAN_OR_EQUAL,

			LESS_THAN,
			LESS_THAN_OR_EQUAL

		};

		template <bool ASCENDING = true>
		constexpr static bool compare(const auto& a, const auto& b) {
			if constexpr (ASCENDING)
				return a < b;
			else
				return a > b;
		}

		template <Comparison COMPARISON>
		constexpr static bool compare(const auto& a, const auto& b) {
			if constexpr (COMPARISON == Comparison::EQUAL)
				return a == b;
			else if constexpr (COMPARISON == Comparison::NOT_EQUAL)
				return a != b;
			else if constexpr (COMPARISON == Comparison::GREATER_THAN)
				return a > b;
			else if constexpr (COMPARISON == Comparison::GREATER_THAN_OR_EQUAL)
				return a >= b;
			else if constexpr (COMPARISON == Comparison::LESS_THAN)
				return a < b;
			else if constexpr (COMPARISON == Comparison::LESS_THAN_OR_EQUAL)
				return a <= b;
			
		}

		constexpr static bool is_sorted(const auto* const begin, const auto* const end) {
			if (begin == end)
				return true;
			const auto* cursor = begin;
			while (++cursor != end) {
				if (*cursor < *(cursor - 1))
					return false;
			}
			return true;
		}

		template <typename T>
		constexpr static bool greater_than(const T& a, const T& b) {
			return a > b;
		}

		template <typename T>
		constexpr static bool less_than(const T& a, const T& b) {
			return a < b;
		}

		template <bool ASCENDING = true, typename T>
		constexpr static void sort4(T* const a) {
			if constexpr (ASCENDING) {
				Memory::swap_if<greater_than<T>>(a[0], a[1]);
				Memory::swap_if<greater_than<T>>(a[2], a[3]);
				Memory::swap_if<greater_than<T>>(a[0], a[2]);
				Memory::swap_if<greater_than<T>>(a[1], a[3]);
				Memory::swap_if<greater_than<T>>(a[1], a[2]);
			}
			else {
				Memory::swap_if<less_than<T>>(a[0], a[1]);
				Memory::swap_if<less_than<T>>(a[2], a[3]);
				Memory::swap_if<less_than<T>>(a[0], a[2]);
				Memory::swap_if<less_than<T>>(a[1], a[3]);
				Memory::swap_if<less_than<T>>(a[1], a[2]);
			}
		}

	}

}