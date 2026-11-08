#pragma once

#include "Bitset.hpp"
#include "Collection.hpp"
#include "Stack.hpp"

namespace DSA {

    template <concepts::StoragePolicy ContainerConfiguration, typename T>
        requires std::is_nothrow_destructible_v<T>
    class BitmappedTrie : public ContainerConfiguration::template STRUCTURE<T> {
    public:

        using Base = ContainerConfiguration::template STRUCTURE<T>;
        using typename Base::INDEX;
        using typename Base::T;

    protected:

    public:

    };

}