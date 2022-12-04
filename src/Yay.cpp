#include <iostream>
#include <fstream>

#include "Grammar.hpp"
#include "TableBuilder.hpp"

int main()
{
    Grammar g;
    std::string inPath = "test/test1/examples/13ppjLang/test.san";
    std::ifstream inputFile(inPath);
    
    std::string outPath = "src/analizator/in.txt";
    
    g.loadGrammar(std::cin);

    TableBuilder tableBuilder(g);

    tableBuilder.generate();
    
    tableBuilder.outputTable(outPath);

    return 0;
}
