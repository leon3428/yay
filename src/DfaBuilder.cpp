#include "DfaBuilder.hpp"
#include "Utils.hpp"

DfaBuilder::DfaBuilder(const Grammar &grammar) 
    : m_grammar(grammar) {
    
    m_nonTerminalFirst.resize(grammar.getNonTerminalSize());
}

const std::set<int>& DfaBuilder::m_getFirstForNonTerminalChar(int id) {
    DebugAssert(Grammar::isTerminal(id), "Char is not terminal");

    int epsilonId = m_grammar.getTerminalCharId("$");

    if(m_nonTerminalFirst[Grammar::decodeNonTerminalId(id)].empty()) {
        for(GrammarProduction gp : m_grammar.getProductionsForNTC(id)) {
            getFirst(gp.rightSide, m_nonTerminalFirst[Grammar::decodeNonTerminalId(id)]);
        }
    }

    return m_nonTerminalFirst[Grammar::decodeNonTerminalId(id)];
}

void DfaBuilder::getFirst(const std::vector<int> &chSeries, std::set<int> &dst) {
    DebugAssert(chSeries.size() == 0, "Character series is empty");

    int epsilonId = m_grammar.getTerminalCharId("$");

    for(int i = 0;i < chSeries.size(); i++) {
        if(Grammar::isTerminal(chSeries[i])) {
            dst.insert(chSeries[i]);
            if(chSeries[i] != epsilonId)
                break;
        } else {
            const std::set<int> &s = m_getFirstForNonTerminalChar(chSeries[i]);
            dst.insert(s.begin(), s.end());

            if(s.find(epsilonId) == s.end())
                break;
            
            if(i != chSeries.size() - 1)
                dst.erase(epsilonId);
        }
    }
}