#include <iostream>
#include <fstream>
#include <string>
#include "synt.h"

int main()
{
    std::cout << "Testing program" << std::endl;
    std::cout << "Enter a filename: ";
    /* recieve input file name */
    std::string ifn;
    std::cin >> ifn;
    /* open file */
    std::ifstream ifs(ifn);
    if (ifs.is_open()) {
        /* create SyntTree and print it */
        SyntTree st(ifs);
        std::cout << st;
    }
}
