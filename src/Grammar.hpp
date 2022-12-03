#pragma once

#include <istream>
#include <vector>
#include <iostream>
#include "Utils.hpp"

/**
 * @brief Grammar production
 * 
 */
struct GrammarProduction {
    int priority;                   ///< smaller value equals larger priority
    std::vector<int> rightSide;     ///< position of terminal character in vector m_terminalChar or (position of nonterminal character in vector m_nonTerminalChar) * -1 -1
};

/**
 * @brief Grammar class
 * 
 */
class Grammar {
private:
    std::vector<std::string> m_nonTerminalChars;                                ///< sorted vector of nonterminal characters
    std::vector<std::string> m_terminalChars;                                   ///< sorted vector of terminal characters
    std::vector<std::string> m_synChars;                                        ///< sorted vector of synchronizing characters used for error recovery 
    std::vector<std::vector<GrammarProduction> > m_grammarProductions;    ///< list of grammar productions for every left side nonterminal character

    int m_startChar;                                                 ///< index of starting nonterminal character

    static int m_svFindInd(const std::vector<std::string> &v, const std::string &s);        ///< finds index of string s in sorted vector v using binary search

public:
    Grammar();
    Grammar(std::istream& stream);                                              ///< constructs grammar from stream

    void loadGrammar(std::istream& stream);                                     ///< loads grammar from stream

    inline int getStartChar() const { return m_startChar; }
	const std::vector<std::string> &getNonTerminalChars() const { return m_nonTerminalChars; }
	const std::vector<std::string> &getTerminalChars() const { return m_terminalChars; }
	const std::vector<std::string> &getSynChars() const { return m_synChars; }

    inline const std::vector<GrammarProduction>& getProductionsForNTC(int nonTerminalChar) const {
        DebugAssert(decodeNonTerminalId(nonTerminalChar) < 0 || decodeNonTerminalId(nonTerminalChar) >= m_nonTerminalChars.size(), "Requested nonterminal char does not exist");
        return m_grammarProductions[decodeNonTerminalId(nonTerminalChar)];
    }

    inline int getNonTerminalCharId(const std::string& nonTerminalChar) const {
        return encodeNonTerminalId(m_svFindInd(m_nonTerminalChars, nonTerminalChar));
    }

    inline int getTerminalCharId(const std::string& terminalChar) const {
        return m_svFindInd(m_terminalChars, terminalChar);
    }

    inline int getSynCharId(const std::string& synChar) const {
        return m_svFindInd(m_synChars, synChar);
    }

    static inline int decodeNonTerminalId(int id) {
        return id * -1 - 1;
    }

    static inline int encodeNonTerminalId(int id) {
        return id * -1 - 1;
    }

    static inline bool isTerminal(int ch) {
        return ch >= 0;
    }

    inline int getNonTerminalSize() const {
        return m_nonTerminalChars.size();
    }

    inline int getTerminalSize() const {
        return m_terminalChars.size();
    }

	inline int getSynSize() const {
		return m_synChars.size();
	}

    inline const std::string& getCharForId(int id) const {
        return isTerminal(id) ? m_terminalChars[id] : m_nonTerminalChars[decodeNonTerminalId(id)];
    }

    inline const GrammarProduction& getGrammarProduction(int leftChar, int ind) const {
        return m_grammarProductions[decodeNonTerminalId(leftChar)][ind];
    }
    inline const int getGrammarProductionSize(int leftChar, int ind) const {
        return m_grammarProductions[decodeNonTerminalId(leftChar)][ind].rightSide.size();
    }
    void print() {
        for(auto it : m_nonTerminalChars)
            std::cout << it << " position: " << getNonTerminalCharId(it) << std::endl;

        for(auto it : m_grammarProductions) {
            for(auto it2 : it) {
                for(auto it3 : it2.rightSide) 
                    std::cout << it3 << " ";
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }
};