#pragma once

#include <utility>
#include <functional> // Required for std::invoke

namespace Algorithms {

	template <size_t UNROLL_COUNT, auto OPERATION>
	constexpr void unroll()
		noexcept(noexcept(std::invoke(OPERATION, std::declval<size_t>())))
	{
		[&] <size_t... INDEX_SEQUENCE>(std::index_sequence<INDEX_SEQUENCE...>) {
			(std::invoke(OPERATION, INDEX_SEQUENCE), ...);
		}(std::make_index_sequence<UNROLL_COUNT>{});
	}

	template <size_t UNROLL_COUNT, typename OPERATION>
	constexpr void unroll(OPERATION&& operation)
		noexcept(noexcept(std::invoke(std::forward<OPERATION>(operation), std::declval<size_t>())))
	{
		[&] <size_t... INDEX_SEQUENCE>(std::index_sequence<INDEX_SEQUENCE...>) {
			(std::invoke(std::forward<OPERATION>(operation), INDEX_SEQUENCE), ...);
		}(std::make_index_sequence<UNROLL_COUNT>{});
	}

	template <size_t UNROLL_COUNT, auto OPERATION, typename ITERATOR>
	void unroll_forward_iterator(ITERATOR begin, ITERATOR end)
		noexcept(noexcept(std::invoke(OPERATION, *begin)))
	{
		while (begin != end) {
			unroll<UNROLL_COUNT>([&](auto) {
				std::invoke(OPERATION, *(begin++));
			});
		}
	}

	template <size_t UNROLL_COUNT, typename OPERATION, typename ITERATOR>
	void unroll_forward_iterator(ITERATOR begin, ITERATOR end, OPERATION&& operation)
		noexcept(noexcept(std::invoke(std::forward<OPERATION>(operation), *begin)))
	{
		while (begin != end) {
			unroll<UNROLL_COUNT>([&](auto) {
				std::invoke(operation, *(begin++));
			});
		}
	}

}
