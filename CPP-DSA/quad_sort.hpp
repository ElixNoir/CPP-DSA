#pragma once

#include "compare.hpp"
#include "swap.hpp"

namespace Algorithms {

    template <bool ASCENDING = true, typename T>
    constexpr static void sort4(T* const data) {
        constexpr auto c = compare<ASCENDING, T>;

        swap_if<c>(data[0], data[1]);
        swap_if<c>(data[2], data[3]);
        swap_if<c>(data[0], data[2]);
        swap_if<c>(data[1], data[3]);
        swap_if<c>(data[1], data[2]);
    }

}