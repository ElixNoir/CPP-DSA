#pragma once

template <typename T, bool PASSABLE>
struct Parameter;

template <typename T>
struct Parameter<T, false> {

	constexpr Parameter() noexcept = default;

};

template <typename T>
struct Parameter<T, true> {

    T data;

    Parameter() = delete;
    constexpr Parameter(T val) noexcept : data(val) {}

};
