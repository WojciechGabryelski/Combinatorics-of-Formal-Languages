#ifndef DFA_H
#define DFA_H

#include <string>
#include <map>
#include "RationalFunction.h"
#include "Rational.h"
#include <gmpxx.h>

typedef mpz_class integer;

class DFA {
private:
    std::map<char, DFA*> transitions;
    bool acceptable = false;
    bool explored = false;
    int index = 0;
    void setDepths();
    void walk(std::string prefix = "", char transition = '\0', bool isLast = true, bool printChildren = true);
public:
    bool isExplored() const;
    void setExplored(bool explored);
    bool isAcceptable() const;
    void setAcceptable(bool acceptable);
    unsigned int getNumberOfStates();
    void print();
    bool regexMatch(const std::string& word);
    DFA*& operator [] (char transition);
    RationalFunction<Rational<integer>> getGeneratingFunction();
    DFA* minimize();
};

#endif //DFA_H