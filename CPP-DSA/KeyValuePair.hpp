#pragma once

namespace DSA {

    template <typename _KEY, typename _VALUE = EMPTY_STRUCT>
    struct KeyValuePair {

        using KEY = _KEY;
        using VALUE = _VALUE;

        KEY key;
        [[no_unique_address]] VALUE data;

        constexpr KeyValuePair(KEY key) : key(key) {}
        constexpr KeyValuePair(KEY key, VALUE data) : key(key), data(data) {}

    };

    namespace concepts {

        template <typename T>
        concept KeyValuePair = requires(T t) {

            typename T::KEY;
            typename T::VALUE;

            t.key;
            t.data;

        };

    }

    namespace traits {

        template <typename T>
        struct normalize_KeyValuePair {
            using type = KeyValuePair<T>;
        };

        template <concepts::KeyValuePair T>
        struct normalize_KeyValuePair<T> {
            using type = T;
        };

        template <typename T>
        using normalize_KeyValuePair_t = typename normalize_KeyValuePair<T>::type;

    }

}