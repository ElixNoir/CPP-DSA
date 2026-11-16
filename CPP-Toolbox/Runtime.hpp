#pragma once

#pragma region Dependencies

#include "NetworkingNative.hpp"
#include "NetworkError.hpp"

#include <cstddef>
#include <mutex>

#pragma endregion

namespace Networking {

	class SocketHandle;

	class Runtime {
	protected:

		struct State {
			std::mutex mutex;

			bool initialized = false;
			size_t active_sockets = 0;
		};

		[[nodiscard]] static State& singleton() noexcept {
			static State instance;
			return instance;
		}

		friend class SocketHandle;

	public:

		Runtime() = delete;

#pragma region Methods

		[[nodiscard]] static size_t active_sockets() noexcept {
			State& runtime = singleton();
			std::lock_guard lock(runtime.mutex);

			return runtime.active_sockets;
		}

		[[nodiscard]] static bool is_initialized() noexcept {
			State& runtime = singleton();
			std::lock_guard lock(runtime.mutex);

			return runtime.initialized;
		}

#pragma region Initialize & Shutdown

		[[nodiscard]] static bool initialize(ErrorInfo& error) noexcept {
			State& runtime = state();
			std::lock_guard lock(runtime.mutex);

			if (runtime.initialized) {
				error.clear();
				return true;
			}

#if defined(NETWORKING_WINDOWS)
			WSADATA data{};
			const int result = WSAStartup(MAKEWORD(2, 2), &data);
			if (result != 0) {
				error = { Error::InitializationFailed, result };
				return false;
			}
#endif

			runtime.initialized = true;

			error.clear();
			return true;
		}

		[[nodiscard]] static bool shutdown(ErrorInfo& error) noexcept {
			State& runtime = state();
			std::lock_guard lock(runtime.mutex);

			if (!runtime.initialized) {
				error.clear();
				return true;
			}

			if (runtime.active_sockets != 0) {
				error = { Error::InvalidState, 0 };
				return false;
			}

#if defined(NETWORKING_WINDOWS)
			if (WSACleanup() != 0) {
				error = { Error::SystemError, WSAGetLastError() };
				return false;
			}
#endif

			runtime.initialized = false;

			error.clear();
			return true;
		}

#pragma endregion

#pragma endregion

	};

}