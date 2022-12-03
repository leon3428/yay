#include "TableBuilder.hpp"
#include "Utils.hpp"
#include <stack>
#include <set>
#include <fstream>
#include <assert.h>

TableBuilder::TableBuilder(const Grammar &grammar) 
    : m_grammar(grammar) {
    
    m_nonTerminalFirst.resize(grammar.getNonTerminalSize());
}

const std::set<int>& TableBuilder::m_getFirstForNonTerminalChar(int id) {
    DebugAssert(Grammar::isTerminal(id), "Char is not terminal");

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

        for(size_t i = 0;i < m_grammar.getProductionsForNTC(state_gp.rightSide[lr1StateIt -> dotPosition]).size(); i++) {
            std::set<int> first;

            getFirst(state_gp.rightSide.begin() + lr1StateIt -> dotPosition + 1, state_gp.rightSide.end(), first);
            if(first.find(epsilonId) != first.end()) {
                first.erase(epsilonId);
                first.insert(lr1StateIt -> followChar);
            }

            for(int nextCh : first) {
                LR1State newState{state_gp.rightSide[lr1StateIt -> dotPosition], (unsigned int)i, 0, nextCh};
                
                if(dfaState.find(newState) != dfaState.end()) {
                    auto res = dfaState.insert(newState);
                    stack.push(res.first);
                }
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
    std::cout <<"PRODUCTIONS\n";
    for(int i = 0; i < (int)m_C.size(); ++i){
        std::cout << "set i = " << i << "\n";
        for(auto it : m_C[i]) {

            std::cout << m_grammar.getTerminalChars()[m_grammar.decodeNonTerminalId(it.grammarProductionLeft)] << " " 
            << it.grammarProductionId << " " << it.dotPosition << " " << m_grammar.getTerminalChars()[it.followChar] << "\n"; 
        }
    }
    std::cout << "\n";
    std::cout << "Table:\n";
    for(auto it1 : m_table){
        for(auto it2 : it1) {
            if(it2.action == 'S')
                std::cout << it2.shiftTo << " ";
            else
                std::cout << it2.action << " ";
        }
        std::cout << "\n";
    }
}

void TableBuilder::generate_items() {
    std::set<LR1State> state;
    state.emplace(m_grammar.getNonTerminalCharId("<'S>"), 0, 0, m_grammar.getTerminalCharId("$"));
    m_C.push_back(state);

    size_t idx = 0;
    while(idx < m_C.size()) {
        closure(m_C[idx]);
        for(int id = -m_grammar.getNonTerminalSize(); id < m_grammar.getTerminalSize(); ++id) {
            std::set<LR1State> dst;
            gotoState(m_C[idx], id, dst);
            
            if(!dst.size()) continue;
            
            bool exists = false;

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

void TableBuilder::outputTable(std::string outPath) {
    std::ofstream out(outPath);
    out << m_grammar.getTerminalSize() << "\n";
    out << m_grammar.getSynSize() << "\n";
    for(auto it : m_grammar.getSynChars())
        out << it << " ";
    out << "\n";
    out << m_C.size() << " " << m_grammar.getNonTerminalSize() + m_grammar.getTerminalSize() << "\n";
    for(auto it : m_grammar.getTerminalChars())
        out << it << " ";
    for(auto it : m_grammar.getNonTerminalChars())
        out << it << " ";
    out << "\n";

    for(auto it1 : m_table){
        for(auto it2 : it1) {
            out << it2.action << " " << it2.leftSide << " " << it2.elementCnt << " " << it2.shiftTo << " " << it2.priority << "\n";
        }
    }
    out << startState;
}


void TableBuilder::generate() {
    generate_items();
    //print();

    m_table.resize(m_C.size());
    
    for(int i = 0; i < (int)m_C.size(); ++i){
        m_table[i].resize(m_grammar.getTerminalSize() + m_grammar.getNonTerminalSize());
        
        for(LR1State curr : m_C[i]) {
            if(curr.grammarProductionLeft == m_grammar.getNonTerminalCharId("<'S>") && curr.dotPosition == 1 && curr.followChar == m_grammar.getTerminalCharId("$")){ // S'
                m_table[i][m_grammar.getTerminalCharId("$")] = TableElement('A');
                continue;
            }
            if(curr.grammarProductionLeft == m_grammar.getNonTerminalCharId("<'S>") && curr.dotPosition == 0)
                startState = i;

            GrammarProduction production = m_grammar.getGrammarProduction(curr.grammarProductionLeft, curr.grammarProductionId);
            int emptySymbol = m_grammar.getTerminalCharId("$");

            if(curr.dotPosition < m_grammar.getGrammarProductionSize(curr.grammarProductionLeft, curr.grammarProductionId)
                && production.rightSide[0] != emptySymbol) {
                int symbol = production.rightSide[curr.dotPosition];
                if(!m_grammar.isTerminal(symbol))
                    continue;
                
                std::set<LR1State> dest;
                gotoState(m_C[i], symbol, dest);
                
                for(size_t j = 0; j < m_C.size(); ++j) {
                    if(dest == m_C[j]){
                        if( (m_table[i][symbol].action == 'S' && m_table[i][symbol].priority > production.priority) || m_table[i][symbol].action != 'S')
                            m_table[i][symbol] = TableElement('S', j, production.priority); // shift j
                        break;
                    }
                }
            } else if(curr.dotPosition == m_grammar.getGrammarProductionSize(curr.grammarProductionLeft, curr.grammarProductionId) ||
                    production.rightSide[0] == emptySymbol) {
                int nxt = curr.followChar;
                if(!m_grammar.isTerminal(nxt))
                    nxt = m_grammar.decodeNonTerminalId(nxt) + m_grammar.getNonTerminalSize();
                if(m_table[i][nxt].action != 'S'){ // reduce shift -> in favor of shift
                    if(production.rightSide[0] != emptySymbol)
                        m_table[i][nxt] = TableElement('R', curr.grammarProductionLeft, m_grammar.getGrammarProductionSize(curr.grammarProductionLeft, curr.grammarProductionId));
                    else
                        m_table[i][nxt] = TableElement('R', curr.grammarProductionLeft, 0);
                }
            } else{
                assert(false);
            }
        }
    }
    // construct for non terminals
    for(int i = 0; i < (int)m_C.size(); ++i) {
        for(LR1State curr : m_C[i]) {
            GrammarProduction production = m_grammar.getGrammarProduction(curr.grammarProductionLeft, curr.grammarProductionId);
            int symbol = production.rightSide[curr.dotPosition];

            if(m_grammar.isTerminal(symbol) || production.rightSide.size() == curr.dotPosition) continue;

            std::set<LR1State> dest;
            gotoState(m_C[i], symbol, dest);

            for(size_t j = 0; j < m_C.size(); ++j){
                if(dest == m_C[j]){
                    if(m_table[i][m_grammar.decodeNonTerminalId(symbol) + m_grammar.getTerminalSize()].action != 'S' || m_table[i][m_grammar.decodeNonTerminalId(symbol) + m_grammar.getTerminalSize()].priority > production.priority)
                        m_table[i][m_grammar.decodeNonTerminalId(symbol) + m_grammar.getTerminalSize()] = TableElement('S', j, production.priority); // shift j
                    break;
                }
            }
        }
    }
    
    for(int i = 0; i < (int)m_table.size(); ++i)
        for(int j = 0; j < (int)m_table[i].size(); ++j){
            if(!m_table[i][j].action)
                m_table[i][j] = TableElement('E');
        }
}