#ifndef EXTENDED_RATIONAL_FUNCTION_H
#define EXTENDED_RATIONAL_FUNCTION_H

#include <iostream>
#include <list>
#include "Polynomial.h"
#include "RationalFunction.h"
#include "Rational.h"

template <typename T>
class ExtendedRationalFunction {
private:
    Polynomial<Rational<T>> rest;
    Polynomial<Rational<T>> numerator;
    std::list<std::pair<Polynomial<Rational<T>>, unsigned int>> denominator;
public:
    explicit ExtendedRationalFunction(RationalFunction<Rational<T>> f) {
        auto p = f.getNumerator().div(f.getDenominator());
        this->rest = p.first;
        this->numerator = p.second;
        this->denominator = this->decompose(f.getDenominator());
    }

    std::list<std::pair<Polynomial<Rational<T>>, unsigned int>> decompose(Polynomial<Rational<T>> a) {
        T _gcd = a[0].getDenominator();
        for (int i = 1; i <= a.degree(); ++i) {
            _gcd = Operators<T>::gcd(_gcd, a[i].getDenominator());
        }
        a *= Polynomial<Rational<T>>({(Rational<T>)_gcd});
        T a_0 = a[0].getNumerator();
        T a_n = a[a.degree()].getNumerator();
        auto a_0_decomposition = Operators<T>::primeFactorsDecomposition(a_0);
        auto a_n_decomposition = Operators<T>::primeFactorsDecomposition(a_n);
        auto it_0 = a_0_decomposition.begin();
        auto it_n = a_n_decomposition.begin();
        std::list<Rational<T>> candidates;
        std::list<Rational<T>> help;
        candidates.push_back((Rational<T>) (T) 1);
        while (it_0 != a_0_decomposition.end() && it_n != a_n_decomposition.end()) {
            if ((*it_0).first <= (*it_n).first) {
                for (auto candidate : candidates) {
                    for (unsigned int i = 1; i <= (*it_0).second; ++i) {
                        help.push_back(candidate);
                        candidate *= (Rational<T>) (*it_0).first;
                    }
                }
                ++it_0;
            }
            if ((*it_0).first >= (*it_n).first) {
                for (auto candidate : candidates) {
                    for (unsigned int i = 1; i <= (*it_n).second; ++i) {
                        help.push_back(candidate);
                        candidate /= (Rational<T>) (*it_n).first;
                    }
                }
                ++it_n;
            }
            for (auto candidate : candidates) {
                help.push_back(candidate);
            }
            candidates = help;
            help.clear();
        }
        while (it_0 != a_0_decomposition.end()) {
            for (auto candidate : candidates) {
                for (unsigned int i = 0; i <= (*it_0).second; ++i) {
                    help.push_back(candidate);
                    candidate *= (Rational<T>) (*it_0).first;
                }
            }
            candidates = help;
            ++it_0;
        }
        while (it_n != a_n_decomposition.end()) {
            for (auto candidate : candidates) {
                for (unsigned int i = 0; i <= (*it_n).second; ++i) {
                    help.push_back(candidate);
                    candidate /= (Rational<T>) (*it_n).first;
                }
            }
            candidates = help;
            ++it_n;
        }
        std::list<std::pair<Polynomial<Rational<T>>, unsigned int>> result;
        for (auto const &candidate : candidates) {
            if (a(candidate) == Rational<T>()) {
                std::pair<Polynomial<Rational<T>>, unsigned int> p;
                Polynomial<Rational<T>> b = Polynomial<Rational<T>>({Rational<T>(1), Rational<T>(-1) / candidate});
                p.first = b;
                p.second = 0;
                do {
                    a /= b;
                    ++p.second;
                } while (a(candidate) == Rational<T>());
                result.push_back(p);
            }
    }
        if (a.degree() > 0) {
            result.push_back({a, 1});
        } else {
            this->numerator /= a;
        }
        return result;
    }

    friend std::ostream& operator << (std::ostream &s, const ExtendedRationalFunction<T> &f) {
        if (f.rest != Polynomial<Rational<T>>()) {
            s << f.rest << "+";
        }
        s << "(" << f.numerator << ")/(";
        if (f.denominator.size() > 0) {
            for (auto const& p : f.denominator) {
                s << "(" << p.first << ")";
                if (p.second > 1) {
                    s << "^" << p.second;
                }
            }
        } else {
            std::cout << "1";
        }
        return s << ")";
    }
};

#endif //EXTENDED_RATIONAL_FUNCTION_H
