#pragma once

#include "NetworkingNative.hpp"

namespace Networking {

	class SocketRuntime {
	protected:

#pragma region Properties

		bool initialized = false;

#pragma endregion

	public:

#pragma region Constructors & Destructors

		SocketRuntime() noexcept = default;

		SocketRuntime(const SocketRuntime&) = delete;
		SocketRuntime& operator=(const SocketRuntime&) = delete;

		SocketRuntime(SocketRuntime&&) = delete;
		SocketRuntime& operator=(SocketRuntime&&) = delete;

		~SocketRuntime() noexcept {
			shutdown();
		}

#pragma endregion

#pragma region Methods

#pragma region Checks

		[[nodiscard]] bool is_initialized() const noexcept {
			return initialized;
		}

#pragma endregion

#pragma region Initialize & Shutdown

		[[nodiscard]] bool initialize(int& error) noexcept {
			if (initialized) {
				error = 0;
				return true;
			}

#if defined(NETWORKING_WINDOWS)
			WSADATA data{};
			const int result = WSAStartup(MAKEWORD(2, 2), &data);
			if (result != 0) {
				error = result;
				return false;
			}
#endif
			initialized = true;
			error = 0;

			return true;
		}

		void shutdown() noexcept {
			if (!initialized)
				return;

#if defined(NETWORKING_WINDOWS)
			WSACleanup();
#endif
			initialized = false;
		}

#pragma endregion

#pragma endregion

	};

}