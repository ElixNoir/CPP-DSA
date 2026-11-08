#pragma once

#include "binary_search.hpp"
#include "compare.hpp"
#include "rotate.hpp"

namespace Algorithms {

    namespace Sorting {

        struct Layout {
            size_t block_length;
            size_t key_count;
            size_t required_keys;
        };

        template <bool ASCENDING, DSA::concepts::Comparable T>
        void build_blocks(T* data, size_t length, size_t K) {
            // K must be a power of two.
            // data[-K ... -1] contains the internal buffer.

            if (!length || K < 2)
                return;

            // Build sorted pairs, using the reserved buffer.

            for (size_t m = 1; m < length; m += 2) {
                const size_t u = compare<ASCENDING>(data[m], data[m - 1]);
                swap(data + m - 3, data + m - 1 + u);
                swap(data + m - 2, data + m - u);
            }

            if (length & 1)
                swap(data + length - 1, data + length - 3);

            data -= 2;

            size_t h = 2;

            // Merge pairs into progressively larger runs.

            for (; h < K; h <<= 1) {
                size_t p0 = 0;
                const size_t p1 = length - (h << 1);

                while (p0 <= p1) {
                    merge<Direction::LEFT>(data + p0, h, h, -h);
                    p0 += h << 1;
                }

                const size_t rest = length - p0;

                if (rest > h)
                    merge<Direction::LEFT>(data + p0, h, rest - h, -h);
                else {
                    for (; p0 < length; ++p0)
                        data[p0 - h] = data[p0];
                }

                data -= h;
            }

            // Align the final incomplete block.

            const size_t rest = length & ((K << 1) - 1);

            size_t p = length - rest;

            if (rest <= K)
                rotate(data + p, rest, K);
            else
                merge<Direction::RIGHT>(data + p, K, rest - K, K);

            while (p) {
                p -= K << 1;
                merge<Direction::RIGHT>(data + p, K, K, K);
            }
        }


        constexpr Layout calculate_layout(size_t length) noexcept {
            if (length < 16)
                return {};

            size_t block_length = 1;

            // Find the smallest power of two satisfying
            // block_length * block_length >= length.
            //
            // Avoid multiplication to prevent overflow.

            while (block_length < length / block_length || (block_length == length / block_length && length % block_length != 0))
                block_length <<= 1;

            const size_t key_count = (length - 1) / block_length + 1;

            return {
                block_length,
                key_count,
                block_length + key_count
            };
        }

        template <bool ASCENDING = true, DSA::concepts::Comparable T>
        void combine_blocks(
            T* keys,
            T* data,
            size_t length,
            size_t run_length,
            size_t block_length
        ) noexcept {
            const size_t full_runs = length / (run_length << 1);

            size_t remainder = length % (run_length << 1);
            if (remainder <= run_length) {
                length -= remainder;
                remainder = 0;
            }

            for (size_t b = 0; b <= full_runs; ++b) {
                if (b == full_runs && !remainder)
                    break;

                T* current = data + b * (run_length << 1);

                const size_t current_length = b == full_runs ? remainder : (run_length << 1);
                const size_t blocks = current_length / block_length;
                const size_t middle = run_length / block_length;

                // Include the incomplete final block in key sorting.
                sort_ins(
                    keys,
                    blocks + (b == full_runs)
                );

                sort_blocks<ASCENDING>(keys, current, blocks, block_length, middle);

                size_t second_blocks = 0;
                size_t last_length = 0;

                if (b == full_runs)
                    last_length = remainder % block_length;

                if (last_length)
                    while (second_blocks < blocks && compare<ASCENDING>(current[blocks * block_length], current[(blocks - second_blocks - 1) * block_length]))
                        ++second_blocks;

                merge_buffers_left<ASCENDING>(keys, keys + middle, current, blocks - second_blocks, block_length, second_blocks, last_length);
            }

            // Return the internal buffer to its original position.
            for (size_t i = length; i-- > 0;)
                swap(data + i, data + i - block_length);
        }

        template <DSA::concepts::Comparable T>
        Layout prepare(T* data, size_t length) noexcept {
            const Layout layout =
                calculate_layout(length);

            if (length < 16)
                return layout;

            sort_keys(
                data,
                layout.required_keys
            );

            return layout;
        }

		enum Direction : uint8_t {

			LEFT,
			RIGHT

		};

        template <Direction DIRECTION, bool ASCENDING = true, DSA::concepts::Comparable T>
        constexpr static void merge(T* data, size_t L1, size_t L2, size_t M) {
            size_t p0 = L1 + L2 + M - 1;
			size_t p1 = L1 - 1;
			size_t p2 = L1 + L2 - 1;

            if constexpr (DIRECTION == Direction::LEFT) {
                size_t p0 = 0;
                size_t p1 = L1;

                const size_t end = L1 + L2;

                while (p1 < end) {
                    if (p0 == L1 || compare<ASCENDING>(data[p1], data[p0]))
                        swap(data + M++, data + p1++);
                    else
                        swap(data + M++, data + p0++);
                }

                if (M != p0)
                    swap_many(data + M, data + p0, L1 - p0);
            }
            else {
                while (p1 >= 0) {
                    if (p2 < L1 || compare<ASCENDING>(data[p2], data[p1]))
                        swap(data + p0--, data + p1--);
                    else
                        swap(data + p0--, data + p2--);
                }

                if (p2 != p0)
                    while (p2 >= L1)
                        swap(data + p0--, data + p2--);
            }
        }

