#pragma once

#include "Runtime.hpp"

#include <utility>

namespace Networking {

	class SocketHandle {
	protected:

		Platform::NativeSocket handle = Platform::InvalidSocket;

		explicit SocketHandle(Platform::NativeSocket socket) noexcept : handle(socket) {}

		friend class Runtime;

		void close() noexcept {
			if (handle == Platform::InvalidSocket)
				return;

			Platform::close_socket(handle);

			handle = Platform::InvalidSocket;

			Runtime::State& runtime = Runtime::singleton();
			std::lock_guard lock(runtime.mutex);

			--runtime.active_sockets;
		}

	public:

		[[nodiscard]] inline static SocketHandle create_socket(int domain, int type, int protocol, ErrorInfo& error) noexcept {
			Runtime::State& runtime = Runtime::singleton();
			std::lock_guard lock(runtime.mutex);

			if (!runtime.initialized) {
				error = { Error::InvalidState, 0 };
				return {};
			}

			const Platform::NativeSocket socket = ::socket(domain, type, protocol);

			if (socket == Platform::InvalidSocket) {
				error = { Error::SystemError, Platform::last_error() };
				return {};
			}

			++runtime.active_sockets;

			error.clear();

			return SocketHandle(socket);
		}

#pragma region Constructors & Destructors

		constexpr SocketHandle() noexcept = default;

		SocketHandle(const SocketHandle&) = delete;
		SocketHandle& operator=(const SocketHandle&) = delete;

		SocketHandle(SocketHandle&& other) noexcept : handle(std::exchange(other.handle, Platform::InvalidSocket)) {}

		SocketHandle& operator=(SocketHandle&& other) noexcept {
			if (this != &other) {
				close();
				handle = std::exchange(other.handle, Platform::InvalidSocket);
			}

			return *this;
		}

		~SocketHandle() noexcept {
			close();
		}

#pragma endregion

#pragma region Methods

#pragma region Checks

		[[nodiscard]] bool is_valid() const noexcept {
			return handle != Platform::InvalidSocket;
		}

		[[nodiscard]] explicit operator bool() const noexcept {
			return is_valid();
		}

#pragma endregion

#pragma endregion

	};

}