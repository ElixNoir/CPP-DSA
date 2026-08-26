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
template <
  typename Key,
  typename Value = void,
  bool DefaultKeyIsActive = false,
  Index(*hash)(Key),
  std::unsigned_integral Index = size_t,
  Allocator A = DefaultAllocator>
class DynamicOAInsertionOnlyHashMap : public DynamicIncrementalAoSContainer<KeyValuePair<Key, Value>> {
protected:

  [[no_unique_address]] std::conditional_t<DefaultKeyIsActive, DynamicBitArray, void> KeyStates;

public:

  using Base = DynamicIncrementalAoSContainer<KeyValuePair<Key, Value>>;

  DynamicOAInsertionOnlyHashMap(Index initialCapacity) : Base(initialCapacity) {}

  DynamicOAInsertionOnlyHashMap(DynamicAoSContainer& other) : Base(other) {}

  DynamicOAInsertionOnlyHashMap(DynamicAoSContainer&& other) : Base(other) {}

#pragma region Methods

#pragma region Helpers

  [[nodiscard]] bool can_insert_at(Index index, Key key) const {
    const Key& otherKey = Data[index].Key;
    if constexpr (DefaultKeyIsActive) {
      if (!KeyStates.get(index))
        return true;
    } else {
      if (key == Key{})
        return true;
    }
    
    return false;
  }

  [[nodiscard]] bool can_upsert_at(Index index, Key key) const {
    const Key& otherKey = Data[index].Key;
    if constexpr (DefaultKeyIsActive) {
      if (!KeyStates.get(index) || key == otherKey)
        return true;
    } else {
      if (key == Key{} || key == otherKey)
        return true;
    }
    
    return false;
  }

#pragma endregion

#pragma region Getters

  //. If the key is changed, it is only legal to do so if the corresponding hash is not changed.
  [[nodsicard]] constexpr Key& key_of(Index index) const {
    return Data[index].Key;
  }

  [[nodsicard]] constexpr KeyValuePair& pair_of(Index index) const {
    return Data[index];
  }

  [[nodsicard]] constexpr Key& value_of(Index index) const {
    return Data[index].Value;
  }

#pragma endregion

#pragma region Memory Management

  void double_capacity_to_prime() {
    // find next prime after doubled capacity
    // grow(prime);
  }

  void grow(Index newCapacity) {
    if constexpr (ReallocatableAllocator<A>) {
      Data = static_cast<T*>(Alloc.reallocate(Data, sizeof(T) * newCapacity));
      rehash();
    } else {
      T* newData = static_cast<T*>(Alloc.allocate(sizeof(T) * newCapacity));
      rehash();
      Alloc.deallocate(Data);
      Data = newData;
    }
    
    Capacity = newCapacity;
  }

  void rehash() {
    // iterate through pairs and re-insert them.
    
  }

#pragma endregion

#pragma region Find

  // It is highly recommended that all keys between the original hash index and the actual corresponding index are active.
  Index find_existing(Key& key) const { // linear
    Index beginning = hash(key) % Capacity;
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

  Index find_inactive(Key& key) const { // linear
    Index beginning = hash(key) % Capacity;
    Index index = beginning;
    
    for (; index < Capacity; index++)
      if (can_insert_at(index))
        return index + 1;

    index = 0;
    
    for (; index < beginning; index++)
      if (can_insert_at(index))
        return index + 1;
    
    return 0;
  }

  Index find_or_default(Key& key) const { // linear
    Index beginning = hash(key) % Capacity;
    Index index = beginning;
    
    for (; index < Capacity; index++)
      if (can_upsert_at(index, key))
        return index + 1;

    index = 0;
    
    for (; index < beginning; index++)
      if (can_upsert_at(index, key))
        return index + 1;
    
    return 0;
  }

#pragma endregion

#pragma region Set

  // Requires that all keys from [original hash index, final index) are active.
  void set(Key& key, Value& value) {
    set(find_existing(key), value);
  }

  void set(Index index, Value& value) {
    value_of(index - 1) = value;
  }

#pragma endregion

#pragma region Upsert

  void insert(Key& key, Value& value) {
    Data[find_inactive(key)] = { key, value };
  }

  void upsert(Key& key, Value& value) {
    Data[find_or_default(key)] = { key, value };
  }

#pragma endregion

#pragma endregion

};
