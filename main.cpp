#include <iostream>
#include <fstream>
#include <string>
#include "tid.h"
#include "synt.h"

int main()
{
    std::cout << "Testing program" << std::endl;
    std::cout << "Enter a filename: ";
    /* receive input file name */
    std::string ifn;
    std::cin >> ifn;
    /* open file */
    std::ifstream ifs(ifn);
    if (ifs.is_open()) {
        /* form SyntTree and TID and print them */
        TID tid;
        Parser par(ifs, tid);
        std::cout << par;
    }
}
