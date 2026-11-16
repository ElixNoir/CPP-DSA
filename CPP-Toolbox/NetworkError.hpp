#pragma once

#include <cstdint>

namespace Networking {

	enum class Error : std::uint8_t {

		None,

		InvalidArgument,
		InvalidState,

		InitializationFailed,

		AddressFamilyNotSupported,
		AddressInUse,
		AddressNotAvailable,

		ConnectionRefused,
		ConnectionReset,
		ConnectionAborted,
		ConnectionClosed,

		NetworkUnreachable,
		HostUnreachable,

		TimedOut,

		WouldBlock,
		Interrupted,

		MessageTooLarge,

		ResourceExhausted,
		PermissionDenied,

		SystemError,
		Unknown

	};

	struct ErrorInfo {

		Error code = Error::None;
		int native_code = 0;

#pragma region Methods

		constexpr void clear() noexcept {
			code = Error::None;
			native_code = 0;
		}

		[[nodiscard]] constexpr bool succeeded() const noexcept {
			return code == Error::None;
		}

		[[nodiscard]] constexpr explicit operator bool() const noexcept {
			return !succeeded();
		}

#pragma endregion

	};

}