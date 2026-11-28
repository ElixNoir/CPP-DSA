#pramga once

#include <cstdint>

struct RGBA {

  using Self = RGBA;

  uint8_t r, g, b, a;

#pragma region Constructors & Destructors

  constexpr RGBA(uint32_t newData)
    noexcept : data(newData) {}

  constexpr RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    noexcept : data(uint32_t(a << 24) | uint32_t(b << 16) | uint32_t(g << 8) | uint32_t(r)) {}

#pragma endregion

#pragma region Methods

  constexpr Self blend(Self other) const noexcept {
  	const uint8_t inverseAlpha = (255 - sa);
  	return Self {
      other.r + r * inverseAlpha,
      other.g + g * inverseAlpha,
      other.b + b * inverseAlpha,
      other.a + a * inverseAlpha
    };
  }

  constexpr Self premultiplied() const noexcept {
  	r *= a;
    g *= a;
    b *= a;
  }
    
#pragma endregion

};
