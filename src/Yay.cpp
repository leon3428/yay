#include <iostream>

#include "Grammar.hpp"
#include "TableBuilder.hpp"

int main(int argc, char const *argv[])
{
    std::cout << "Hello World!!" << std::endl;

    Grammar g;

    g.loadGrammar(std::cin);

    TableBuilder tableBuilder(g);

    std::set<LR1State> state, dst;
    state.emplace(-2, 0, 0, 0);
    tableBuilder.closure(state);

    tableBuilder.gotoState(state, g.getNonTerminalCharId("<S>"), dst);

    for(const LR1State& s : dst) {
        std::cout << g.getCharForId(s.grammarProductionLeft) << " -> ";
        auto &gp = g.getGrammarProduction(s.grammarProductionLeft, s.grammarProductionId);
        for(int i = 0;i < gp.rightSide.size(); i++) {
            if(i == s.dotPosition)
                std::cout << ". ";
            std::cout << g.getCharForId(gp.rightSide[i]) << ' ';
        }    
        std::cout << ", " << g.getCharForId(s.followChar) << '\n';    
    }

    return 0;
}
