#pragma once

#include "Memory.hpp"

namespace Algorithms {

	namespace Sorting {

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