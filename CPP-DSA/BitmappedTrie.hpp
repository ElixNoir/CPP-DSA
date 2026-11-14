#pragma once

#include "Bitset.hpp"
#include "Collection.hpp"
#include "Stack.hpp"

namespace DSA {

    template <concepts::StoragePolicy StoragePolicy, typename T>
        requires std::is_nothrow_destructible_v<T>
    class BitmappedTrie : public Collection<StoragePolicy, T> {
    public:

        using Base = Collection<StoragePolicy, T>;
        using typename Base::INDEX;
        using typename Base::T;

        constexpr static INDEX bit_count = Bitmask<>::bit_count;
        constexpr static INDEX bit_shift = Bitmask<>::bit_shift;

    protected:

    public:

    };

}
