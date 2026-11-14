#pragma region Dependencies

#include "PointerHashmap.hpp"

#include <iostream>
#include <format>

#pragma endregion

using namespace DSA;

struct String {

    const char* data = nullptr;
    size_t size = 0;

    constexpr String(const char* data) noexcept : data(data), size(strlen(data)) {}
    constexpr String(const char* data, size_t size) noexcept : data(data), size(size) {}

    constexpr auto hash() const noexcept {
        return Algorithms::hash_pointer(data);
    }

    friend std::ostream& operator<<(std::ostream& ostream, const String& t) {
        ostream << t.data;
        return ostream;
    }

};

int main() {
    String string = "hello";
    PointerHashmap<InlineStoragePolicy<16>, String> h;
    auto index = h.insert(&string);
    std::cout << *h.get(index);
    return 0;
}
