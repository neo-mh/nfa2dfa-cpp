#include <cstdio>
#include <iostream>
#include <bitset>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <queue>
#include <set>

#define MAX_NFA_STATES 10
#define MAX_ALPHABET_SIZE 10

using namespace std;

class NFAState {
public:
    int transitions[MAX_ALPHABET_SIZE][MAX_NFA_STATES];
    NFAState() {
        for (int i = 0; i < MAX_ALPHABET_SIZE; i++)
            for (int j = 0; j < MAX_NFA_STATES; j++)
                transitions[i][j] = -1;
    }
} *nfaStates;

struct DFAState {
    bool isFinalState;
    bitset<MAX_NFA_STATES> nfaStatesSet;
    bitset<MAX_NFA_STATES> transitions[MAX_ALPHABET_SIZE];
    int symbolicTransitions[MAX_ALPHABET_SIZE];
};

set<int> nfaFinalStates;
vector<int> dfaFinalStates;
vector<DFAState*> dfaStates;
queue<int> incompleteDFAStates;
int nfaStateCount, alphabetSize;

void findEpsilonClosure(int state, bitset<MAX_NFA_STATES>& closure) {
    for (int i = 0; i < nfaStateCount && nfaStates[state].transitions[0][i] != -1; i++)
        if (closure[nfaStates[state].transitions[0][i]] == 0) {
            closure[nfaStates[state].transitions[0][i]] = 1;
            findEpsilonClosure(nfaStates[state].transitions[0][i], closure);
        }
}

void findEpsilonClosure(bitset<MAX_NFA_STATES> state, bitset<MAX_NFA_STATES>& closure) {
    for (int i = 0; i < nfaStateCount; i++)
        if (state[i] == 1)
            findEpsilonClosure(i, closure);
}

void nfaMove(int state, int symbol, bitset<MAX_NFA_STATES>& resultStates) {
    for (int i = 0; i < nfaStateCount && nfaStates[state].transitions[symbol][i] != -1; i++)
        resultStates[nfaStates[state].transitions[symbol][i]] = 1;
}

void nfaMove(bitset<MAX_NFA_STATES> currentStates, int symbol, bitset<MAX_NFA_STATES>& resultStates) {
    for (int i = 0; i < nfaStateCount; i++)
        if (currentStates[i] == 1)
            nfaMove(i, symbol, resultStates);
}

int main() {
    int state, transitionCount, finalStateCount, transitionState, transitionSymbol, targetCount;

    cout << "Enter the number of NFA states and the size of the input alphabet: ";
    cin >> nfaStateCount >> alphabetSize;

    nfaStates = new NFAState[nfaStateCount];

    cout << "Enter the number of final states: ";
    cin >> finalStateCount;

    cout << "Enter the final states: ";
    for (int i = 0; i < finalStateCount; i++) {
        cin >> state;
        nfaFinalStates.insert(state);
    }

    cout << "Enter the number of transitions: ";
    cin >> transitionCount;

    cout << "Enter the transitions in the format 'X A Y1 Y2 ... Yk':\n";
    while (transitionCount--) {
        cin >> state >> transitionSymbol >> targetCount;
        for (int i = 0; i < targetCount; i++) {
            cin >> transitionState;
            nfaStates[state].transitions[transitionSymbol][i] = transitionState;
        }
    }

    int dfaStateCount = 1;
    dfaStates.push_back(new DFAState);
    dfaStates[0]->nfaStatesSet[0] = 1;
    findEpsilonClosure(0, dfaStates[0]->nfaStatesSet);

    for (int j = 0; j < nfaStateCount; j++)
        if (dfaStates[0]->nfaStatesSet[j] == 1 && nfaFinalStates.find(j) != nfaFinalStates.end()) {
            dfaStates[0]->isFinalState = true;
            dfaFinalStates.push_back(0);
            break;
        }

    incompleteDFAStates.push(0);

    while (!incompleteDFAStates.empty()) {
        int currentState = incompleteDFAStates.front();
        incompleteDFAStates.pop();
        for (int i = 1; i <= alphabetSize; i++) {
            bitset<MAX_NFA_STATES> newStates;
            nfaMove(dfaStates[currentState]->nfaStatesSet, i, newStates);
            findEpsilonClosure(newStates, newStates);
            int j;
            for (j = 0; j < dfaStateCount; j++) {
                if (newStates == dfaStates[j]->nfaStatesSet) {
                    dfaStates[currentState]->symbolicTransitions[i] = j;
                    break;
                }
            }
            if (j == dfaStateCount) {
                dfaStates[currentState]->symbolicTransitions[i] = dfaStateCount;
                dfaStates.push_back(new DFAState);
                dfaStates[dfaStateCount]->nfaStatesSet = newStates;
                for (j = 0; j < nfaStateCount; j++)
                    if (dfaStates[dfaStateCount]->nfaStatesSet[j] == 1 && nfaFinalStates.find(j) != nfaFinalStates.end()) {
                        dfaStates[dfaStateCount]->isFinalState = true;
                        dfaFinalStates.push_back(dfaStateCount);
                        break;
                    }
                incompleteDFAStates.push(dfaStateCount);
                dfaStateCount++;
            }
        }
    }

    cout << "\nDFA Representation:\n";
    cout << "Number of states: " << dfaStateCount << "\n";
    cout << "Size of input alphabet: " << alphabetSize << "\n";
    cout << "Number of final states: " << dfaFinalStates.size() << "\n";
    cout << "Final states: ";
    for (vector<int>::iterator it = dfaFinalStates.begin(); it != dfaFinalStates.end(); it++)
        cout << *it << " ";
    cout << "\nTransitions:\n";
    for (int i = 0; i < dfaStateCount; i++) {
        for (int j = 1; j <= alphabetSize; j++)
            cout << "State " << i << " on input " << j << " -> State " << dfaStates[i]->symbolicTransitions[j] << "\n";
    }

    return 0;
}
