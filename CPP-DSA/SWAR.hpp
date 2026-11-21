#pragma once

#include <iostream>
#include <cstdint>

struct swar8x8 {

	using T = uint8_t;
	using U = uint64_t;
	using Self = swar8x8;

	constexpr static U msbon = 0x8080808080808080U;

#pragma region Properties

	U data;

#pragma endregion

#pragma region Constructors & Destructors

	swar8x8() noexcept = default;

	constexpr swar8x8(U newData) noexcept : data(newData) {}

	template <std::integral V>
	constexpr static Self from_broadcast(V broadcastedValue) noexcept {
		return U(broadcastedValue) * 0x0101010101010101ULL;
	}

	constexpr static Self from_set(T e0, T e1, T e2, T e3, T e4, T e5, T e6, T e7) noexcept {
		return (U(e7) << 56) | (U(e6) << 48) | (U(e5) << 40) | (U(e4) << 32) | (U(e3) << 24) | (U(e2) << 16) | (U(e1) << 8) | U(e0);
	}

#pragma endregion

#pragma region Methods

	template <std::integral V>
	constexpr void broadcast(V broadcastedValue) noexcept {
		data = from_broadcast(broadcastedValue).data;
	}

	[[nodiscard]] constexpr T get(U index) const noexcept {
		return T(data >> (index * 8));
	}

	constexpr void set(T e0, T e1, T e2, T e3, T e4, T e5, T e6, T e7) noexcept {
		data = from_set(e0, e1, e2, e3, e4, e5, e6, e7).data;
	}

#pragma region Arithmetic

	[[nodiscard]] constexpr Self add(Self other) const noexcept {
		return data + other.data;
	}

	[[nodiscard]] constexpr Self add_no_overflow(Self other) const noexcept {
		return ((data & ~msbon) + (other.data & ~msbon)) ^ ((data ^ other.data) & msbon);
	}

	[[nodiscard]] constexpr Self add_saturated(Self other) const noexcept {
		const T sum = data + other.data;
		return (sum < data) ? 0xFF : sum;
	}

	[[nodiscard]] constexpr Self sub(Self other) const noexcept {
		return data - other.data;
	}

	[[nodiscard]] constexpr Self sub_no_overflow(Self other) const noexcept {
		return Self{ ((data | msbon) - (other.data & ~msbon)) ^ ((data ^ ~other.data) & msbon) };
	}

	[[nodiscard]] constexpr Self sub_saturated(Self other) const noexcept {
		const T sum = data - other.data;
		return (sum > data) ? 0xFF : sum;
	}

#pragma endregion

#pragma region Comparison

	constexpr U has_zero() const noexcept {
		return (data - 0x0101010101010101ull) & ~data & 0x8080808080808080ull;
	}

#pragma endregion

#pragma region Operators

	[[nodiscard]] constexpr Self operator+(Self other) const noexcept {
		return add_no_overflow(other.data);
	}

	constexpr Self& operator+=(const Self& other) noexcept {
		data = (*this + other).data;
		return *this;
	}

	[[nodiscard]] constexpr Self operator-(Self other) const noexcept {
		return sub_no_overflow(other.data);
	}

	constexpr Self& operator-=(const Self& other) noexcept {
		data = (*this - other).data;
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& out, const Self t) {
		out << '{' << t.get(0) << ", " << t.get(1) << ", " << t.get(2) << ", " << t.get(3) << ", " << t.get(4) << ", " << t.get(5) << ", " << t.get(6) << ", " << t.get(7) << '}';
		return out;
	}

#pragma endregion

#pragma endregion

};

struct swar16x4 {

	using T = uint16_t;
	using U = uint64_t;
	using Self = swar16x4;

	constexpr static U msbon = 0x8000800080008000U;

#pragma region Properties

	U data;

#pragma endregion

#pragma region Constructors & Destructors

	swar16x4() noexcept = default;

	constexpr swar16x4(U newData) noexcept : data(newData) {}

	template <std::integral V>
	constexpr static Self from_broadcast(V broadcastedValue) noexcept {
		return U(broadcastedValue) * 0x0001000100010001ULL;
	}

	constexpr static Self from_set(T e0, T e1, T e2, T e3) noexcept {
		return (U(e3) << 48) | (U(e2) << 32) | (U(e1) << 16) | U(e0);
	}

#pragma endregion

#pragma region Methods

	template <std::integral V>
	constexpr void broadcast(V broadcastedValue) noexcept {
		data = from_broadcast(broadcastedValue).data;
	}

	[[nodiscard]] constexpr T get(U index) const noexcept {
		return T(data >> (index * 16));
	}

	constexpr void set(T e0, T e1, T e2, T e3) noexcept {
		data = from_set(e0, e1, e2, e3).data;
	}

#pragma region Arithmetic

	[[nodiscard]] constexpr Self add(Self other) const noexcept {
		return data + other.data;
	}

	[[nodiscard]] constexpr Self add_no_overflow(Self other) const noexcept {
		return ((data & ~msbon) + (other.data & ~msbon)) ^ ((data ^ other.data) & msbon);
	}

	[[nodiscard]] constexpr Self add_saturated() const noexcept {
		const T sum = data + other.data;
		return (sum < data) ? 0xFFFF : sum;
	}

	[[nodiscard]] constexpr Self sub(Self other) const noexcept {
		return data - other.data;
	}

	[[nodiscard]] constexpr Self sub_no_overflow(Self other) const noexcept {
		return Self{ ((data | msbon) - (other.data & ~msbon)) ^ ((data ^ ~other.data) & msbon) };
	}

	[[nodiscard]] constexpr Self sub_saturated() const noexcept {
		const T sum = data - other.data;
		return (sum > data) ? 0xFFFF : sum;
	}

#pragma endregion

#pragma region Comparison

	constexpr U has_zero() const noexcept {
		return (data - 0x0001000100010001ull) & ~data & 0x8000800080008000ull;
	}

#pragma endregion

#pragma region Operators

	[[nodiscard]] constexpr Self operator+(Self other) const noexcept {
		return add_no_overflow(other.data);
	}

	constexpr Self& operator+=(const Self& other) noexcept {
		data = (*this + other).data;
		return *this;
	}

	[[nodiscard]] constexpr Self operator-(Self other) const noexcept {
		return sub_no_overflow(other.data);
	}

	constexpr Self& operator-=(const Self& other) noexcept {
		data = (*this - other).data;
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& out, const Self t) {
		out << '{' << t.get(0) << ", " << t.get(1) << ", " << t.get(2) << ", " << t.get(3) << '}';
		return out;
	}

#pragma endregion

#pragma endregion

};
