#include <iostream>

#include "Grammar.hpp"
#include "DfaBuilder.hpp"

int main(int argc, char const *argv[])
{
    std::cout << "Hello World!!" << std::endl;

    Grammar g;

    g.loadGrammar(std::cin);

    DfaBuilder dfaBuilder(g);

    std::set<int> dst;
    std::vector<int> chSeries;

    chSeries.push_back(g.getNonTerminalCharId("<A>"));

    dfaBuilder.getFirst(chSeries, dst);

    for(auto ch : dst) {
        std::cout << g.getCharForId(ch) << ' ';
    }
    std::cout << std::endl;

    return 0;
}
