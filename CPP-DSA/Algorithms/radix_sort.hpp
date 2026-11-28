#pragma once

/*template <size_t BASE = 16, std::unsigned_integral T>
constexpr static void radix_sort(T* data, size_t length) noexcept {
    // local count = table_create(base, 0)
    // local output = table_create(length, 0)
    size_t maximum = Math::maximum_in(data, length);
    size_t exponent = 1; // 0
    while (maximum / exponent > 0) {
        // table.clear(count)
        if constexpr (is_power_of_2(BASE)) {
            for (size_t index = 0; index < length; ++index)
                ++count[(data[index] >> exponent) & (base - 1)];
            for (size_t index = 1; index < base; ++index)
                count[index] += count[index - 1];
            for (size_t index = length; index > 0; --index) {
                const T digit = (data[index] >> exponent) & (base - 1);
                output[count[digit]] = data[index];
                --count[digit];
            }
            ++exponent;
        }
        else {
            for (size_t index = 0; index < length; ++index)
                ++count[(data[index] / exponent) % base];
            for (size_t index = 1; index < base; ++index)
                count[index] += count[index - 1];
            for (size_t index = length; index > 0; --index) {
                const T digit = (data[index] / exponent) % base;
                output[count[digit]] = data[index];
                --count[digit];
            }
            exponent *= BASE;
        }
        // table_move(output, 1, length, 1 list)
    }
}*/