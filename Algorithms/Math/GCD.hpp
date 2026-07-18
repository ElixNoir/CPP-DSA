#pragma once

#include <cstddef>

namespace Math {

    namespace GCD {

        size_t binary(size_t a, size_t b) {
            if (a == 0) return b;
            if (b == 0) return a;
        
            int shift = __builtin_ctz(a | b); // Common power of 2
        
            a >>= __builtin_ctz(a); // Remove trailing zeros
        
            do {
                b >>= __builtin_ctz(b); // Remove trailing zeros
        
                // Subtract lesser from greater
                if (a > b) {
                    size_t t = a;
                    a = b;
                    b = t;
                }
                
                b -= a;
            } while (b != 0);
        
            return a << shift; // Common power of 2 factor restoration
        }

        size_t euclidean(size_t a, size_t b) {
            while (b) {
                size_t t = a;
                a = b;
                b = t % b;
            }
            
            return a;
        }
    
    }

}
