#pragma once

#include <array>

#include "Dynamic/DynamicStack.hpp"

template <typename T, std::unsigned_integral Address = uint16_t, unsigned char MinimumCharacter = ' ', unsigned char MaximumCharacter = '~'>
class DFA {
public:

  using State = std::array<Node, AlphabetSize>;

protected:

  constexpr static uint8_t AlphabetSize = MaximumCharacter - MinimumCharacter + 1;

  DynamicStack<State, Address> States;

public:

  DFA(Address initialCapacity = 5) : States(initialCapacity) {}

  Node* ensurePath(const unsigned char* key, size_t length) {
		if (length == 0) return nullptr;

		unsigned char character = key[0] - MinimumCharacter;
		if (character >= AlphabetSize) return nullptr;

		Address currentState = 0;
		Node* node = &States[currentState][character];

		for (size_t index = 1; index < length; ++index) {
			character = key[index] - MinimumCharacter;
			if (character >= AlphabetSize) return nullptr;

			Address nextState = node->nextState();

			if (nextState == 0) {
				nextState = static_cast<Address>(States.size() + 1);
        States.emplace();

				node = &States[currentState][key[index - 1] - MinimumCharacter]; // reacquire node after potential reallocation
				node->setNextState(nextState);
			}

			currentState = nextState;
			node = &States[currentState][character];
		}

		return node;
	}

  Node* ensurePath(const char* key, size_t length) {
		return ensurePath(reinterpret_cast<const unsigned char*>(key), length);
	}

  const Node* find(const unsigned char* key, size_t length) const {
		if (length == 0) return nullptr;

    unsigned char character = key[index] - MinimumCharacter;
		if (character >= AlphabetSize) return nullptr;
    
		const Node* node = &States[0][character];

		for (size_t index = 0; index < length; index++) {
			character = key[index] - MinimumCharacter;
			if (character >= AlphabetSize) return nullptr;

			Address nextState = node->nextState();
			if (nextState == 0) return nullptr;

			node = &States[nextState - 1][character];
		}

		return node;
	}

  const Node* find(const char* key, size_t length) const {
		return find(reinterpret_cast<const unsigned char*>(key), length);
	}

  bool get(const unsigned char* key, size_t length, Element& value) const {
		const Node* node = find(key, length);
		if (!node) return false;

		if (node->active()) {
			value = node->Value;
			return true;
		}

		return false;
	}

  bool get(const char* key, size_t length, Element& value) const {
		return get(reinterpret_cast<const unsigned char*>(key), length, value);
	}

  bool set(const unsigned char* key, size_t length, const Element& value) {
		Node* node = ensurePath(key, length);
		if (!node) return false;

		node->activate();
		node->Value = value;

		return true;
	}

	bool set(const char* key, size_t length, const Element& value) {
		return set(reinterpret_cast<const unsigned char*>(key), length, value);
	}

}
