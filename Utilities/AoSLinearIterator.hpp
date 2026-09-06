#pragma once

template <typename T>
class AoSLinearIterator {
protected:

    T* Address;

public:

    AoSLinearIterator(T* address) : Address(address) {}

    T& operator*() const noexcept {
        return *Address;
    }

    T* operator->() const noexcept {
        return Address;
    }

    AoSLinearIterator<T>& operator++() noexcept {
        Address++;
        return *this;
    }

    AoSLinearIterator<T>& operator--() noexcept {
        Address--;
        return *this;
    }

    bool operator!=(const AoSLinearIterator<T>& other) const {
        return Address != other.Address;
    }

};
