#ifndef LEX_H
#define LEX_H

#include <fstream>
#include <string>

enum ret_vals {
    RET_OK,
    RET_ERR,
    RET_EOF,
};

enum lex_types {
    LEX_UNKNOWN,
    LEX_BEGIN,
    LEX_TYPE,
    LEX_VAR,
    LEX_CONST,
    LEX_OPER_COMMA,
    LEX_OPER_END,
    LEX_OPER_0,
    LEX_OPER_1,
    LEX_OPER_2_NORET,
    LEX_OPER_2_RET,
    LEX_IF,
    LEX_ELSE,
    LEX_FOR,
    LEX_WHILE,
    LEX_DO,
    LEX_UNTIL,
    LEX_SCOPE_L,
    LEX_SCOPE_R,
    LEX_PARENTHESIS_L,
    LEX_PARENTHESIS_R,
};

/* the function gets one lexeme from file and returns it;
 * sets 'ret' value */
std::string get_lex(std::ifstream & ifs, ret_vals & ret);

/* the function receives a lexeme and returns its type */
lex_types define_lex_type(std::string const & lex);

#endif
