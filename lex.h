#ifndef LEX_H
#define LEX_H

#include <fstream>
#include <string>

enum LexType {
    LEX_TMP, // initial temporary value
    LEX_BEGIN,
    LEX_TYPE,
    LEX_ID,
    LEX_INT_CONST,
    LEX_FLOAT_CONST,
    LEX_BOOL_CONST,
    LEX_STR_CONST,
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
    LEX_EOF,
};

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
