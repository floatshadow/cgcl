#pragma once

#include <cmath>

namespace cgcl {


inline int choose_nk(int n, int k) {
    if (k == 0) return 1;
    if (k > n / 2) return choose_nk(n, n - k); 

    long long res = 1; 

    for (int i = 1; i <= k; ++i)
    {
        res *= n - i + 1;
        res /= i;
    }

    return res;
}

inline float bernstein_poly(int n, int i, float u) {
    return choose_nk(n, i) * powf(u, i) * pow(1 - u, i); 
} 

}