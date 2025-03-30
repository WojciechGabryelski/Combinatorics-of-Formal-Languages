#ifndef RATIONAL_H
#define RATIONAL_H

#include <iostream>
#include "Operators.h"
#include "ZeroInversionException.h"

template <typename T>
class Rational {
private:
    T numerator;
    T denominator;
public:
    Rational() {
        this->numerator = (T) 0;
        this->denominator = (T) 1;
    }

    explicit Rational(T _numerator) {
        this->numerator = _numerator;
        this->denominator = (T) 1;
    }

    Rational(T _numerator, T _denominator, bool _reduce = true) {
        this->setNumeratorAndDenominator(_numerator, _denominator, _reduce);
    }

    void setNumeratorAndDenominator(T _numerator, T _denominator, bool _reduce = true) {
        this->numerator = _numerator;
        this->denominator = _denominator;
        if (_reduce) {
            T _gcd = Operators<T>::gcd(_numerator, _denominator);
            this->numerator /= _gcd;
            this->denominator /= _gcd;
        }
        if (this->denominator < (T) 0) {
            this->numerator = -this->numerator;
            this->denominator = -this->denominator;
        }
    }

    T getNumerator() {
        return this->numerator;
    }

    T getDenominator() {
        return this->denominator;
    }

    Rational operator + () const {
        return *this;
    }

    Rational operator - () const {
        return Rational(-this->numerator, this->denominator);
    }

    Rational operator + (const Rational &a) const {
        T _gcd = Operators<T>::gcd(this->denominator, a.denominator);
        return Rational(this->numerator * (a.denominator / _gcd) + a.numerator * (this->denominator / _gcd), this->denominator / _gcd * a.denominator);
    }

    Rational operator - (const Rational &a) const {
        T _gcd = Operators<T>::gcd(this->denominator, a.denominator);
        return Rational(this->numerator * (a.denominator / _gcd) - a.numerator * (this->denominator / _gcd), this->denominator / _gcd * a.denominator);
    }

    Rational operator * (const Rational &a) const {
        T _gcd1 = Operators<T>::gcd(this->numerator, a.denominator);
        T _gcd2 = Operators<T>::gcd(a.numerator, this->denominator);
        return Rational((this->numerator / _gcd1) * (a.numerator / _gcd2), (this->denominator / _gcd2) * (a.denominator / _gcd1), false);
    }

    Rational operator / (const Rational &a) const {
        if (a.numerator == (T) 0) {
            throw ZeroInversionException();
        }
        return *this * Rational(a.denominator, a.numerator, false);
    }

    Rational operator += (const Rational &a) {
        return *this = *this + a;
    }

    Rational operator -= (const Rational &a) {
        return *this = *this - a;
    }

    Rational operator *= (const Rational &a) {
        return *this = *this * a;
    }

    Rational operator /= (const Rational &a) {
        return *this = *this / a;
    }

    bool operator == (const Rational &a) const {
        return this->numerator * a.denominator == a.numerator * this->denominator;
    }

    bool operator != (const Rational &a) const {
        return !(*this == a);
    }

    bool operator > (const Rational &a) const {
        return this->numerator * a.denominator > a.numerator * this->denominator;
    }

    bool operator < (const Rational &a) const {
        return this->numerator * a.denominator < a.numerator * this->denominator;
    }

    bool operator >= (const Rational &a) const {
        return this->numerator * a.denominator >= a.numerator * this->denominator;
    }

    bool operator <= (const Rational &a) const {
        return this->numerator * a.denominator <= a.numerator * this->denominator;
    }

    friend std::ostream& operator << (std::ostream &s, const Rational &a) {
        if (a.denominator == (T) 1) {
            return s << a.numerator;
        }
        return s << a.numerator << "/" << a.denominator;
    }
};

#endif //RATIONAL_H
