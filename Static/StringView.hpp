#pragma once

#include "Hashes.hpp"

#include <cstdint>
#include <string_view>

struct String : public std::string_view {

    using Base = std::string_view;

#pragma region Properties

#pragma endregion

    constexpr String() noexcept = default;

    constexpr String(const char* data) : Base(data) {}

#pragma region Methods

#pragma region Hashing

    [[nodiscard]] constexpr uintmax_t hash(uintmax_t hash = FNV1aOffset) const noexcept {
        const size_t total_size = size();
        if consteval {
            const char* const base_ptr = data();

            for (size_t index = 0; index < total_size; index += sizeof(uintmax_t)) {
                uintmax_t word = 0;
                const size_t remaining = total_size - index;
                const size_t loop_count = (remaining < sizeof(uintmax_t)) ? remaining : sizeof(uintmax_t);

                for (size_t i = 0; i < loop_count; i++)
                    word |= static_cast<uintmax_t>(static_cast<uint8_t>(base_ptr[index + i])) << (i * 8);

                hash ^= word;
                hash *= FNV1aPrime;
            }
            return hash;
        }
        else {
            uintmax_t index = 0;
            if (total_size >= sizeof(uintmax_t)) {
                const size_t length = total_size - sizeof(uintmax_t);
                for (; index < length; index += sizeof(uintmax_t)) {
                    hash ^= *reinterpret_cast<const uintmax_t*>(data() + index);
                    hash *= FNV1aPrime;
                }
            }

            const size_t remaining = total_size - index;

            if (remaining == 0) return hash;

            hash ^= *reinterpret_cast<const uintmax_t*>(data()) << remaining;
            hash *= FNV1aPrime;

            return hash;
        }
    }

#pragma endregion

#pragma endregion

};
