#pragma once

#include "Grammar.hpp"
#include <set>
#include <vector>

class DfaBuilder {
private:
    const Grammar &m_grammar;
    std::vector<std::set<int> > m_nonTerminalFirst;
    
    const std::set<int>& m_getFirstForNonTerminalChar(int id);

public:
    DfaBuilder(const Grammar &grammar);

    void getFirst(const std::vector<int> &chSeries, std::set<int> &dst);
};