#pragma once

#include "Memory.hpp"

namespace Algorithms {

	namespace Sorting {

		constexpr static bool is_sorted(const auto* const begin, const auto* const end) {
			if (begin == end)
				return true;
			const auto* cursor = begin;
			while (++cursor != end)
				if (*cursor < *(cursor - 1))
					return false;
			return true;
		}

	}

}