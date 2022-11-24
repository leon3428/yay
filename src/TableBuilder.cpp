#include "TableBuilder.hpp"
#include "Utils.hpp"
#include <stack>
#include <set>

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

void TableBuilder::print() {
    for(int i = 0; i < (int)m_C.size(); ++i){
        std::cout << "set i = " << i << "\n";
        for(auto it : m_C[i]) {
            std::cout << it.grammarProductionLeft << " " << it.grammarProductionId << " " << it.dotPosition << " " << it.followChar << "\n"; 
        }
    }
    /*
    std::cout << "Table:\n";
    for(auto it1 : m_table){
        for(auto it2 : it1) {
            std::cout << it2.action << " ";
        }
        std::cout << "\n";
    }*/
}

void TableBuilder::generate_items() {
    std::set<LR1State> state;
    state.emplace(-1, 0, 0, m_grammar.getTerminalCharId("$"));
    m_C.push_back(state);

    int idx = 0;
    while(idx < m_C.size()) {
        // items
        closure(m_C[idx]);
        for(int id = -m_grammar.getNonTerminalSize(); id < m_grammar.getTerminalSize(); ++id) {
            //int id = m_grammar.encodeNonTerminalId(m_grammar.m_svFindInd(m_grammar.m_nonTerminalChars, symbol));

            std::set<LR1State> dst;
            gotoState(m_C[idx], id, dst);
            
            if(!dst.size()) continue;
            
            bool exists = false;
            int item_id = -1;

            for(auto sets : m_C)
                if(sets == dst){
                    exists = true;
                    break;
                }
            if(!exists)
                m_C.push_back(dst);
        }    
        idx ++;
    }
}



void TableBuilder::generate() {
    generate_items();
    print();
    return;

    m_table.resize(m_C.size());
    
    for(int i = 0; i < (int)m_C.size(); ++i){
        m_table[i].resize(m_grammar.getTerminalSize() + m_grammar.getNonTerminalSize());
        
        for(LR1State curr : m_C[i]) {
            if(curr.grammarProductionLeft == m_grammar.getStartChar()){
                GrammarProduction production = m_grammar.getGrammarProduction(curr.grammarProductionLeft, curr.grammarProductionId);
                if(production.rightSide[0] == -1)
                    m_table[i][m_grammar.encodeNonTerminalId(m_grammar.getStartChar()) + m_grammar.getNonTerminalSize()] = TableElement('a', -1);
                continue;
            }
            
            if(curr.dotPosition < m_grammar.getGrammarProductionSize(curr.grammarProductionLeft, curr.grammarProductionId)) {
                GrammarProduction production = m_grammar.getGrammarProduction(curr.grammarProductionLeft, curr.grammarProductionId);
                int symbol = production.rightSide[curr.dotPosition];
                if(!m_grammar.isTerminal(symbol))
                    continue;
                
                std::set<LR1State> dest;
                gotoState(m_C[i], symbol, dest);
                
                for(int j = 0; j < m_C.size(); ++j){
                    if(dest == m_C[j]){
                        int idx = 0;
                        if (!m_grammar.isTerminal(symbol)){
                            idx = m_grammar.getTerminalSize();
                            symbol = m_grammar.decodeNonTerminalId(symbol);
                        }
                        //std::cout << idx << " " << symbol << " " << idx+symbol << "/" << m_grammar.getTerminalSize() + m_grammar.getTerminalSize() << "\n";
                        m_table[i][idx + symbol] = TableElement('s', j); // shift j
                        break;
                    }
                }
            } else if(curr.dotPosition == m_grammar.getGrammarProductionSize(curr.grammarProductionLeft, curr.grammarProductionId)) {
                if(curr.grammarProductionLeft == -1) // S'
                    continue;

                int nxt = curr.followChar;
                if(!m_grammar.isTerminal(nxt))
                    nxt = m_grammar.decodeNonTerminalId(nxt) + m_grammar.getNonTerminalSize();
                std::cout << curr.grammarProductionLeft << " ";
                std::cout << m_grammar.getGrammarProductionSize(curr.grammarProductionLeft, curr.grammarProductionId) << " ";
                std::cout << nxt << "\n";
                m_table[i][nxt] = TableElement('r', curr.grammarProductionLeft, m_grammar.getGrammarProductionSize(curr.grammarProductionLeft, curr.grammarProductionId));
            } else{
                std::cout << "WTF\n";
            }
        }
    }
    
    for(int i = 0; i < (int)m_table.size(); ++i)
        for(int j = 0; j < (int)m_table[i].size(); ++j){
            if(!m_table[i][j].action)
                m_table[i][j] = TableElement('e', -1);
        }
    
    print();
}
