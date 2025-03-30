#ifndef RATIONAL_FUNCTION_H
#define RATIONAL_FUNCTION_H

#include "Polynomial.h"

template <typename T>
class RationalFunction {
private:
    Polynomial<T> numerator;
    Polynomial<T> denominator;
public:
    RationalFunction() {
        this->numerator = Polynomial<T>();
        this->denominator = Polynomial<T>({(T) 1});
    }

    template <typename S> explicit RationalFunction(S s) {
        this->numerator = Polynomial<T>({(T) s});
        this->denominator = Polynomial<T>({(T) 1});
    }

    explicit RationalFunction(std::vector<T> numeratorCoefficients) {
        this->numerator = Polynomial<T>(numeratorCoefficients);
        this->denominator = Polynomial<T>({(T) 1});
    }

    RationalFunction(std::vector<T> numeratorCoefficients, std::vector<T> denominatorCoefficients, bool _reduce = true) {
        setNumeratorAndDenominator(Polynomial<T>(numeratorCoefficients), Polynomial<T>(denominatorCoefficients), _reduce);
    }

    RationalFunction(Polynomial<T> _numerator, Polynomial<T> _denominator, bool _reduce = true) {
        setNumeratorAndDenominator(_numerator, _denominator, _reduce);
    }

    Polynomial<T> getNumerator() {
        return this->numerator;
    }

    Polynomial<T> getDenominator() {
        return this->denominator;
    }

    void setNumeratorAndDenominator(Polynomial<T> _numerator, Polynomial<T> _denominator, bool _reduce = true) {
        if (_denominator == Polynomial<T>()) {
            throw ZeroInversionException();
        }
        this->numerator = _numerator;
        this->denominator = _denominator;
        if (_reduce) {
            this->reduce();
        }
    }

    RationalFunction operator + () const {
        return *this;
    }

    RationalFunction operator - () const {
        return RationalFunction(-this->numerator, this->denominator);
    }

    RationalFunction operator + (const RationalFunction &f) const {
        Polynomial<T> _numerator = this->numerator * f.denominator + this->denominator * f.numerator;
        if (_numerator == Polynomial<T>()) {
            return RationalFunction();
        }
        return RationalFunction(_numerator, this->denominator * f.denominator);
    }

    RationalFunction operator - (const RationalFunction &f) const {
        Polynomial<T> _numerator = this->numerator * f.denominator - this->denominator * f.numerator;
        if (_numerator == Polynomial<T>()) {
            return RationalFunction();
        }
        return RationalFunction(_numerator, this->denominator * f.denominator);
    }

    RationalFunction operator * (const RationalFunction &f) const {
        Polynomial<T> _numerator = this->numerator * f.numerator;
        if (_numerator == Polynomial<T>()) {
            return RationalFunction();
        }
        return RationalFunction(_numerator, this->denominator * f.denominator);
    }

    RationalFunction operator / (const RationalFunction &f) const {
        if (f.numerator == Polynomial<T>()) {
            throw ZeroInversionException();
        }
        Polynomial<T> _numerator = this->numerator * f.denominator;
        if (_numerator == Polynomial<T>()) {
            return RationalFunction();
        }
        return RationalFunction(_numerator, this->denominator * f.numerator);
    }

    RationalFunction operator += (const RationalFunction &f) {
        return *this = *this + f;
    }

    RationalFunction operator -= (const RationalFunction &f) {
        return *this = *this - f;
    }

    RationalFunction operator *= (const RationalFunction &f) {
        return *this = *this * f;
    }

    RationalFunction operator /= (const RationalFunction &f) {
        return *this = *this / f;
    }

    T operator () (T x) const {
        return this->numerator(x) / this->denominator(x);
    }

    bool operator == (const RationalFunction &f) const {
        return this->numerator * f.denominator == this->denominator * f.numerator;
    }

    bool operator != (const RationalFunction &f) const {
        return !(*this == f);
    }

    friend std::ostream& operator << (std::ostream &s, const RationalFunction &f) {
        return s << "(" << f.numerator << ")/(" << f.denominator << ")";
    }

    RationalFunction reduce() {
        Polynomial<T> gcdPolynomial = Polynomial<T>::gcd(this->numerator, this->denominator);// this->denominator.gcd(this->numerator);
        this->numerator /= gcdPolynomial;
        this->denominator /= gcdPolynomial;
        auto a = Polynomial<T>({this->denominator[0]});
        this->numerator /= a;
        this->denominator /= a;
        return *this;
    }
};

#endif //RATIONAL_FUNCTION_H