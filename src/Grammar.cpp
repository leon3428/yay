#include "Grammar.hpp"
#include "Utils.hpp"
#include <sstream>
#include <algorithm>

Grammar::Grammar() {

}

Grammar::Grammar(std::istream& stream) {
    loadGrammar(stream);
}

int Grammar::m_svFindInd(const std::vector<std::string> &v, const std::string &s) {
    auto it = std::lower_bound(v.begin(), v.end(), s);
    if(it == v.end() || (it == v.begin() && v[0] != s))
        return -1;

    return it - v.begin();
}

void Grammar::loadGrammar(std::istream& stream) {
    std::string line;
    bool charsSorted = false;
    int productionCnt = 0;

    while(getline(stream, line)) {
        int leftSide;

        if(line[0] == '%') {       
            std::stringstream ss(line);
            std::string token;   
            ss >> token;
            if(token == "%V") {
                while(ss >> token) {
                    m_nonTerminalChars.push_back(token);
                }  
            } else if(token == "%T") {
                while(ss >> token) {
                    m_terminalChars.push_back(token);
                }  
            } else if(token == "%Syn") {
                 while(ss >> token) {
                    m_synChars.push_back(token);
                }  
            }
        } else if(line[0] != ' ') {
            if(!charsSorted) {
                charsSorted = true;
                std::string start = m_nonTerminalChars[0];

                std::sort(m_nonTerminalChars.begin(), m_nonTerminalChars.end());
                std::sort(m_terminalChars.begin(), m_terminalChars.end());
                std::sort(m_synChars.begin(), m_synChars.end());

                m_startNonTerminalChar = m_svFindInd(m_nonTerminalChars, start);

                m_grammarProductions.resize(m_nonTerminalChars.size());
            }

            leftSide = m_svFindInd(m_nonTerminalChars, line);
        } else {
            std::stringstream ss(line);
            std::string token;

            m_grammarProductions[leftSide].emplace_back();
            m_grammarProductions[leftSide].back().priority = productionCnt;
            productionCnt++;

            while(ss >> token) {
                int ind;
                if(token[0] == '<')
                    ind = m_svFindInd(m_nonTerminalChars, token) * -1 -1;
                else
                    ind = m_svFindInd(m_terminalChars, token);
                
                 m_grammarProductions[leftSide].back().rightSide.push_back(ind);
            }
        }
    }
}