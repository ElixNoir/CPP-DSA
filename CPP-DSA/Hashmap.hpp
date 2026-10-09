#pragma once

#include <concepts>

template <typename T, std::unsigned_integral INDEX = size_t>
INDEX default_hash(T& value) noexcept {
	return 0; // to-do
}

template <typename T, std::unsigned_integral INDEX = size_t, INDEX (*HASH)(T&) = default_hash>
class Hashmap {
protected:

public:

};