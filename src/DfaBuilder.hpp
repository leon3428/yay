#pragma once

#include "Grammar.hpp"
#include <set>
#include <vector>

struct LR1State {
    int grammarProductionLeft;
    int grammarProductionId;
    int dotPosition;
    int followChar;

    LR1State(int grammarProductionLeft, int grammarProductionId, int dotPosition, int followChar)
        : grammarProductionLeft(grammarProductionLeft), grammarProductionId(grammarProductionId), dotPosition(dotPosition), followChar(followChar) {}

    LR1State()
        : grammarProductionLeft(-1), grammarProductionId(-1), dotPosition(-1), followChar(-1) {}

    bool operator<(const LR1State &s) const {
        if(grammarProductionLeft == s.grammarProductionLeft) {
            if(grammarProductionId == s.grammarProductionId) {
                if(dotPosition == s.dotPosition) {
                    return followChar < s.followChar; 
                }
                return dotPosition < s.dotPosition;
            }
            return grammarProductionId < s.grammarProductionId;
        }
        return grammarProductionLeft < s.grammarProductionLeft;
    }
};

class DfaBuilder {
private:
    const Grammar &m_grammar;
    std::vector<std::set<int> > m_nonTerminalFirst;
    
    const std::set<int>& m_getFirstForNonTerminalChar(int id);

public:
    DfaBuilder(const Grammar &grammar);

    void getFirst(const std::vector<int> &chSeries, std::set<int> &dst);
    void closure(std::set<LR1State> &dfaState);
    void gotoState(const std::set<LR1State> &src, int ch, std::set<LR1State> &dst);
};