#include <iostream>

#include "NFA.h"
#include "DFA.h"
#include "RationalFunction.h"
#include "ExtendedRationalFunction.h"

int main() {
    std::string regex;
    std::cout << "Podaj wyrażenie regularne: ";
    std::cin >> regex;

    NFA* nfa = NFA::regexToAutomaton(regex);
    if (nfa == nullptr) {
        std::cerr << "Wyrażenie regularne nie jest prawidłowe\n";
        return -1;
    }

//    std::cout << "NFA z \u03B5-przejściami:\n";
//    nfa->print();
    nfa->removeEpsilonTransitions();
 //   std::cout << "\n\nNFA bez \u03B5-przejść:\n";
//    nfa->print();
    DFA* dfa = nfa->toDFA();
//    std::cout << "\n\nDFA:\n";
//    dfa->print();
    dfa = dfa->minimize();
//    std::cout << "\n\nZminimalizowany DFA:\n";
//    dfa->print();

    RationalFunction<Rational<integer>> f = dfa->getGeneratingFunction();
    std::cout << "Funkcja tworząca:" << "\n";
    std::cout << f << "\n";
    std::cout << "Inna postać:\n";
    std::cout << ExtendedRationalFunction<integer>(f) << "\n";
    return 0;
}
