#pragma once

#include "repeat.hpp"
#include "Traits.hpp"

#include <concepts>
#include <memory>

namespace Algorithms {

	namespace Memory {

		template <typename T, typename... Arguments>
		constexpr static void construct_at(T* address, Arguments&&... arguments)
			noexcept(std::is_nothrow_constructible_v<T, Arguments...>)
		{
			std::construct_at<T>(address, std::forward<Arguments>(arguments)...);
		}

		template <bool LEFT_INCLUSIVE = true, typename T, typename... Arguments>
		constexpr static void construct_range_backward(T* begin, T* end, Arguments&&... arguments)
			noexcept(std::is_nothrow_constructible_v<T, Arguments...>)
		{
			if constexpr (std::is_trivially_constructible_v<T, Arguments...>)
				repeat(begin + !LEFT_INCLUSIVE, end + !LEFT_INCLUSIVE, T(forward<Arguments>(arguments)...));
			else {
				T* const e = end + !LEFT_INCLUSIVE;
				try {
					while (begin != end)
						construct_at(LEFT_INCLUSIVE ? --end : end--, forward<Arguments>(arguments)...);
				}
				catch (...) {
					destroy_range_backward(end, e);
					throw;
				}
			}
		}

		template <bool LEFT_INCLUSIVE = true, typename T, typename... Arguments>
		constexpr static void construct_range_forward(T* begin, T* end, Arguments&&... arguments)
			noexcept(std::is_nothrow_constructible_v<T, Arguments...>)
		{
			if constexpr (std::is_trivially_constructible_v<T, Arguments...>)
				repeat(begin + !LEFT_INCLUSIVE, end + !LEFT_INCLUSIVE, T(forward<Arguments>(arguments)...));
			else {
				T* const b = begin + !LEFT_INCLUSIVE;
				try {
					while (begin != end)
						construct_at(LEFT_INCLUSIVE ? begin++ : ++begin, forward<Arguments>(arguments)...);
				}
				catch (...) {
					destroy_range_forward(b, begin);
					throw;
				}
			}
		}

		template <typename T>
		constexpr static void copy_construct_at(T* destination, const T& source)
			noexcept(std::is_nothrow_copy_constructible_v<T>
				|| std::is_trivially_copy_constructible_v<T>)
		{
			construct_at(destination, source);
		}

		template <bool DESTINATION_LEFT_INCLUSIVE = true, bool SOURCE_LEFT_INCLUSIVE = true, typename T>
		constexpr static void copy_construct_range_backward(T* begin, T* end, T* source)
			noexcept(std::is_nothrow_copy_constructible_v<T>
				|| std::is_trivially_copyable_v<T>)
		{
			if constexpr (std::is_trivially_copy_constructible_v<T>)
				std::memcpy(begin + (!DESTINATION_LEFT_INCLUSIVE) * sizeof(T), source + (!SOURCE_LEFT_INCLUSIVE) * sizeof(T), (end - begin) * sizeof(T));
			else {
				T* const e = end + !DESTINATION_LEFT_INCLUSIVE;
				try {
					while (begin != end)
						copy_construct_at(
							DESTINATION_LEFT_INCLUSIVE ? --end : end--,
							*(SOURCE_LEFT_INCLUSIVE ? --source : source--));
				}
				catch (...) {
					destroy_range_backward<DESTINATION_LEFT_INCLUSIVE>(end, e);
					throw;
				}
			}
		}

		template <bool DESTINATION_LEFT_INCLUSIVE = true, bool SOURCE_LEFT_INCLUSIVE = true, typename T>
		constexpr static void copy_construct_range_forward(T* begin, T* end, T* source)
			noexcept(std::is_nothrow_copy_constructible_v<T>
				|| std::is_trivially_copyable_v<T>)
		{
			if constexpr (std::is_trivially_copy_constructible_v<T>)
				std::memcpy(begin + (!DESTINATION_LEFT_INCLUSIVE) * sizeof(T), source + (!SOURCE_LEFT_INCLUSIVE) * sizeof(T), (end - begin) * sizeof(T));
			else {
				T* const b = begin + !DESTINATION_LEFT_INCLUSIVE;
				try {
					while (begin != end)
						copy_construct_at(
							DESTINATION_LEFT_INCLUSIVE ? begin++ : ++begin,
							*(SOURCE_LEFT_INCLUSIVE ? source++ : ++source));
				}
				catch (...) {
					destroy_range_forward<DESTINATION_LEFT_INCLUSIVE>(b, begin);
					throw;
				}
			}
		}