        template <bool ASCENDING = true, DSA::concepts::Comparable T>
        constexpr static void merge_without_buffer(T* data, size_t left, size_t right) noexcept {
            if (left < right) {
                while (left) {
                    const size_t offset = Search::binary_search<Search::BinarySearchType::LEFT>(data + left, right, data[0]);
                    if (offset) {
                        rotate(data, left, offset);
                        data += offset;
                        right -= offset;
                    }

                    if (!right)
                        return;

                    do {
                        ++data;
                        --left;
                    } while (left && (compare<ASCENDING>(data + 0, data[left])));
                }
            }
            else {
                while (right) {
                    const size_t offset = Search::binary_search<Search::BinarySearchType::RIGHT>(data, left, data[left + right - 1]);
                    if (offset != left) {
                        rotate(data + offset, left - offset, right);
                        left = offset;
                    }

                    if (!left)
                        return;

                    do {
                        --right;
                    } while (right && (compare<ASCENDING>(data[left - 1], data[left + right - 1])));
                }
            }
        }

        template <bool ASCENDING = true, DSA::concepts::Comparable T>
        void smart_merge_with_buffer(
            T* data,
            size_t& len1,
            bool& type,
            size_t len2,
            size_t buffer_length
        ) noexcept {
            size_t p0 = -buffer_length;
            size_t p1 = 0;
            size_t p2 = len1;

            const size_t q1 = len1;
            const size_t q2 = len1 + len2;

            const bool next_type = !type;

            while (p1 < q1 && p2 < q2) {
                const bool take_left = next_type
                    ? !compare<ASCENDING>(data[p2], data[p1])
                    : compare<ASCENDING>(data[p1], data[p2]); // ??

                if (take_left)
                    swap(data + p0++, data + p1++);
                else
                    swap(data + p0++, data + p2++);
            }

            if (p1 < q1) {
                len1 = q1 - p1;
                while (p1 < q1)
                    swap(data + --q2, data + --q1);
            }
            else {
                len1 = q2 - p2;
                type = next_type;
            }
        }

        template <DSA::concepts::Comparable T>
        void merge_buffers_left(
            T* keys,
            T* midkey,
            T* data,
            size_t block_count,
            size_t block_length,
            size_t second_stream_blocks,
            size_t last_block_length
        ) noexcept {
            if (!block_count) {
                merge<Direction::LEFT, ASCENDING>(data, second_stream_blocks * block_length, last_block_length, -block_length);
                return;
            }

            size_t remaining = block_length;

            bool stream = compare<ASCENDING>(*keys, *midkey);

            size_t position = block_length;

            for (size_t T = 1; T < block_count; ++T) {
                size_t start = position - remaining;

                const bool next_stream = compare<ASCENDING>(keys[T], *midkey);

                if (next_stream == stream) {
                    swap_many(
                        data + start - block_length,
                        data + start,
                        remaining
                    );

                    start = position;
                    remaining = block_length;
                }
                else
                    smart_merge_with_buffer<ASCENDING>(data + start, remaining, stream, block_length, block_length);

                position += block_length;
            }

            size_t start = position - remaining;

            if (last_block_length) {
                if (stream) {
                    swap_many(data + start - block_length, data + start, remaining);

                    start = position;
                    remaining = block_length * second_stream_blocks;

                    stream = false;
                }
                else
                    remaining += block_length * second_stream_blocks;

                merge_left(data + start, remaining, last_block_length, -block_length);
            }
            else {
                swap_many(
                    data + start,
                    data + start - block_length,
                    remaining
                );
            }
        }

        template <bool ASCENDING = true, DSA::concepts::Comparable T>
        void sort_blocks(
            T* keys,
            T* data,
            size_t count,
            size_t block_length,
            size_t& middle
        ) noexcept {
            for (size_t i = 1; i < count; ++i) {
                size_t smallest = i - 1;

                for (size_t j = i; j < count; ++j) {
                    if (compare<ASCENDING>(data[j * block_length], data[smallest * block_length])
                        || (!compare<ASCENDING>(data[smallest * block_length], data[j * block_length]) && compare<ASCENDING>(keys[j], keys[smallest])))
                        smallest = j;
                }

                if (smallest != i - 1) {
                    swap_many(data + (i - 1) * block_length, data + smallest * block_length, block_length);
                    swap(keys + i - 1, keys + smallest);
                    if (middle == i - 1 || middle == smallest)
                        middle ^= (i - 1) ^ smallest;
                }
            }
        }

    }

}