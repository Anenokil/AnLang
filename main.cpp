#include <iostream>
#include <fstream>
#include <string>
#include "lex.h"

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
        /* recieve lexemes and print them */
        ret_vals ret = RET_OK;
        while (ret == RET_OK) {
            std::cout << get_lex(ifs, ret) << std::endl;
        }
        /* check that reading ended normally */
        if (ret == RET_ERR) {
            std::cout << ">>> ERROR" << std::endl;
        } else if (ret == RET_EOF) {
            std::cout << ">>> EOF" << std::endl;
        }
    }
}
