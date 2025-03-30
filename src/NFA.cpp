#include <iostream>
#include <stack>
#include <queue>
#include "NFA.h"
#include "PtrMap.h"

NFA* NFA::add(NFA* tree) {
    NFA* state = new NFA(false);
    std::set<NFA*> &s = state->transitions['\0'];
    s.insert(this);
    this->parents.insert(&s);
    s.insert(tree);
    tree->parents.insert(&s);
    return state;
}

NFA* NFA::concatenate(NFA* tree) {
    std::stack<NFA*> states;
    std::stack<NFA*> stateStack;
    NFA* root = this;
    stateStack.push(this);
    if (this != tree) {
        tree->explored = true;
    }
    while (!stateStack.empty()) {
        NFA* state = stateStack.top();
        stateStack.pop();
        if (!state->explored) {
            state->explored = true;
            states.push(state);
            for (const auto& p : state->transitions) {
                for (NFA* q : p.second) {
                    stateStack.push(q);
                }
            }
            if (state->acceptable) {
                if (state->transitions.empty()) {
                    for (auto s : state->parents) {
                        s->erase(state);
                        s->insert(tree);
                        tree->parents.insert(s);
                    }
                    states.pop();
                    if (state == this) {
                        root = tree;
                    }
                    delete state;
                } else {
                    state->acceptable = false;
                    std::set<NFA*> &s = state->transitions['\0'];
                    s.insert(tree);
                    tree->parents.insert(&s);
                }
            }
        }
    }
    while (!states.empty()) {
        NFA* state = states.top();
        states.pop();
        state->explored = false;
    }
    tree->explored = false;
    return root;
}

NFA* NFA::cycle() {
    this->acceptable = false;
    this->concatenate(this);
    this->acceptable = true;
    return this;
}

void NFA::findEpsilonClosures() {
    if (this->explored) {
        this->explored = false;
        std::set<NFA*> &epsChildren = this->transitions['\0'];
        std::stack<std::set<NFA*>*> listsToAdd;
        for (NFA* state : epsChildren) {
            state->findEpsilonClosures();
            listsToAdd.push(&state->transitions['\0']);
        }
        while (!listsToAdd.empty()) {
            std::set<NFA*> &s = *listsToAdd.top();
            listsToAdd.pop();
            for (NFA* q : s) {
                epsChildren.insert(q);
            }
        }
        epsChildren.insert(this);
    }
}

