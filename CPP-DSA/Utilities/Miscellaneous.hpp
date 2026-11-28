#pragma once

#include <array>
#include <utility>

template <std::size_t... INDICES, typename OPERATION>
constexpr static auto generate_sequence(std::index_sequence<INDICES...>, OPERATION&& operation) {
    return std::array{ operation.template operator()<INDICES>()... };
}

template <std::size_t CAPACITY, typename OPERATION>
constexpr static auto generate_monotonic_sequence(OPERATION&& operation) {
    return generate_sequence(
        std::make_index_sequence<CAPACITY>{},
        operation
    );
}