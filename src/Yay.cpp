#include <iostream>
#include <fstream>

#include "Grammar.hpp"
#include "TableBuilder.hpp"

int main()
{
    Grammar g;

    std::string outPath = "src/analizator/in.txt";
    
    g.loadGrammar(std::cin);

    TableBuilder tableBuilder(g);

    tableBuilder.generate();
    
    tableBuilder.outputTable(outPath);

    return 0;
}
