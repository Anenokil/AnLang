#ifndef LEX_H
#define LEX_H

#include <fstream>
#include <string>

enum RetVal {
    RET_OK,
    RET_ERR,
    RET_EOF,
};

enum LexType {
    LEX_UNKNOWN,
    LEX_BEGIN,
    LEX_TYPE,
    LEX_VAR,
    LEX_CONST,
    LEX_OPER_COMMA,
    LEX_OPER_END,
    LEX_OPER_LOOP,
    LEX_OPER_IN,
    LEX_OPER_OUT,
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

class Lex
{
private:
    std::string word_{};
    LexType type_{};
    unsigned row_{};
    unsigned col_{};

    /* the function receives a lexeme and returns its type */
    LexType define_lex_type(std::string const & lex);
    
public:
    Lex(std::string const & str, unsigned row, unsigned col);
    std::string word() const;
    LexType type() const;
    unsigned row() const;
    unsigned col() const;
};

/* the function gets one lexeme from file and returns it;
* sets 'ret' value;
* if to_throw sets on true, throws an error instead of RET_ERR returns */
Lex get_lex(std::ifstream & ifs, RetVal & ret, bool to_throw = true);

#endif