		template <typename T>
		constexpr static void destroy_at(T* address) noexcept {
			std::destroy_at(address);
		}

		template <bool LEFT_INCLUSIVE = true, typename T>
		constexpr static void destroy_range_backward(T* begin, T* end) noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				while (begin != end)
					destroy_at(LEFT_INCLUSIVE ? --end : end--);
		}

		template <bool LEFT_INCLUSIVE = true, typename T>
		constexpr static void destroy_range_forward(T* begin, T* end) noexcept {
			if constexpr (!std::is_trivially_destructible_v<T>)
				while (begin != end)
					destroy_at(LEFT_INCLUSIVE ? begin++ : ++begin);
		}

		template <typename T>
		constexpr static void move_construct_at(T* destination, T&& source) noexcept(
			std::is_nothrow_move_constructible_v<T>
			|| std::is_trivially_copyable_v<T>
			) {
			if constexpr (std::is_trivially_copyable_v<T>)
				std::memcpy(destination, &source, sizeof(T));
			else
				construct_at(destination, std::move(source));
		}

		template <bool DESTINATION_LEFT_INCLUSIVE = true, bool SOURCE_LEFT_INCLUSIVE = true, typename T>
		constexpr static void move_construct_range_backward(T* begin, T* end, T* source)
			noexcept(std::is_nothrow_move_constructible_v<T>
				|| std::is_trivially_copyable_v<T>)
		{
			if constexpr (std::is_trivially_copyable_v<T>)
				std::memcpy(begin + (!DESTINATION_LEFT_INCLUSIVE) * sizeof(T), source + (!SOURCE_LEFT_INCLUSIVE) * sizeof(T), (end - begin) * sizeof(T));
			else {
				T* const e = end + !DESTINATION_LEFT_INCLUSIVE;
				try {
					while (begin != end)
						move_construct_at(
							DESTINATION_LEFT_INCLUSIVE ? --end : end--,
							std::move(*(SOURCE_LEFT_INCLUSIVE ? --source : source--)));
				}
				catch (...) {
					destroy_range_backward<DESTINATION_LEFT_INCLUSIVE>(end, e);
					throw;
				}
			}
		}

		template <bool DESTINATION_LEFT_INCLUSIVE = true, bool SOURCE_LEFT_INCLUSIVE = true, typename T>
		constexpr static void move_construct_range_forward(T* begin, T* end, T* source)
			noexcept(std::is_nothrow_move_constructible_v<T>
				|| std::is_trivially_copyable_v<T>)
		{
			if constexpr (std::is_trivially_copyable_v<T>)
				std::memcpy(begin + (!DESTINATION_LEFT_INCLUSIVE) * sizeof(T), source + (!SOURCE_LEFT_INCLUSIVE) * sizeof(T), (end - begin) * sizeof(T));
			else {
				T* const b = begin + !DESTINATION_LEFT_INCLUSIVE;
				try {
					while (begin != end)
						move_construct_at(
							DESTINATION_LEFT_INCLUSIVE ? begin++ : ++begin,
							std::move(*(SOURCE_LEFT_INCLUSIVE ? source++ : ++source)));
				}
				catch (...) {
					destroy_range_forward<DESTINATION_LEFT_INCLUSIVE>(b, begin);
					throw;
				}
			}
		}

		template <typename T>
		constexpr static void reconstruct_at(T* destination, T&& source)
			noexcept(std::is_nothrow_move_constructible_v<T>
				|| std::is_trivially_copyable_v<T>)
		{
			move_construct_at(destination, std::move(source));
			destroy_at(&source);
		}

