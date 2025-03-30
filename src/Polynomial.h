#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include <iostream>
#include <vector>
#include "ZeroInversionException.h"

template <typename T>
class Polynomial {
private:
    std::vector<T> coefficients;
public:
    Polynomial() = default;

    explicit Polynomial(std::vector<T> _coef) {
        this->setCoefficients(_coef);
    }

    std::vector<T> getCoefficients() const {
        return this->coefficients;
    }

    void setCoefficients(std::vector<T> _coef) {
        this->coefficients = _coef;
        while (!this->coefficients.empty() && this->coefficients.back() == (T)0) {
            this->coefficients.pop_back();
        }
    }

    void setCoefficient(unsigned int i, T a) {
        if (i < coefficients.size()) {
            if (i == coefficients.size() - 1 && a == (T)0) {
                coefficients.pop_back();
            } else {
                coefficients[i] = a;
            }
        }
    }

    unsigned int degree() const {
        return this->coefficients.size() - 1;
    }

    Polynomial operator + () const {
        return *this;
    }

    Polynomial operator - () const {
        std::vector<T> v(this->coefficients.size());
        for (int i = 0; i < this->coefficients.size(); ++i) {
            v[i] = -this->coefficients[i];
        }
        return Polynomial(v);
    }

    Polynomial operator + (const Polynomial &a) const {
        int n = this->coefficients.size();
        int m = a.coefficients.size();
        std::vector<T> v(std::max(n, m));
        if (n < m) {
            std::swap(n, m);
            for (int i = m; i < n; ++i) {
                v[i] = a.coefficients[i];
            }
        } else {
            for (int i = m; i < n; ++i) {
                v[i] = this->coefficients[i];
            }
        }

        for (int i = 0; i < m; ++i) {
            v[i] = this->coefficients[i] + a.coefficients[i];
        }
        return Polynomial(v);
    }

    Polynomial operator - (const Polynomial &a) const {
        int n = this->coefficients.size();
        int m = a.coefficients.size();
        std::vector<T> v(std::max(n, m));
        if (n < m) {
            std::swap(n, m);
            for (int i = m; i < n; ++i) {
                v[i] = -a.coefficients[i];
            }
        } else {
            for (int i = m; i < n; ++i) {
                v[i] = this->coefficients[i];
            }
        }

        for (int i = 0; i < m; ++i) {
            v[i] = this->coefficients[i] - a.coefficients[i];
        }
        return Polynomial(v);
    }

    Polynomial operator * (const Polynomial &a) const {
        int n = this->coefficients.size();
        int m = a.coefficients.size();
        if (n == 0 || m == 0) {
            return Polynomial();
        }
        std::vector<T> v(m + n - 1);
        for (int i = 0; i < m + n - 1; ++i) {
            v[i] = (T) 0;
        }
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                v[i + j] += this->coefficients[i] * a.coefficients[j];
            }
        }
        return Polynomial(v);
    }

    std::pair<Polynomial, Polynomial> div (const Polynomial &a) const {
        int n = this->coefficients.size();
        int m = a.coefficients.size();
        if (m == 0) {
            throw ZeroInversionException();
        }
        std::vector<T> reminder = this->coefficients;
        if (m > n) {
            return {Polynomial(), Polynomial(reminder)};
        }
        std::vector<T> quotient = std::vector<T>(n - m + 1);
        for (int i = n - 1; i >= m - 1; --i) {
            T b = reminder[i] / (a.coefficients)[m - 1];
            quotient[i - m + 1] = b;
            for (int j = 1; j < m; ++j) {
                reminder[i - j] -= b * (a.coefficients)[m - j - 1];
            }
            reminder.pop_back();
        }
        return {Polynomial(quotient), Polynomial(reminder)};
    }

    Polynomial operator / (const Polynomial &a) const {
        return this->div(a).first;
    }

    Polynomial operator % (const Polynomial &a) const {
        return this->div(a).second;
    }

    Polynomial operator += (const Polynomial &f) {
        return *this = *this + f;
    }

    Polynomial operator -= (const Polynomial &f) {
        return *this = *this - f;
    }

    Polynomial operator *= (const Polynomial &f) {
        return *this = *this * f;
    }

    Polynomial operator /= (const Polynomial &f) {
        return *this = *this / f;
    }

    Polynomial operator %= (const Polynomial &f) {
        return *this = *this % f;
    }

    T operator () (T x) const {
        T result = (T) 0;
        for (int i = this->coefficients.size() - 1; i >= 0; --i) {
            result *= x;
            result += this->coefficients[i];
        }
        return result;
    }

    T operator [] (unsigned int n) const {
        if (n > this->coefficients.size()) {
            return (T) 0;
        }
        return this->coefficients[n];
    }

    bool operator == (const Polynomial &a) const {
        if ((this -> coefficients).size() != a.coefficients.size()) {
            return false;
        }
        for (int i = 0; i < a.coefficients.size(); i++) {
            if (this->coefficients[i] != a.coefficients[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator != (const Polynomial &a) const {
        return !(*this == a);
    }

    friend std::ostream& operator << (std::ostream &s, const Polynomial &a) {
        if (a.coefficients.size() > 0) {
            if (a.coefficients[0] != (T) 0) {
                s << a.coefficients[0];
            }
            if (a.coefficients.size() > 1) {
                if (a.coefficients[1] != (T) 0) {
                    if (a.coefficients[0] != (T) 0 && a.coefficients[1] > (T) 0) {
                        s << "+";
                    }
                    if (a.coefficients[1] != (T) 1) {
                        if (a.coefficients[1] != (T) -1) {
                            s << a.coefficients[1] << "*";
                        } else {
                            s << "-";
                        }
                    }
                    s << "x";
                }
                if (a.coefficients.size() > 2) {
                    bool b = a.coefficients[0] != (T) 0 || a.coefficients[1] != (T) 0;
                    for (int i = 2; i < a.coefficients.size(); ++i) {
                        if (a.coefficients[i] != (T) 0) {
                            if (b && a.coefficients[i] > (T) 0) {
                                s << "+";
                            }
                            if (a.coefficients[i] != (T) 1) {
                                if (a.coefficients[i] != (T) -1) {
                                    s << a.coefficients[i] << "*";
                                } else {
                                    s << "-";
                                }
                            }
                            s << "x^" << i;
                            b = true;
                        }
                    }
                }
            }
            return s;
        }
        return s << (T) 0;
    }

    static Polynomial gcd(Polynomial a, Polynomial b) {
        if (a != Polynomial()) {
            a /= Polynomial({a.coefficients.back()});
        }
        while (b != Polynomial()) {
            b /= Polynomial({b.coefficients.back()});
            a %= b;
            std::swap(a, b);
        }
        return a;
    }
};

#endif //POLYNOMIAL_H