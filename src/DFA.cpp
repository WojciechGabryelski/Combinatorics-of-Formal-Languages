#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <string>
#include <list>
#include "DFA.h"
#include "MatrixInversion.h"

unsigned int DFA::getNumberOfStates() {
    std::queue<DFA*> remainingStates;
    std::queue<DFA*> states;
    remainingStates.push(this);
    while (!remainingStates.empty()) {
        DFA* tree = remainingStates.front();
        remainingStates.pop();
        if (!tree->explored) {
            tree->explored = true;
            states.push(tree);
            for (auto p : tree->transitions) {
                remainingStates.push(p.second);
            }
        }
    }
    unsigned int result = states.size();
    while (!states.empty()) {
        states.front()->explored = false;
        states.pop();
    }
    return result;
}

void DFA::setDepths() {
    std::queue<std::pair<DFA*, int>> remainingStates;
    remainingStates.emplace(this, 0);
    while (!remainingStates.empty()) {
        auto treeDepthPair = remainingStates.front();
        remainingStates.pop();
        DFA* tree = treeDepthPair.first;
        if (!tree->explored) {
            tree->index = treeDepthPair.second;
            tree->explored = true;
            for (auto p : tree->transitions) {
                remainingStates.emplace(p.second, tree->index + 1);
            }
        }
    }
}

void DFA::walk(std::string prefix, char transition, bool isLast, bool printChildren) {
    std::cout << prefix << (isLast ? "\\" : "|") << "-" << transition << "-(" << (this->acceptable ? "1" : "0") << ")" << this << "\n";
    if (printChildren) {
        this->explored = false;
        prefix += (isLast ? "  " : "| ");
        if (this->transitions.begin() != this->transitions.end()) {
            std::map<char, DFA*>::iterator it;
            for (it = this->transitions.begin(); it != --this->transitions.end(); ++it) {
                std::cout << prefix << "|\n";
                it->second->walk(prefix, it->first, false, it->second->index > this->index && it->second->explored);
            }
            std::cout << prefix << "|\n";
            it->second->walk(prefix, it->first, true, it->second->index > this->index && it->second->explored);
        }
    }
}

bool DFA::isExplored() const {
    return this->explored;
}

void DFA::setExplored(bool _explored) {
    this->explored = _explored;
}

bool DFA::isAcceptable() const {
    return this->acceptable;
}

void DFA::setAcceptable(bool _acceptable) {
    this->acceptable = _acceptable;
}

void DFA::print() {
    this->setDepths();
    this->walk();
}

bool DFA::regexMatch(const std::string& word) {
    DFA* state = this;
    for (char i : word) {
        auto it = state->transitions.find(i);
        if (it == state->transitions.end()) {
            return false;
        }
        state = it->second;
    }
    return state->acceptable;
}

RationalFunction<Rational<integer>> DFA::getGeneratingFunction() {
    int counter = 0;
    std::stack<DFA*> states;
    std::stack<DFA*> stateStack;
    std::vector<bool> v;
    stateStack.push(this);
    while (!stateStack.empty()) {
        DFA* state = stateStack.top();
        stateStack.pop();
        if (!state->explored) {
            state->explored = true;
            state->index = counter++;
            v.push_back(state->acceptable);
            states.push(state);
            for (auto p : state->transitions) {
                stateStack.push(p.second);
            }
        }
    }
    std::vector<std::vector<RationalFunction<Rational<integer>>>> a(counter, std::vector<RationalFunction<Rational<integer>>>(counter, RationalFunction<Rational<integer>>()));
    for (int i = 0; i < a.size(); ++i) {
        a[i][i] = RationalFunction<Rational<integer>>(1);
    }
    while (!states.empty()) {
        DFA* state = states.top();
        states.pop();
        state->explored = false;
        for (const auto &p : state->transitions) {
            a[state->index][p.second->index] -= RationalFunction<Rational<integer>>({(Rational<integer>) 0, (Rational<integer>) 1});
        }
    }
    a = MatrixInversion<RationalFunction<Rational<integer>>>::gaussianElimination(a);
    RationalFunction<Rational<integer>> result;
    for (int i = 0; i < a.size(); ++i) {
        if (v[i]) {
            result += a[0][i];
        }
    }
    return result;
}

DFA*& DFA::operator [] (char transition) {
    return this->transitions[transition];
}

