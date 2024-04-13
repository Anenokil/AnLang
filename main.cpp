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
        bool is_ok = true; // whether there is one more lexeme
        while (is_ok) {
            std::cout << get_lex(ifs, is_ok) << std::endl;
        }
    }
}
