#ifndef LEX_H
#define LEX_H

#include <fstream>
#include <string>

/* the function gets one lexeme from file and returns it;
 * if there is the end of file, sets 'ret' on false, else sets it on true */
std::string get_lex(std::ifstream & ifs, bool & ret);

#endif
