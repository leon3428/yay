#include <iostream>
#include <fstream>

#include "Grammar.hpp"
#include "TableBuilder.hpp"

int main()
{
    Grammar g;
    std::string inPath = "test/test1/examples/11minusLang_2/test.san";
    std::string outPath = "src/analizator/in.txt";
    std::ifstream inputFile(inPath);
    g.loadGrammar(std::cin);
    inputFile.close();

    TableBuilder tableBuilder(g);
    tableBuilder.generate();
    tableBuilder.outputTable(outPath);

    return 0;
}
