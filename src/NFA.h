#ifndef NFA_H
#define NFA_H

#include <map>
#include <set>
#include <list>
#include <string>
#include "DFA.h"

class NFA {
private:
    std::map<char, std::set<NFA*>> transitions;
    std::set<std::set<NFA*>*> parents;
    std::list<NFA*> epsilonParents;
    bool acceptable;
    bool explored = false;
    int index = 0;
    NFA* add(NFA* tree);
    NFA* concatenate(NFA* tree);
    NFA* cycle();
    void findEpsilonClosures();
    void setDepths();
    void walk(std::string prefix = "", bool isLast = true, bool printChildren = true);
public:
    static const bool ADDITION;
    static const bool CONCATENATION;
    explicit NFA(bool acceptable = true);
    explicit NFA(char transition);
    unsigned int getNumberOfStates();
    void print();
    NFA* removeEpsilonTransitions();
    std::set<NFA*>& operator [] (char transition);
    static bool isValidRegex(std::string regex);
    static NFA* regexToAutomaton(std::string regex);
    DFA* toDFA();
};

#endif //NFA_H