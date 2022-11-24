#pragma once

#include "Grammar.hpp"
#include <set>
#include <vector>
#include <unordered_set>

/**
 * @brief Shift Reduce table element;
 * 
 */
struct TableElement {
    char action; // 'a' - accept, 'r' - reduce, 's' - shift
    int leftSide, elementCnt, shiftTo;

    TableElement() { action = leftSide = elementCnt = shiftTo = 0; };
    TableElement(char action, int shiftTo) : action(action), shiftTo(shiftTo) {};
    TableElement(char action, int leftSide, int elementCnt) : action(action), leftSide(leftSide), elementCnt(elementCnt) {};
};

/**
 * @brief LR(1) parser state
 * 
 */
struct LR1State {
    int grammarProductionLeft;      ///< encoded id of non terminal character on left side of the production
    int grammarProductionId;        ///< id of grammar production for ntc grammarProductionLeft
    int dotPosition;                ///< position of character that dot is in front of
    int followChar;                 ///< character after production


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
    bool operator==(const LR1State &s) const {
        return (grammarProductionLeft == s.grammarProductionLeft &&
                grammarProductionId == s.grammarProductionId &&
                dotPosition == s.dotPosition &&
                followChar == followChar);
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
    std::vector<std::set<LR1State> > m_C; 
    std::vector<std::vector<TableElement> > m_table; // terminal ... nonterminal
    std::set<LR1State> m_exists;

    const std::set<int>& m_getFirstForNonTerminalChar(int id);                          ///< computes first set if not in memory

public:
    TableBuilder(const Grammar &grammar);                                               ///< constructor from grammar

    void getFirst(const std::vector<int>::const_iterator chSeriesBegin, const std::vector<int>::const_iterator chSeriesEnd, std::set<int> &dst);      ///< computes first set of a character series
    void closure(std::set<LR1State> &dfaState);                                         ///< computes closure set and stores in place
    void gotoState(const std::set<LR1State> &src, int ch, std::set<LR1State> &dst);     ///< determines next state given current state and character
    void generate();
    void generate_items();
    void print();
};