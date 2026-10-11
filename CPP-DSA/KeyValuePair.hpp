#pragma once

namespace DSA {

    template <typename KEY, typename VALUE = EMPTY_STRUCT>
    struct KeyValuePair {

        KEY key;
        [[no_unique_address]] VALUE value;

        constexpr KeyValuePair(KEY key) : key(key) {}
        constexpr KeyValuePair(KEY key, VALUE value) : key(key), value(value) {}

    };

    namespace concepts {

        template <typename T>
        concept KeyValuePair = requires(T t) {

            t.key;
            t.value;

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