#include "SocketHandle.hpp"

namespace Networking {

	struct TransferResult {

		size_t transferred = 0;
		ErrorInfo error{};

		[[nodiscard]] constexpr bool succeeded() const noexcept {
			return !error;
		}

		[[nodiscard]] constexpr bool complete(size_t requested) const noexcept {
			return succeeded() && transferred == requested;
		}

	};

}

int test() {

	Networking::ErrorInfo error;

	if (!Networking::Runtime::initialize(error))
		return 1;

	{
		using namespace Networking;

		SocketHandle socket = SocketHandle::create_socket(
			AF_INET,
			SOCK_STREAM,
			IPPROTO_TCP,
			error
		);

		if (!socket)
			return 2;

		// TCP operations will go here.
	}

	// SocketHandle has now been destroyed.

	if (!Networking::Runtime::shutdown(error))
		return 3;

	return 0;
}