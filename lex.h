#ifndef LEX_H
#define LEX_H

#include <fstream>
#include <string>

enum LexType {
    LEX_NULL, // initial temporary value
    LEX_BEGIN,
    LEX_INT, LEX_FLOAT, LEX_BOOL, LEX_STR,
    LEX_ID,
    LEX_INT_CONST, LEX_FLOAT_CONST, LEX_BOOL_CONST, LEX_STR_CONST,
    LEX_COMMA,
    LEX_SEMICOLON,
    LEX_BREAK, LEX_CONTINUE,
    LEX_OPER_IN, LEX_OPER_OUT,
    LEX_ADD, LEX_SUB, LEX_MULT, LEX_DIV, LEX_REM,
    LEX_EQ, LEX_NE, LEX_L, LEX_LE, LEX_G, LEX_GE,
    LEX_NOT, LEX_AND, LEX_OR,
    LEX_ASSIGN,
    LEX_IF, LEX_ELSE,
    LEX_FOR,
    LEX_WHILE,
    LEX_DO, LEX_UNTIL,
    LEX_BRACE_L, LEX_BRACE_R,
    LEX_PARENTHESIS_L, LEX_PARENTHESIS_R,
    LEX_EOF,
};

bool is_type(LexType lt);
bool is_const(LexType lt);
bool is_oper_1(LexType lt);
bool is_oper_2_ret(LexType lt);
bool is_oper_2_noret(LexType lt);

class Lex
{
private:
    std::string word_{};
    LexType type_{};
    unsigned row_{};
    unsigned col_{};

    Lex(std::string const & word, LexType type, unsigned row, unsigned col);
    LexType define_lex_type();
    
public:
    Lex();
    Lex(std::string const & str, unsigned row, unsigned col);
    static Lex eof(unsigned row, unsigned col);
    std::string word() const;
    LexType type() const;
    unsigned row() const;
    unsigned col() const;
};

class Scanner
{
private:
    std::ifstream & ifs_;
    Lex lex_{};
    char c_{};
    unsigned row_{};
    unsigned col_{};

    void iter();

public:
    explicit Scanner(std::ifstream & ifs);
    bool get_lex(Lex & lex, bool to_throw = true);
};

#endif
