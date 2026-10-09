#pragma once

#include "IncrementalContainer.hpp"

namespace DSA {

    template <concepts::Container CONTAINER>
		requires std::is_nothrow_destructible_v<typename CONTAINER::T>
    class BumpAllocator : IncrementalContainer<CONTAINER> {
    public:

        using Base = IncrementalContainer<CONTAINER>;
        using typename Base::INDEX;
        using typename Base::T;

    protected:

        using Base::data;
		using Base::size;

    public:

#pragma region Constructors & Destructors

        /*~BumpAllocator() {
            // lookup destructor by class (also vtables??)
        }*/

#pragma endregion

#pragma region Methods

#pragma region BumpAllocator

        [[nodiscard]] constexpr bool can_allocate() const noexcept {
            return size <= Base::get_capacity();
        }

        [[nodiscard]] INDEX allocate() {
            return size++;
        }

        [[nodiscard]] constexpr bool can_allocate(size_t count) const noexcept {
            return size * count <= Base::get_capacity();
        }

        [[nodiscard]] INDEX allocate(size_t count) {
            INDEX index = size;
            size += count;
            return index;
        }

#pragma endregion

#pragma endregion

    };

}