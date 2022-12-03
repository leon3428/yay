#include <iostream>
#include <fstream>

#include "Grammar.hpp"
#include "TableBuilder.hpp"

int main(int argc, char const *argv[])
{
    std::cout << "Hello World!!" << std::endl;

    Grammar g;
    std::string inPath = "test/test1/examples/09redred/test.san";
    std::string outPath = "src/analizator/in.txt";
    std::ifstream inputFile(inPath);
    g.loadGrammar(inputFile);
    inputFile.close();

    TableBuilder tableBuilder(g);
    tableBuilder.generate();
    tableBuilder.outputTable(outPath);

    return 0;
}
