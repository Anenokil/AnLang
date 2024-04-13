#ifndef LEX_H
#define LEX_H

#include <fstream>
#include <string>

enum ret_vals {
    RET_OK,
    RET_ERR,
    RET_EOF,
};

/* the function gets one lexeme from file and returns it;
 * sets 'ret' value */
std::string get_lex(std::ifstream & ifs, ret_vals & ret);

#endif
