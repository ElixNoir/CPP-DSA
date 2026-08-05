#pragma once

#pragma region Dependencies

#include <concepts>
#include <mutex>
#include <shared_mutex>
#include <type_traits>

#pragma endregion

template <typename T>
concept Lockable =
    requires(T t) {
        { t.lock() };
        { t.unlock() };
    };

template <typename T>
concept SharedLockable =
    Lockable<T> &&
    requires(T t) {
        { t.lock_shared() };
        { t.unlock_shared() };
    };

template <typename T>
concept ParallelSafe = std::is_trivially_copyable_v<T> || std::is_empty_v<T>;
