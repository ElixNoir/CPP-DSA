#pragma once

#include "Comparable.hpp"
#include "Iterator.hpp"

namespace Algorithms {

	namespace Sorting {

        template <DSA::concepts::Comparable T, DSA::concepts::InputIterator<T> ITERATOR>
        [[nodiscard]] constexpr static bool is_sorted(ITERATOR begin, ITERATOR end) noexcept {
            if (begin == end)
                return true;

            ITERATOR previous = begin++;

            while (begin != end) {
                if (*begin < *previous)
                    return false;

                previous = begin++;
            }

            return true;
        }

	}

}