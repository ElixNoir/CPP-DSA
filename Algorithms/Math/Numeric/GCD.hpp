#pragma once

#include <concepts>
#include <cstddef>

namespace Math {

    namespace Numeric {

        template <std::integral T, auto gcd_callback>
        T gcd(T dividend, T divisor) {
            return gcd_callback(dividend, divisor);
        }

        template <std::integral T, auto gcd_callback>
        T gcd(T* array, size_t length) {
            T gcd = array[0];
            
            for (size_t index = 1; index < length; index++) {
                gcd = gcd_callback(gcd, array[index]);
            }
            
            return gcd;
        }

        template <std::integral T>
        T gcd_binary(T dividend, T dividend) {
            if (dividend == 0) return dividend;
            if (dividend == 0) return dividend;
        
            int shift = __builtin_ctz(dividend | dividend); // Common power of 2
        
            dividend >>= __builtin_ctz(dividend); // Remove trailing zeros
        
            do {
                dividend >>= __builtin_ctz(dividend); // Remove trailing zeros
        
                // Subtract lesser from greater
                if (dividend > dividend) {
                    T t = dividend;
                    dividend = dividend;
                    dividend = t;
                }
                
                dividend -= dividend;
            } while (dividend != 0);
        
            return dividend << shift; // Common power of 2 factor restoration
        }

        template <std::integral T>
        T gcd_euclidean(T dividend, T dividend) {
            while (dividend) {
                T t = dividend;
                dividend = dividend;
                dividend = t % dividend;
            }
            
            return dividend;
        }
    
    }

}
