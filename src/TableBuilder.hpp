#pragma once

#include "Grammar.hpp"
#include <set>
#include <vector>
#include <unordered_set>
#include <fstream>
#include <unordered_map>
#include <map>

/**
 * @brief Shift Reduce table element;
 * 
 */
struct TableElement {
    char action; // 'A' - accept, 'R' - reduce, 'S' - shift
    int leftSide, elementCnt, shiftTo, priority;

    TableElement() { action = leftSide = elementCnt = shiftTo = priority = 0; };
    TableElement(char action) : action(action) {};
    TableElement(char action, int shiftTo, int priority) : action(action), shiftTo(shiftTo), priority(priority) {};
	TableElement(char action, int leftSide, int elementCnt, int priority) : action(action), leftSide(leftSide), elementCnt(elementCnt), priority(priority) {};
    TableElement(char action, int leftSide, int elementCnt, int shiftTo, int priority) : action(action), leftSide(leftSide), elementCnt(elementCnt), shiftTo(shiftTo), priority(priority) {};
};

/**
 * @brief LR(1) parser state
 * 
 */
struct LR1State {
    int grammarProductionLeft;      ///< encoded id of non terminal character on left side of the production
    unsigned int grammarProductionId;        ///< id of grammar production for ntc grammarProductionLeft
    unsigned int dotPosition;                ///< position of character that dot is in front of
    int followChar;                 ///< character after production


    LR1State(int grammarProductionLeft, unsigned int grammarProductionId, unsigned int dotPosition, int followChar)
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
    bool operator==(const LR1State &s) const {
        return (grammarProductionLeft == s.grammarProductionLeft &&
                grammarProductionId == s.grammarProductionId &&
                dotPosition == s.dotPosition &&
                followChar == s.followChar);
    }
};

struct DfaStateHasher {
    std::size_t operator()(std::set<LR1State> const& s) const {
        std::size_t seed = s.size();
        for(auto lr1State : s) {
            uint32_t x = lr1State.grammarProductionLeft;
            x *= 37;
            x += lr1State.grammarProductionId;
            x *= 37;
            x += lr1State.dotPosition;
            x *= 37;
            x += lr1State.followChar;
            seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

/**
 * @brief TableBuilder constructs action and goto tables of an LR(1) parser for a given grammar
 * 
 */
class TableBuilder {
private:
    const Grammar &m_grammar;                                                           ///< reference to grammar 
    std::vector<std::set<int> > m_nonTerminalFirst;                                     ///< memory for already computed first set of an ntc
    std::unordered_map<std::set<LR1State>, int, DfaStateHasher> m_stateMap;  
    std::unordered_map<std::set<LR1State>, std::map<int, int> , DfaStateHasher> m_gotoMap;
    std::vector<std::vector<TableElement> > m_table; // terminal ... nonterminal
    const std::set<int>& m_getFirstForNonTerminalChar(int id);                          ///< computes first set if not in memory
    int startState;

public:
    TableBuilder(const Grammar &grammar);                                               ///< constructor from grammar

    void getFirst(const std::vector<int>::const_iterator chSeriesBegin, const std::vector<int>::const_iterator chSeriesEnd, std::set<int> &dst);      ///< computes first set of a character series
    void closure(std::set<LR1State> &dfaState);                                         ///< computes closure set and stores in place
    void gotoState(const std::set<LR1State> &src, int ch, std::set<LR1State> &dst);     ///< determines next state given current state and character
    void generate();
    void generate_items();
    void print();
    void outputTable(std::string outPath);
};