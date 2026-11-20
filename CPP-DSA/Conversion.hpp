#pragma once

#include <concepts>
#include <cstdint>

namespace Algorithms {

	namespace Imperial {

		enum class Length : uint8_t {

			FEET,
			INCHES,
			MILES,
			YARDS

		};

	}

	namespace Metric {

		enum class Length : uint8_t {

			CENTIMETERS,
			DECAMETERS,
			DECIMETERS,
			HECTOMETERS,
			KILOMETERS,
			METERS

		};

	}

	template <Metric::Length FROM, Imperial::Length TO, typename T>
		requires std::is_arithmetic_v<T>
	[[nodiscard]] constexpr static T convert_unit(T from) noexcept {
		if constexpr (FROM == Imperial::Length::INCHES)
			from /= 12;
		else if constexpr (FROM == Imperial::Length::MILES)
			from /= 5280;
		else if constexpr (FROM == Imperial::Length::YARDS)
			from /= 3;

		if constexpr (TO == Metric::Length::CENTIMETERS)
			return from * 3.048;
		if constexpr (TO == Metric::Length::DECAMETERS)
			return from * 0.03048;
		if constexpr (TO == Metric::Length::DECIMETERS)
			return from * 30.48;
		if constexpr (TO == Metric::Length::HECTOMETERS)
			return from * 0.003048;
		if constexpr (TO == Metric::Length::KILOMETERS)
			return from * 3048;
		if constexpr (TO == Metric::Length::METERS)
			return from * .3048;
	}

}