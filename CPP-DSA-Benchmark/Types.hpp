#pragma once

#include <array>
#include <cstdint>
#include <cstddef>
#include <chrono>
#include <type_traits>

struct LargeTrivial {
    std::uint64_t data[8];
};

struct NonTrivial {
    std::uint64_t data[8];

    NonTrivial() noexcept {
        const auto seed = static_cast<std::uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
        for (std::size_t i = 0; i < 8; ++i)
            data[i] = seed + i;
    };

    NonTrivial(const NonTrivial& other) noexcept
        : data{}
    {
        for (std::size_t i = 0; i < 8; ++i)
            data[i] = other.data[i];
    }

    NonTrivial(NonTrivial&& other) noexcept
        : data{}
    {
        for (std::size_t i = 0; i < 8; ++i)
            data[i] = other.data[i];
    }

    ~NonTrivial() noexcept {}
};

static_assert(std::is_trivially_copyable_v<LargeTrivial>);
static_assert(!std::is_trivially_destructible_v<NonTrivial>);
static_assert(std::is_nothrow_move_constructible_v<NonTrivial>);