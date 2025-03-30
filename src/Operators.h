#ifndef OPERATORS_H
#define OPERATORS_H

#include <list>

template <typename T>
class Operators {
public:
    static T gcd(T a, T b) {
        while (b != (T) 0) {
            a %= b;
            T c = a;
            a = b;
            b = c;
        }
        return a;
    }

    static std::list<std::pair<T, unsigned int>> primeFactorsDecomposition(T a) {
        if (a < (T) 0) {
            a = -a;
        }
        T i = (T) 2;
        std::list<std::pair<T, unsigned int>> decomposition;
        while (a != (T) 1) {
            if (a % i == (T) 0) {
                std::pair<T, unsigned int> p = {i, 0};
                do {
                    a /= i;
                    ++p.second;
                } while (a % i == 0);
                decomposition.push_back(p);
            }
            ++i;
        }
        return decomposition;
    }
};

#endif //OPERATORS_H
