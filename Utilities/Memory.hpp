#pragma once

#pragma region Dependencies

#include <memory>
#include <type_traits>
#include <utility>

#pragma endregion

template <typename T, typename... Arguments>
constexpr void construct(T* data, Arguments&&... arguments) {
    if constexpr (std::is_trivially_constructible_v<T>)
        *data = T(std::forward<Arguments>(arguments)...);
    else
        std::construct_at(data, std::forward<Arguments>(arguments)...);
}

template <typename T, typename... Arguments>
constexpr void construct(T* data, Arguments&&... arguments, size_t count) {
    for (size_t index = 0; index < count; index++)
        construct(data + index, std::forward<Arguments>(arguments)...);
}

template <typename T>
constexpr void destroy(T* data) {
    if constexpr (!std::is_trivially_destructible_v<T>)
        std::destroy_at(data);
}

template <typename T>
constexpr void destroy(T* data, size_t count) {
    if constexpr (!std::is_trivially_destructible_v<T>)
        for (size_t index = 0; index < count; index++)
            std::destroy_at(data + index);
}

template <typename T>
void relocate(const T* source, T* destination) {
    if constexpr (std::is_trivially_copyable_v<T>)
        std::memcpy(destination, source, sizeof(T));
    else {
        try {
            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>)
                std::construct_at(destination, std::move(source[0]));
            else
                std::construct_at(destination, source[0]);
        }
        catch (...) {
            std::destroy_at(destination);
            throw;
        }
    }
}

template <typename T>
void relocate(const T* source, T* destination, size_t count) {
    if constexpr (std::is_trivially_copyable_v<T>)
        std::memcpy(destination, source, sizeof(T) * count);
    else {
        size_t index = 0;
        try {
            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                for (; index < count; index++)
                    std::construct_at(destination + index, std::move(source[index]));
            }
            else {
                for (; index < count; index++)
                    std::construct_at(destination + index, source[index]);
            }
        }
        catch (...) {
            while (index)
                std::destroy_at(destination + --index);
            throw;
        }
    }
}
