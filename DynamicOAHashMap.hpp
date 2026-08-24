#pragma once

// W.I.P., as all the others are.

#pragma region Dependencies

#include "DSAConcepts.hpp"

#pragma endregion

template <typename Key, typename Value>
struct KeyValuePair {

  Key Key;
  Value Value;

};

//. If the key's default type does not signify inactivity, then a bitmap corresponding to the indices may be used.  This will be done at a later date.
template <typename Key, typename Value = void, Index(*hash)(Key), std::unsigned_integral Index = size_t, Allocator A = DefaultAllocator>
class DynamicOAHashMap : public DynamicIncrementalAoSContainer<KeyValuePair<Key, Value>> {
public:

  using Base = DynamicIncrementalAoSContainer<KeyValuePair<Key, Value>>;

  DynamicOAHashMap(Index initialCapacity) : Base(initialCapacity) {}

  DynamicOAHashMap(DynamicAoSContainer& other) : Base(other) {}

  DynamicOAHashMap(DynamicAoSContainer&& other) : Base(other) {}

  ~DynamicOAHashMap() {

  }

#pragma region Methods

#pragma region Getters

  //. If the key is changed, it is only legal to do so if the corresponding hash is not changed.
  [[nodsicard]] Key& key_of(Index index) {
    return Data[index].Key;
  }

  [[nodsicard]] KeyValuePair& pair_of(Index index) {
    return Data[index];
  }

  [[nodsicard]] Key& value_of(Index index) {
    return Data[index].Value;
  }

#pragma endregion

#pragma region Find

  Index find_or_default(Key key) const { // linear
    Index beginning = hash(key);
    Index index = beginning;
    
    for (; index < Capacity; index++) {
      const Key& otherKey = Data[index].Key;
      if (otherKey == Key{} || key == otherKey)
        return index + 1;
    }

    index = 0;
    
    for (; index < beginning; index++) {
      const Key& otherKey = Data[index].Key;
      if (otherKey == Key{} || key == otherKey)
        return index + 1;
    }
    
    return 0;
  }

  // Requires that all keys between the original hash index and the final index are active.
  Index find_existing(Key key) const { // linear
    Index beginning = hash(key);
    Index index = beginning;
    
    for (; index < Capacity; index++)
      if (key == Data[index].Key)
        return index + 1;

    index = 0;
    
    for (; index < beginning; index++)
      if (key == Data[index].Key)
        return index + 1;
    
    return 0;
  }

#pragma endregion

#pragma region Set

  // Requires that all keys from [original hash index, final index) are active.
  void set(Key& key, Value& value) {
    value_of(find_existing(key)) = value;
  }

#pragma endregion

#pragma region Upsert

  void upsert(Key& key, Value& value) {
    Data[find_or_default(key)] = { key, value };
  }

#pragma endregion

#pragma endregion

};