		template <bool DESTINATION_LEFT_INCLUSIVE = true, bool SOURCE_LEFT_INCLUSIVE = true, typename T>
		constexpr static void reconstruct_range_backward(T* begin, T* end, T* source)
			noexcept(std::is_nothrow_move_constructible_v<T>
				|| std::is_trivially_copyable_v<T>)
		{
			if constexpr (std::is_trivially_copyable_v<T>)
				std::memcpy(begin + (!DESTINATION_LEFT_INCLUSIVE) * sizeof(T), source + (!SOURCE_LEFT_INCLUSIVE) * sizeof(T), (end - begin) * sizeof(T));
			else {
				T* const e = end + !DESTINATION_LEFT_INCLUSIVE;
				try {
					source += end - begin;
					while (begin != end)
						reconstruct_at(
							DESTINATION_LEFT_INCLUSIVE ? --end : end--,
							std::move(*(SOURCE_LEFT_INCLUSIVE ? --source : source--)));
				}
				catch (...) {
					destroy_range_backward<DESTINATION_LEFT_INCLUSIVE>(end, e);
					throw;
				}
			}
		}

		template <bool DESTINATION_LEFT_INCLUSIVE = true, bool SOURCE_LEFT_INCLUSIVE = true, typename T>
		constexpr static void reconstruct_range_forward(T* begin, T* end, T* source)
			noexcept(std::is_nothrow_move_constructible_v<T>
				|| std::is_trivially_copyable_v<T>)
		{
			if constexpr (std::is_trivially_copyable_v<T>)
				std::memcpy(begin + (!DESTINATION_LEFT_INCLUSIVE) * sizeof(T), source + (!SOURCE_LEFT_INCLUSIVE) * sizeof(T), (end - begin) * sizeof(T));
			else {
				T* const b = begin + !DESTINATION_LEFT_INCLUSIVE;
				try {
					while (begin != end)
						reconstruct_at(
							DESTINATION_LEFT_INCLUSIVE ? begin++ : ++begin,
							std::move(*(SOURCE_LEFT_INCLUSIVE ? source++ : ++source)));
				}
				catch (...) {
					destroy_range_forward<DESTINATION_LEFT_INCLUSIVE>(b, begin);
					throw;
				}
			}
		}

		template <typename T>
		constexpr static void replace_at(T* destination, T&& source)
			noexcept(std::is_nothrow_move_constructible_v<T>
				|| std::is_trivially_copyable_v<T>)
		{
			destroy_at(destination);
			move_construct_at(destination, std::move(source));
			destroy_at(&source);
		}

		template <bool DESTINATION_LEFT_INCLUSIVE = true, bool SOURCE_LEFT_INCLUSIVE = true, typename T>
		constexpr static void replace_range_backward(T* begin, T* end, T* source)
			noexcept(std::is_nothrow_move_constructible_v<T>
				|| std::is_trivially_copyable_v<T>)
		{
			if constexpr (std::is_trivially_copyable_v<T>)
				std::memcpy(begin + (!DESTINATION_LEFT_INCLUSIVE) * sizeof(T), source + (!SOURCE_LEFT_INCLUSIVE) * sizeof(T), (end - begin) * sizeof(T));
			else {
				T* const e = end + !DESTINATION_LEFT_INCLUSIVE;
				try {
					source += end - begin;
					while (begin != end)
						replace_at(
							DESTINATION_LEFT_INCLUSIVE ? --end : end--,
							std::move(*(SOURCE_LEFT_INCLUSIVE ? --source : source--)));
				}
				catch (...) {
					destroy_range_backward<DESTINATION_LEFT_INCLUSIVE>(end, e);
					throw;
				}
			}
		}

		template <bool DESTINATION_LEFT_INCLUSIVE = true, bool SOURCE_LEFT_INCLUSIVE = true, typename T>
		constexpr static void replace_range_forward(T* begin, T* end, T* source)
			noexcept(std::is_nothrow_move_constructible_v<T>
				|| std::is_trivially_copyable_v<T>)
		{
			if constexpr (std::is_trivially_copyable_v<T>)
				std::memcpy(begin + (!DESTINATION_LEFT_INCLUSIVE) * sizeof(T), source + (!SOURCE_LEFT_INCLUSIVE) * sizeof(T), (end - begin) * sizeof(T));
			else {
				T* const b = begin + !DESTINATION_LEFT_INCLUSIVE;
				try {
					while (begin != end)
						replace_at(
							DESTINATION_LEFT_INCLUSIVE ? begin++ : ++begin,
							std::move(*(SOURCE_LEFT_INCLUSIVE ? source++ : ++source)));
				}
				catch (...) {
					destroy_range_forward<DESTINATION_LEFT_INCLUSIVE>(b, begin);
					throw;
				}
			}
		}

	}

}