#include "TableBuilder.hpp"
#include "Utils.hpp"
#include <stack>

TableBuilder::TableBuilder(const Grammar &grammar) 
    : m_grammar(grammar) {
    
    m_nonTerminalFirst.resize(grammar.getNonTerminalSize());
}

const std::set<int>& TableBuilder::m_getFirstForNonTerminalChar(int id) {
    DebugAssert(Grammar::isTerminal(id), "Char is not terminal");

    int epsilonId = m_grammar.getTerminalCharId("$");

    if(m_nonTerminalFirst[Grammar::decodeNonTerminalId(id)].empty()) {
        for(GrammarProduction gp : m_grammar.getProductionsForNTC(id)) {
            getFirst(gp.rightSide.begin(), gp.rightSide.end(), m_nonTerminalFirst[Grammar::decodeNonTerminalId(id)]);
        }
    }

    return m_nonTerminalFirst[Grammar::decodeNonTerminalId(id)];
}

void TableBuilder::getFirst(const std::vector<int>::const_iterator chSeriesBegin, const std::vector<int>::const_iterator chSeriesEnd, std::set<int> &dst) {
    int epsilonId = m_grammar.getTerminalCharId("$");
    
    if(chSeriesBegin == chSeriesEnd) {
        dst.insert(epsilonId);
        return;
    }

    for(auto it = chSeriesBegin; it != chSeriesEnd; ++it) {
        if(Grammar::isTerminal(*it)) {
            dst.insert(*it);
            if(*it != epsilonId)
                break;
        } else {
            const std::set<int> &s = m_getFirstForNonTerminalChar(*it);
            dst.insert(s.begin(), s.end());

            if(s.find(epsilonId) == s.end())
                break;
            
            if(it != chSeriesEnd - 1)
                dst.erase(epsilonId);
        }
    }
}

void TableBuilder::closure(std::set<LR1State> &dfaState) {
    int epsilonId = m_grammar.getTerminalCharId("$");

    std::stack<std::set<LR1State>::iterator > stack;
    for(auto it = dfaState.begin(); it != dfaState.end(); ++it) {
        stack.push(it);
    }
        
    while (!stack.empty())
    {
        auto lr1StateIt = stack.top();
        stack.pop();

        const GrammarProduction &state_gp = m_grammar.getGrammarProduction(lr1StateIt -> grammarProductionLeft, lr1StateIt -> grammarProductionId);
        if(lr1StateIt -> dotPosition >= state_gp.rightSide.size())
            continue;
        if(Grammar::isTerminal(state_gp.rightSide[lr1StateIt -> dotPosition]))
            continue;

        for(int i = 0;i < m_grammar.getProductionsForNTC(state_gp.rightSide[lr1StateIt -> dotPosition]).size(); i++) {
            std::set<int> first;

            getFirst(state_gp.rightSide.begin() + lr1StateIt -> dotPosition + 1, state_gp.rightSide.end(), first);
            if(first.find(epsilonId) != first.end()) {
                first.erase(epsilonId);
                first.insert(lr1StateIt -> followChar);
            }

            for(int nextCh : first) {
                auto res = dfaState.emplace(state_gp.rightSide[lr1StateIt -> dotPosition], i, 0, nextCh);
                stack.push(res.first);
            }
        }
         
    }
}

void TableBuilder::gotoState(const std::set<LR1State> &src, int ch, std::set<LR1State> &dst) {
    DebugAssert(!dst.empty(), "Destination set is not empty");

    for(const LR1State &lr1State : src) {
        const GrammarProduction &gp = m_grammar.getGrammarProduction(lr1State.grammarProductionLeft, lr1State.grammarProductionId);
        if(lr1State.dotPosition >= gp.rightSide.size())
            continue;
        int nextCh = gp.rightSide[lr1State.dotPosition];
        if(nextCh == ch) {
            dst.emplace(lr1State.grammarProductionLeft, lr1State.grammarProductionId, lr1State.dotPosition + 1, lr1State.followChar);
        }
    }

    closure(dst);
}