#pragma once

template <typename T>
concept Number = std::integral<T> || std::floating_point<T>;

template <Number T>
class Vector3 {
protected:

  // simd some time

public:

  /*
    Vector3 cross() {

    }
  */

  /*
    Vector3 dot() {

    }
  */

  /*T magnitude() {
    return sqrt(X * X + Y * Y + Z * Z);
  }*/

  /*Vector3 normal() {
    
  }*/

  /*Vector3 project() {
    return (dot(a, b) / dot(b, b)) * b
  }*/

  /*double compare() {
    return dot(a, b) / (magnitude(a) * magnitude(b))
  }*/

};
