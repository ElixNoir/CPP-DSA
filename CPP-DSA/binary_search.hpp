#pragma once

#include "Comparable.hpp"

namespace Algorithms {

  namespace Search {

    enum BinarySearchType {
      DEFAULT,
      LEFT,
      RIGHT
    };

    template <BinarySearchType SEARCH_TYPE = DEFAULT, DSA::concepts::Comparable T>
    constexpr static size_t binary_search(const T* data, size_t length, const T& key) noexcept {
      size_t first = 0;
      size_t last = length;

      while (first < last) {
        const size_t middle = first + ((last - first) >> 1);
        if constexpr (SEARCH_TYPE == LEFT) {
          if (data[middle] >= key)
            first = middle + 1;
          else
            last = middle;
        }
        else if constexpr (SEARCH_TYPE == RIGHT) {
          if (data[middle] > key)
            first = middle + 1;
          else
            last = middle;
        }
        else {
          if (data[middle] == key)
            return middle;
          else if (data[middle] < key)
            first = middle + 1;
          else
            last = middle;
        }
      }

      return first;
    }

  }

}
