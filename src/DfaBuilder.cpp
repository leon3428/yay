#include "DfaBuilder.hpp"
#include "Utils.hpp"
#include <stack>

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
    int epsilonId = m_grammar.getTerminalCharId("$");
    
    if(chSeries.size() == 0) {
        dst.insert(epsilonId);
        return;
    }

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

void DfaBuilder::closure(std::set<LR1State> &dfaState) {
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
            std::vector<int> charSeq;
            charSeq.insert(charSeq.end(), state_gp.rightSide.begin() + lr1StateIt -> dotPosition + 1, state_gp.rightSide.end());
            if(lr1StateIt -> followChar != epsilonId)
                charSeq.push_back(lr1StateIt -> followChar);
            std::set<int> first;

            getFirst(charSeq, first);

            for(int nextCh : first) {
                auto res = dfaState.emplace(state_gp.rightSide[lr1StateIt -> dotPosition], i, 0, nextCh);
                stack.push(res.first);
            }
        }
         
    }
}

void DfaBuilder::gotoState(const std::set<LR1State> &src, int ch, std::set<LR1State> &dst) {
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