#pragma once

#pragma region Dependencies

#include "DefaultAllocator.hpp"
#include "DSAConcepts.hpp"

#pragma endregion

template <Allocator A = DefaultAllocator>
class DynamicSplayTree {
public:

  struct Node {

    Index Left;
    Index Right;
    Index Parent;
    T Key;

  };

protected:

  [[no_unique_address]] A Alloc;

  Node* Data;
  Index Capacity;
  Index Size = 0;
  Index Root = 0;
  Index FreeRoot = 0;

#pragma region Modification

  void rotate(Index currentIndex) {
    Index parent = Data[currentIndex - 1].Parent;
    if (parent == 0) return;
  
    Index grandParent = Data[parent].Parent;
  
    if (currentIndex == Data[parent].Left) {
      Index rightIndex = Data[currentIndex].Right;
      Data[parent].Left = rightIndex;
      if (rightIndex != 0) Data[rightIndex].Parent = parent;
      Data[currentIndex].Right = parent;
    } else {
      Index leftIndex = Data[currentIndex].Left;
      Data[parent].Right = leftIndex;
      if (leftIndex != 0) Data[leftIndex].Parent = parent;
      Data[currentIndex].Left = parent;
    }
  
    Data[parent].Parent = currentIndex;
    Data[currentIndex].Parent = grandParent;
  
    if (grandParent != 0) {
      if (parent == Data[grandParent].Left) Data[gradParent].Left = currentIndex;
      else Data[gradParent].Right = currentIndex;
    } else Root = currentIndex;
  }

  void splay(Index currentIndex) {
    Index parent = Data[currentIndex].Parent;
    while (parent != 0) {
      Index grandParent = Data[parent].Parent;
      if (grandParent == 0) rotate(currentIndex); // Zig
      else if (parent == Data[grandParent].Left) == (currentIndex == Data[parent].Left) {
        rotate(parent);
        rotate(currentIndex);
      } else {
        rotate(currentIndex);
        rotate(parent);
      }
      
      parent = Data[currentIndex].Parent;
    }
  }

#pragma endregion

public:

  DynamicSplayTree(Index initialCapaity) : Capacity(initialCapacity) {
    Data = static_cast<Node*>(Alloc.allocate(initialCapacity * sizeof(Node)));
  }

  ~DynamicSplayTree() {
    Alloc.deallocate(Data);
  }

#pragma region Memory Management

#pragma endregion

#pragma region Search

  [[nodiscard]] Index find(const T& key) {
    Index currentIndex = Root;
    Index lastIndex = 0;
  
    while (currentIndex != 0) {
      const T& currentKey = self[current]
      lastIndex = currentIndex;
      if (key == currentKey) {
        splay(currentIndex);
        return current;
      } else if (key < currentKey) currentIndex = Data[currentIndex].Left;
      else currentIndex = Data[currentIndex].Right;
    }
  
    if (lastIndex != 0) splay(lastIndex);

    return 0;
  }

#pragma endregion

};

/*function Module.remove(self: SplayTree, key: any): false | number
  local address = self:find(key)
  if not address then
    return false
  end

  local left = self[address + LEFT]
  local right = self[address + RIGHT]

  if not left then
    self.Root = right
    if right then
      self[right + PARENT] = false
    end
  else
    self[left + PARENT] = false
    local m = left
    while self[m + RIGHT] do
      m = self[m + RIGHT]
    end
    splay(self, m)
    self[m + RIGHT] = right
    if right then
      self[right + PARENT] = m
    end
    self.Root = m
  end

  free(self, address)
  
  return address
end

function Module.upsert(self: SplayTree, key: any): false | number
  local current = self.Root
  if not current then
    local address = allocate(self, key, false)
    self.Root = address
    return address
  end
  
  while true do
    local currentKey = self[current]
    if key == currentKey then
      return false
    elseif key < currentKey then
      local left = self[current + LEFT]
      if not left then
        local address = allocate(self, key, current)
        self[current + LEFT] = address
        splay(self, address)
        return address
      end
      current = left
    else
      local right = self[current + RIGHT]
      if not right then
        local address = allocate(self, key, current)
        self[current + RIGHT] = address
        splay(self, address)
        return address
      end
      current = right
    end
  end
end
*/