DFA* DFA::minimize() {
    std::stack<DFA*> stateStack;
    std::vector<DFA*> states;
    states.push_back(new DFA());
    stateStack.push(this);
    int counter = 1;
    while (!stateStack.empty()) {
        DFA* state = stateStack.top();
        stateStack.pop();
        if (!state->explored) {
            state->explored = true;
            state->index = counter++;
            states.push_back(state);
            for (auto p : state->transitions) {
                stateStack.push(p.second);
            }
        }
    }
    bool** marked = new bool* [states.size()];
    auto** candidatesToMark = new std::list<std::pair<int, int>>* [states.size()];
    for (int i = 0; i < states.size(); ++i) {
        marked[i] = new bool [states.size() - i];
        candidatesToMark[i] = new std::list<std::pair<int, int>> [states.size() - i];
    }
    for (int i = 0; i < states.size(); ++i) {
        states[i]->explored = false;
        DFA*& q = states[i];
        for (int j = i; j < states.size(); ++j) {
            DFA*& r = states[j];
            marked[i][j - i] = q->acceptable != r->acceptable;
            candidatesToMark[i][j - i] = std::list<std::pair<int, int>>();
        }
    }

    for (int i = 0; i < states.size(); ++i) {
        DFA*& q = states[i];
        for (int j = i + 1; j < states.size(); ++j) {
            DFA*& r = states[j];
            if (q->acceptable == r->acceptable) {
                auto itQ = q->transitions.begin();
                auto itR = r->transitions.begin();
                bool bQ = itQ != q->transitions.end();
                bool bR = itR != r->transitions.end();
                while (bQ || bR) {
                    int i1, i2;
                    if (bQ && bR && itQ->first == itR->first) {
                        i1 = itQ->second->index;
                        i2 = itR->second->index;
                        if (i1 > i2) {
                            std::swap(i1, i2);
                        }
                        ++itQ;
                        ++itR;
                    } else if (bQ && (!bR || itQ->first < itR->first)) {
                        i1 = 0;
                        i2 = itQ->second->index;
                        ++itQ;
                    } else {
                        i1 = 0;
                        i2 = itR->second->index;
                        ++itR;
                    }
                    if (marked[i1][i2 - i1]) {
                        std::stack<std::pair<int, int>> pairsToMark;
                        pairsToMark.emplace(i, j);
                        while (!pairsToMark.empty()) {
                            auto p = pairsToMark.top();
                            pairsToMark.pop();
                            int s = p.first;
                            int t = p.second;
                            if (!marked[s][t - s]) {
                                marked[s][t - s] = true;
                                for (auto _p : candidatesToMark[s][t - s]) {
                                    pairsToMark.push(_p);
                                }
                            }
                        }
                        break;
                    }
                    bQ = itQ != q->transitions.end();
                    bR = itR != r->transitions.end();
                }
                if (!marked[i][j - i]) {
                    itQ = q->transitions.begin();
                    itR = r->transitions.begin();
                    bQ = itQ != q->transitions.end();
                    bR = itR != r->transitions.end();
                    while (bQ || bR) {
                        int i1, i2;
                        if (bQ && bR && itQ->first == itR->first) {
                            i1 = itQ->second->index;
                            i2 = itR->second->index;
                            if (i1 > i2) {
                                std::swap(i1, i2);
                            }
                            ++itQ;
                            ++itR;
                        }
                        else if (bQ && (!bR || itQ->first < itR->first)) {
                            i1 = 0;
                            i2 = itQ->second->index;
                            ++itQ;
                        }
                        else {
                            i1 = 0;
                            i2 = itR->second->index;
                            ++itR;
                        }
                        if (i1 != i2) {
                            candidatesToMark[i1][i2 - i1].emplace_back( i, j );
                        }
                        bQ = itQ != q->transitions.end();
                        bR = itR != r->transitions.end();
                    }
                }
            }
        }
    }

    int* connectedStates = new int[states.size()];
    for (int i = 0; i < states.size(); ++i) {
        connectedStates[i] = i;
    }
    for (int i = 0; i < states.size() - 1; ++i) {
        if (connectedStates[i] == i) {
            for (int j = i + 1; j < states.size(); ++j) {
                if (!marked[i][j - i]) {
                    connectedStates[j] = i;
                }
            }
        }
    }
    bool unnecessaryState = true;
    for (int i = 1; i < states.size(); ++i) {
        if (connectedStates[i] == i) {
            DFA*& q = states[i];
            for (auto& p : q->transitions) {
                int j = p.second->index;
                p.second = states[connectedStates[j]];
            }
        }
        if (connectedStates[i] == 0) {
            unnecessaryState = false;
        }
    }

    if (unnecessaryState) {
        delete states[0];
    }
    bool hasWords = marked[0][1];
    for (int i = 0; i < states.size(); ++i) {
        if (connectedStates[i] != i) {
            delete states[i];
        }
        delete[] marked[i];
        delete[] candidatesToMark[i];
    }
    delete[] marked;
    delete[] candidatesToMark;
    delete[] connectedStates;
    if (hasWords) {
        return this;
    }
    return states[0];
}