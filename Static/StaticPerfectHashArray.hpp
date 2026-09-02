#pragma once

#pragma region Dependencies

#include <array>
#include <cstdint>

#pragma endregion

template <typename V>
struct KeyValuePair {
    
    const char* Key;
    V Value;
    
};
    
template <>
struct KeyValuePair<void> {
    
    const char* Key;
    
};

template <size_t KeyCount, typename Value = void, size_t ArraySize = static_cast<size_t>(KeyCount * 1.5)>
class StaticPerfectHashArray {
    
    static_assert(KeyCount <= ArraySize);

public:

    using Entry = KeyValuePair<Value>;

protected:

    [[nodiscard]] constexpr static size_t fnv1a_prime() const noexcept {
        if constexpr (sizeof(size_t) == sizeof(uint32_t))
            return static_cast<size_t>(0x01000193u);
        else
            return static_cast<size_t>(0x100000001b3ULL);
    }

    static constexpr size_t FNV1aPrime = fnv1a_prime();

#pragma region Properties

    size_t Seed = 0;
    std::array<Entry, ArraySize> Data{};

#pragma endregion

public:
    
    consteval StaticPerfectHashArray(const std::initializer_list<Entry> list) {
        while (true) {
            std::array<bool, ArraySize> visited{};
            
            for (Entry entry : list) {
                const size_t index = hash(entry.Key) % ArraySize;
    
                if (visited[index]) {
                    Seed++;
                    continue;
                }
    
                visited[index] = true;
            }
            
            break;
        }
        
        for (Entry entry : list)
            Data[hash(entry.Key) % ArraySize] = { entry.Key, entry.Value };
    }
    
    consteval StaticPerfectHashArray(const std::initializer_list<const char*> list) requires std::is_void_v<Value> {
        while (true) {
            std::array<bool, ArraySize> visited{};
            
            for (const char* key : list) {
                const size_t index = hash(key) % ArraySize;
    
                if (visited[index]) {
                    Seed++;
                    continue;
                }
    
                visited[index] = true;
            }
            
            break;
        }
        
        for (const char* key : list)
            Data[hash(key) % ArraySize].Key = key;
    }
    
#pragma region Methods

#pragma region Getters

    [[nodiscard]] constexpr std::array<const char*, ArraySize>& data() const noexcept {
        return Data;
    }
    
    [[nodiscard]] constexpr const Entry& pair_at(size_t index) const noexcept requires (!std::is_void_v<Value>) {
        return Data[index];
    }

    [[nodiscard]] constexpr size_t seed() const noexcept {
        return Seed;
    }
    
#pragma endregion

    [[nodiscard]] constexpr bool contains(const char* key) const noexcept {
        return key == Data[hash(key) % ArraySize].Key;
    }

    [[nodiscard]] constexpr const Entry* find(const char* key) const noexcept {
        const Entry* entry = &Data[hash(key) % ArraySize];
        return key == entry->Key ? entry : nullptr;
    }
  
    [[nodiscard]] constexpr size_t hash(const char* key) const noexcept {
        size_t hash = Seed;

        for (size_t index = 0; key[index] != '\0'; ++index) {
            hash ^= static_cast<size_t>(static_cast<unsigned char>(key[index]));
            hash *= FNV1aPrime;
        }

        return hash;
    }
    
#pragma endregion
    
};
