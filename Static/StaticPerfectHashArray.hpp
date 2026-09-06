#pragma once

#pragma region Dependencies

#include "DSAConcepts.hpp"
#include "Hashes.hpp"
#include "StaticAoSContainer.hpp"
#include "StaticBitArray.hpp"

#include <initializer_list>

#pragma endregion

template <Hashable K, typename V>
struct KeyValuePair {

    K Key;
    V Value;

};

template <Hashable K>
struct KeyValuePair<K, void> {

    K Key;

};

template <size_t KeyCount, Hashable Key, typename Value = void, size_t Capacity = static_cast<size_t>(KeyCount * 1.5)>
class StaticPerfectHashArray : public StaticAoSContainer<KeyValuePair<Key, Value>, Capacity> {

    static_assert(KeyCount <= Capacity);

public:

    using Base = StaticAoSContainer<KeyValuePair<Key, Value>, Capacity>;

    using Entry = KeyValuePair<Key, Value>;

protected:

    using Base::Data;

    constexpr static size_t BitmapCapacity = (Capacity + Bitmask<uintmax_t>::Bits - 1) >> std::bit_width(Bitmask<uintmax_t>::Bits - 1);

#pragma region Properties

    size_t Seed = FNV1aOffset;

#pragma endregion

public:

    constexpr StaticPerfectHashArray(const std::initializer_list<Entry> list) {
        while (true) {
            StaticBitArray<BitmapCapacity> visited{};
            bool collision = false;

            for (const Entry& entry : list) {
                const size_t index = entry.Key.hash(Seed) % Capacity;

                Bitmask<uintmax_t>& bitmask = visited.bitmask_at(index);

                if (bitmask.get(index)) {
                    collision = true;
                    break;
                }

                bitmask.set(index);
            }

            if (!collision)
                break;

            Seed++;
        }

        for (const Entry& entry : list)
            Data[entry.Key.hash(Seed) % Capacity] = entry;
    }

    constexpr StaticPerfectHashArray(const std::initializer_list<Key> list) requires std::is_void_v<Value> {
        while (true) {
            StaticBitArray<BitmapCapacity> visited{};
            bool collision = false;

            for (const Key& key : list) {
                const size_t index = key.hash(Seed) % Capacity;

                Bitmask<uintmax_t>& bitmask = visited.bitmask_at(index);

                if (bitmask.get(index)) {
                    collision = true;
                    break;
                }

                bitmask.set(index);
            }

            if (!collision)
                break;

            Seed++;
        }

        for (const Key& key : list)
            Data[key.hash(Seed) % Capacity].Key = key;
    }

#pragma region Methods

#pragma region Getters

    [[nodiscard]] constexpr size_t seed() const noexcept {
        return Seed;
    }

#pragma endregion

#pragma region Access

    [[nodiscard]] constexpr bool contains(Key& key) const noexcept {
        return key == Data[key.hash(Seed) % Capacity].Key;
    }

    [[nodiscard]] constexpr Entry* find(Key& key) noexcept {
        Entry* entry = &Data[key.hash(Seed) % Capacity];
        return key == entry->Key ? entry : nullptr;
    }

    [[nodiscard]] constexpr const Entry* find(Key key) const noexcept {
        const Entry* entry = &Data[key.hash(Seed) % Capacity];
        return key == entry->Key ? entry : nullptr;
    }

#pragma endregion

#pragma endregion

};
