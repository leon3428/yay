#include <iostream>

#include "Grammar.hpp"

int main(int argc, char const *argv[])
{
    std::cout << "Hello World!!" << std::endl;

    Grammar g;

    g.loadGrammar(std::cin);

    return 0;
}
