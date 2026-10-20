#pragma once

#include "Bitset.hpp"
#include "IncrementalContainer.hpp"
#include "Stack.hpp"

namespace DSA {

    template <typename ContainerConfiguration, typename T>
        requires std::is_nothrow_destructible_v<T>
    class BitmappedTrie : public ContainerConfiguration::template CONTAINER<T> {
    public:

        using Base = ContainerConfiguration::template CONTAINER<T>;
        using typename Base::INDEX;
        using typename Base::T;

    protected:

    public:

    };

}