unsigned int NFA::getNumberOfStates() {
    std::queue<NFA*> remainingStates;
    remainingStates.push(this);
    std::queue<NFA*> states;
    while (!remainingStates.empty()) {
        NFA* tree = remainingStates.front();
        remainingStates.pop();
        if (!tree->explored) {
            tree->explored = true;
            states.push(tree);
            for (auto it = tree->transitions.begin(); it != tree->transitions.end(); ++it) {
                for (NFA* state : it->second) {
                    remainingStates.push(state);
                }
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

void NFA::setDepths() {
    std::queue<std::pair<NFA*, int>> remainingStates;
    remainingStates.emplace(this, 0);
    while (!remainingStates.empty()) {
        auto treeDepthPair = remainingStates.front();
        remainingStates.pop();
        NFA* tree = treeDepthPair.first;
        if (!tree->explored) {
            tree->index = treeDepthPair.second;
            tree->explored = true;
            for (auto it = tree->transitions.begin(); it != tree->transitions.end(); ++it) {
                for (NFA* state : it->second) {
                    remainingStates.emplace(state, tree->index + 1);
                }
            }
        }
    }
}

void NFA::walk(std::string prefix, bool isLast, bool printChildren) {
    std::cout << prefix << (isLast ? "\\" : "|") << "-" << "-(" << (this->acceptable ? "1" : "0") << ")" << this << "\n";
    if (printChildren) {
        this->explored = false;
        prefix += (isLast ? "  " : "| ");
        if (this->transitions.begin() != this->transitions.end()) {
            std::map<char, std::set<NFA*>>::iterator it;
            std::set<NFA*>::iterator it2;
            for (it = this->transitions.begin(); it != --this->transitions.end(); ++it) {
                std::cout << prefix << "|\n";
                std::cout << prefix << "|-" << (it->first  == '\0' ? "\u03B5" : std::string(1, it->first)) << "\\\n";
                for (it2 = it->second.begin(); it2 != --it->second.end(); ++it2) {
                    std::cout << prefix << "|  |\n";
                    (*it2)->walk(prefix + "|  ", false, (*it2)->index > this->index && (*it2)->explored);
                }
                std::cout << prefix << "|  |\n";
                (*it2)->walk(prefix + "|  ", true, (*it2)->index > this->index && (*it2)->explored);
            }
            std::cout << prefix << "|\n";
            std::cout << prefix << "\\-" << (it->first  == '\0' ? "\u03B5" : std::string(1, it->first)) << "\\\n";
            for (it2 = it->second.begin(); it2 != --it->second.end(); ++it2) {
                std::cout << prefix << "   |\n";
                (*it2)->walk(prefix + "   ", false, (*it2)->index > this->index && (*it2)->explored);
            }
            std::cout << prefix << "   |\n";
            (*it2)->walk(prefix + "   ", true, (*it2)->index > this->index && (*it2)->explored);
        }
    }
}

const bool NFA::ADDITION = false;
const bool NFA::CONCATENATION = true;

NFA::NFA(bool acceptable) {
    this->acceptable = acceptable;
}

NFA::NFA(char transition) {
    this->acceptable = false;
    NFA* state = new NFA();
    std::set<NFA*> &s = this->transitions[transition];
    s.insert(state);
    state->parents.insert(&s);
}

NFA* NFA::removeEpsilonTransitions() {
    // find the set of all states
    std::list<NFA*> states;
    std::stack<NFA*> stateStack;
    stateStack.push(this);
    while (!stateStack.empty()) {
        NFA* state = stateStack.top();
        stateStack.pop();
        if (!state->explored) {
            state->explored = true;
            state->epsilonParents.clear();
            states.push_back(state);
            for (const auto& p : state->transitions) {
                for (NFA* q : p.second) {
                    stateStack.push(q);
                }
            }
        }
    }

    // DFS over the graph with epsilon transitions only and set reversed epsilon transitions
    std::stack<NFA*> ordered;
	std::stack<NFA*> orderedHelp;
    for (NFA* state : states) {
        if (state->explored) {
            stateStack.push(state);
            while (!stateStack.empty()) {
                NFA* q = stateStack.top();
                stateStack.pop();
                if (q->explored) {
                    q->explored = false;
                    orderedHelp.push(q);
                    auto it = q->transitions.find('\0');
                    if (it != q->transitions.end()) {
                        for (NFA* p : it->second) {
                            stateStack.push(p);
                            p->epsilonParents.push_back(q);
                        }
                    }
                }
            }
			while (!orderedHelp.empty()) {
				ordered.push(orderedHelp.top());
				orderedHelp.pop();
			}
        }
    }

    // find strongly connected components the graph with epsilon transitions only
    std::list<std::list<NFA*>> components;
    while (!ordered.empty()) {
        NFA* state = ordered.top();
        ordered.pop();
        if (!state->explored) {
            std::list<NFA*> component;
            stateStack.push(state);
            while (!stateStack.empty()) {
                NFA* q = stateStack.top();
                stateStack.pop();
                if (!q->explored) {
                    q->explored = true;
                    component.push_back(q);
                    for (NFA* p : q->epsilonParents) {
                        stateStack.push(p);
                    }
                }
            }
            components.push_back(component);
        }
    }

    // replace elements by their strongly connected components
    states.clear();
    for (std::list<NFA*> component : components) {
        NFA* groupedState = component.front();
        for (auto it = ++component.begin(); it != component.end(); ++it) {
            for (auto s : (*it)->parents) {
                s->erase(*it);
                s->insert(groupedState);
                groupedState->parents.insert(s);
            }
            for (const auto& p : (*it)->transitions) {
                std::set<NFA*> &s = groupedState->transitions[p.first];
                for (NFA* r : p.second) {
                    s.insert(r);
                    r->parents.insert(&s);
                }
            }
            delete *it;
        }
        states.push_back(groupedState);
    }

    for (NFA* state : states) {
        if (state->explored) {
            state->findEpsilonClosures();
        }
    }

    for (NFA* q : states) {
        auto it = q->transitions.find('\0');
        if (it != q->transitions.end()) {
            for (NFA* p : it->second) {
                q->acceptable |= p->acceptable;
                for (const auto& a : p->transitions) {
                    if (a.first != '\0') {
                        for (NFA* r : a.second) {
                            auto it1 = r->transitions.find('\0');
                            if (it1 != r->transitions.end()) {
                                std::set<NFA*> &s = q->transitions[a.first];
                                for (NFA* t : it1->second) {
                                    s.insert(t);
                                    t->parents.insert(&s);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for (NFA* state : states) {
        auto it = state->transitions.find('\0');
        if (it != state->transitions.end()) {
            state->transitions.erase(it);
        }
    }

    return this;
}

void NFA::print() {
    this->setDepths();
    this->walk();
}

std::set<NFA*>& NFA::operator [] (char transition) {
    return this->transitions[transition];
}

bool NFA::isValidRegex(std::string regex) {
    int notClosedBrackets = 0;
    for (int i = 0; i < regex.size(); ++i) {
        if (regex[i] == '+') {
            if (i == 0 || i == regex.size() - 1 || regex[i - 1] == '(' || regex[i + 1] == '+' || regex[i + 1] == ')' || regex[i + 1] == '*') {
                return false;
            }
        } else if (regex[i] == '*') {
            if (i == 0 || regex[i - 1] == '(') {
                return false;
            }
        } else if (regex[i] == '(') {
            ++notClosedBrackets;
        } else if (regex[i] == ')') {
            if (notClosedBrackets == 0) {
                return false;
            }
            --notClosedBrackets;
        }
    }
    return notClosedBrackets == 0;
}

NFA* NFA::regexToAutomaton(std::string regex) {
    if (!isValidRegex(regex)) {
        return nullptr;
    }
    regex += ")";
    std::stack<std::pair<NFA*, bool>> autOpStack;
    autOpStack.push({new NFA(), NFA::CONCATENATION});
    int i = 0;
    NFA* automaton = new NFA();
    while (!autOpStack.empty()) {
        while (regex[i] != '(' && regex[i] != ')' && regex[i] != '+' && regex[i + 1] != '*') {
            automaton = automaton->concatenate(new NFA(regex[i]));
            ++i;
        }
        if (regex[i] != ')' && regex[i + 1] == '*') {
            NFA* tmpAutomaton = new NFA(regex[i]);
            tmpAutomaton = tmpAutomaton->cycle();
            automaton = automaton->concatenate(tmpAutomaton);
            do {
                ++i;
            } while (regex[i] == '*');
        } else {
            if (regex[i] == '(') {
                autOpStack.emplace(automaton, NFA::CONCATENATION);
                automaton = new NFA();
            } else if (regex[i] == ')') {
                auto autOp = autOpStack.top();
                autOpStack.pop();
                if (autOp.second == NFA::ADDITION) {
                    automaton = autOp.first->add(automaton);
                    autOp = autOpStack.top();
                    autOpStack.pop();
                }
                if (i < regex.size() && regex[i + 1] == '*') {
                    automaton = automaton->cycle();
                    do {
                        ++i;
                    } while (regex[i] == '*');
                    --i;
                }
                automaton = autOp.first->concatenate(automaton);
            } else if (regex[i] == '+') {
                auto autOp = autOpStack.top();
                if (autOp.second == NFA::ADDITION) {
                    autOpStack.pop();
                    automaton = autOp.first->add(automaton);
                }
                autOpStack.emplace(automaton, NFA::ADDITION);
                automaton = new NFA();
            }
            ++i;
        }
    }
    return automaton;
}

DFA* NFA::toDFA() {
    PtrMap<std::set<NFA*>, DFA> dfaStates;
    std::stack<std::set<NFA*>> stateSetStack;
    std::stack<DFA*> exploredStates;
    stateSetStack.push({this});
    while (!stateSetStack.empty()) {
        std::set<NFA*> s = stateSetStack.top();
        stateSetStack.pop();
        DFA* &state = dfaStates[s];
        if (!state->isExplored()) {
            state->setExplored(true);
            exploredStates.push(state);
            std::map<char, std::set<NFA*>> _transitions;
            bool _acceptable = false;
            for (NFA* q : s) {
                _acceptable |= q->acceptable;
                for (const auto& p : q->transitions) {
                    std::set<NFA*> &t = _transitions[p.first];
                    for (NFA* r : p.second) {
                        t.insert(r);
                    }
                }
            }
            state->setAcceptable(_acceptable);
            for (const auto& p : _transitions) {
                stateSetStack.push(p.second);
                (*state)[p.first] = dfaStates[p.second];
            }
        }
    }
    while (!exploredStates.empty()) {
        DFA* state = exploredStates.top();
        exploredStates.pop();
        state->setExplored(false);
    }
    return dfaStates[{this}];
}
