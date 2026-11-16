#pragma once

#pragma region Dependencies

#if defined(_WIN32)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define NETWORKING_WINDOWS 1

#elif defined(__APPLE__) || defined(__linux__)

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define NETWORKING_POSIX 1

#else

#error Unsupported operating system

#endif

#include <cstdint>
#include <cstddef>

#pragma endregion

namespace Networking::Platform {

#pragma region Definitions

#if defined(NETWORKING_WINDOWS)
	using NativeSocket = SOCKET;
	using SocketLength = int;

	inline constexpr NativeSocket InvalidSocket = INVALID_SOCKET;
#else
	using NativeSocket = int;
	using SocketLength = socklen_t;

	inline constexpr NativeSocket InvalidSocket = -1;
#endif

	using SocketError = int;

#pragma endregion

#pragma region Methods

#pragma region Checks

	[[nodiscard]] inline bool interrupted(SocketError error) noexcept {
#if defined(NETWORKING_WINDOWS)
		return error == WSAEINTR;
#else
		return error == EINTR;
#endif
	}

	[[nodiscard]] inline SocketError last_error() noexcept {
#if defined(NETWORKING_WINDOWS)
		return WSAGetLastError();
#else
		return errno;
#endif
	}

	[[nodiscard]] inline bool would_block(SocketError error) noexcept {
#if defined(NETWORKING_WINDOWS)
		return error == WSAEWOULDBLOCK;
#else
		return error == EAGAIN || error == EWOULDBLOCK;
#endif
	}

#pragma endregion

#pragma region Close, Shutdown, & Set Nonblocking

	inline void close_socket(NativeSocket socket) noexcept {
		if (socket == InvalidSocket)
			return;

#if defined(NETWORKING_WINDOWS)
		closesocket(socket);
#else
		::close(socket);
#endif
	}

	enum class ShutdownMode : uint8_t {
		Receive,
		Send,
		Both
	};

	[[nodiscard]] inline bool shutdown_socket(NativeSocket socket, ShutdownMode mode) noexcept {
		int how;

		switch (mode) {
		case ShutdownMode::Receive:
			how = 0;
			break;
		case ShutdownMode::Send:
			how = 1;
			break;
		case ShutdownMode::Both:
			how = 2;
			break;
		default:
			return false;
		}

#if defined(NETWORKING_WINDOWS)
		return ::shutdown(socket, how) == 0;
#else
		constexpr int modes[] = { SHUT_RD, SHUT_WR, SHUT_RDWR };
		return ::shutdown(socket, modes[static_cast<std::uint8_t>(mode)]) == 0;
#endif
	}

	[[nodiscard]] inline bool set_nonblocking(NativeSocket socket, bool enabled) noexcept {
#if defined(NETWORKING_WINDOWS)
		u_long mode = enabled ? 1UL : 0UL;
		return ioctlsocket(socket, FIONBIO, &mode) == 0;
#else
		const int flags = fcntl(socket, F_GETFL, 0);

		if (flags == -1)
			return false;

		return fcntl(socket, F_SETFL, enabled ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK)) != -1;
#endif
	}

#pragma endregion

#pragma endregion

}