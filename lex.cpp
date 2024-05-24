#include "lex.h"

#include "reserved.h"
#include "chtype.h"

enum FlagVal {
    FL_ID,
    FL_NUM,
    FL_TEXT,
    FL_TEXT_ESCAPE,
    FL_TEXT_END,
    FL_SPEC,
    FL_SPEC_END,
    FL_ERROR,
};

bool is_type(LexType lt) {
    return lt == LEX_INT || lt == LEX_FLOAT || lt == LEX_BOOL || lt == LEX_STR;
}

bool is_const(LexType lt) {
    return lt == LEX_INT_CONST || lt == LEX_FLOAT_CONST || lt == LEX_BOOL_CONST || lt == LEX_STR_CONST;
}

bool is_oper_1(LexType lt) {
    return lt == LEX_ADD || lt == LEX_SUB || lt == LEX_NOT;
}

bool is_oper_2_ret(LexType lt) {
    return lt == LEX_ASSIGN;
}

bool is_oper_2_noret(LexType lt) {
    return lt == LEX_ADD || lt == LEX_SUB || lt == LEX_MULT || lt == LEX_DIV || lt == LEX_REM ||
            lt == LEX_EQ || lt == LEX_NE || lt == LEX_L || lt == LEX_LE || lt == LEX_G || lt == LEX_GE ||
            lt == LEX_AND || lt == LEX_OR;
}

bool _is_id(std::string const & word)
{
    for (char c: word) {
        if (!is_id_quant(c)) {
            return false;
        }
    }
    return true;
}

bool _is_int_const(std::string const & word)
{
    for (char c: word) {
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

bool _is_float_const(std::string const & word)
{
    bool has_point = false;
    for (char c: word) {
        if (is_point(c)) {
            if (has_point) {
                return false;
            }
            has_point = true;
        } else if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

bool _is_bool_const(std::string const & word)
{
    return word == rw::BOOL_TRUE || word == rw::BOOL_FALSE;
}

bool _is_str_const(std::string const & word)
{
    return word.length() >= 2 && word[0] == '"' && word[word.length() - 1] == '"';
}

Lex::Lex(std::string const & word, LexType type, unsigned row, unsigned col): word_(word), type_(type), row_(row), col_(col)
{}

/* the function receives a lexeme and returns its type */
LexType Lex::define_lex_type()
{
    if (word_ == rw::BEGIN) return LEX_BEGIN;
    if (word_ == rw::TYPE_INT) return LEX_INT;
    if (word_ == rw::TYPE_FLOAT) return LEX_FLOAT;
    if (word_ == rw::TYPE_BOOL) return LEX_BOOL;
    if (word_ == rw::TYPE_STR) return LEX_STR;
    if (word_ == rw::OPER_COMMA) return LEX_COMMA;
    if (word_ == rw::OPER_SEMICOLON) return LEX_SEMICOLON;
    if (word_ == rw::OPER_BREAK) return LEX_BREAK;
    if (word_ == rw::OPER_CONT) return LEX_CONTINUE;
    if (word_ == rw::OPER_READ) return LEX_OPER_IN;
    if (word_ == rw::OPER_WRITE) return LEX_OPER_OUT;
    if (word_ == rw::OPER_ADD) return LEX_ADD;
    if (word_ == rw::OPER_SUB) return LEX_SUB;
    if (word_ == rw::OPER_MULT) return LEX_MULT;
    if (word_ == rw::OPER_DIV) return LEX_DIV;
    if (word_ == rw::OPER_REM) return LEX_REM;
    if (word_ == rw::OPER_EQ) return LEX_EQ;
    if (word_ == rw::OPER_NE) return LEX_NE;
    if (word_ == rw::OPER_L) return LEX_L;
    if (word_ == rw::OPER_LE) return LEX_LE;
    if (word_ == rw::OPER_G) return LEX_G;
    if (word_ == rw::OPER_GE) return LEX_GE;
    if (word_ == rw::OPER_NOT) return LEX_NOT;
    if (word_ == rw::OPER_AND) return LEX_AND;
    if (word_ == rw::OPER_OR) return LEX_OR;
    if (word_ == rw::OPER_ASSIGN) return LEX_ASSIGN;
    if (word_ == rw::OPER_IF) return LEX_IF;
    if (word_ == rw::OPER_ELSE) return LEX_ELSE;
    if (word_ == rw::OPER_FOR) return LEX_FOR;
    if (word_ == rw::OPER_WHILE) return LEX_WHILE;
    if (word_ == rw::OPER_DO) return LEX_DO;
    if (word_ == rw::OPER_UNTIL) return LEX_UNTIL;
    if (word_ == rw::SCOPE_BEG) return LEX_BRACE_L;
    if (word_ == rw::SCOPE_END) return LEX_BRACE_R;
    if (word_ == rw::PAR_BEG) return LEX_PARENTHESIS_L;
    if (word_ == rw::PAR_END) return LEX_PARENTHESIS_R;
    if (_is_int_const(word_)) return LEX_INT_CONST;
    if (_is_float_const(word_)) return LEX_FLOAT_CONST;
    if (_is_bool_const(word_)) return LEX_BOOL_CONST;
    if (_is_str_const(word_)) return LEX_STR_CONST;
    if (_is_id(word_)) return LEX_ID;
    throw std::runtime_error(std::to_string(row_) + " line, " + std::to_string(col_) +
            " column: Unknown lexeme: '" + word_ + "'.");
}

Lex::Lex(): word_(""), type_(LEX_NULL), row_(0), col_(0)
{}

Lex::Lex(std::string const & str, unsigned row, unsigned col): word_(str), row_(row), col_(col)
{
    type_ = define_lex_type();
}

Lex Lex::eof(unsigned row, unsigned col)
{
    return Lex("", LEX_EOF, row, col);
}

std::string Lex::word() const
{
    return word_;
}

LexType Lex::type() const
{
    return type_;
}

unsigned Lex::row() const
{
    return row_;
}

unsigned Lex::col() const
{
    return col_;
}

FlagVal _get_flag(char c)
{
    if (is_id_quant(c)) {
        return FL_ID;
    } else if (is_num_quant(c)) {
        return FL_NUM;
    } else if (is_quot(c)) {
        return FL_TEXT;
    } else if (is_spec_beg(c)) {
        return FL_SPEC;
    } else if (is_spec_end(c)) {
        return FL_SPEC_END;
    } else {
        return FL_ERROR;
    }
}

bool _has_correct_flag(char c, FlagVal & flag)
{
    if (flag == FL_ID) {
        return is_id_quant(c);
    } else if (flag == FL_NUM) {
        return is_num_quant(c);
    } else if (flag == FL_TEXT) {
        if (is_escape(c)) {
            flag = FL_TEXT_ESCAPE;
        } else if (is_quot(c)) {
            flag = FL_TEXT_END;
        }
        return true;
    } else if (flag == FL_TEXT_ESCAPE) {
        if (is_escapable(c)) {
            flag = FL_TEXT;
            return true;
        }
        flag = FL_ERROR;
        return false;
    } else if (flag == FL_TEXT_END) {
        return false;
    } else if (flag == FL_SPEC) {
        flag = FL_SPEC_END;
        return c == '=';
    } else if (flag == FL_SPEC_END) {
        return false;
    }
    return false; /* unknown flag */
}

void Scanner::iter()
{
    c_ = ifs_.get();
    if (c_ == '\n') {
        col_ = 0;
        ++row_;
    } else {
        ++col_;
    }
}

Scanner::Scanner(std::ifstream & ifs): ifs_(ifs), row_(1), col_(0), c_(' ')
{}

/* the function gets one lexeme from file;
* returns true if OK and false if an error appears;
* if to_throw sets on true, throws an error instead of RET_ERR returns */
bool Scanner::get_lex(Lex & lex, bool to_throw)
{
    /* read space characters */
    while (!ifs_.eof() && (std::isspace(c_) || is_comment_beg(c_))) {
        if (is_comment_beg(c_)) {
            iter();
            while (!ifs_.eof() && !is_comment_end(c_)) {
                iter();
            }
        }
        iter();
    }
    if (ifs_.eof()) {
        lex = Lex::eof(row_, col_);
        return true;
    }

    /* read the first non-space character */
    FlagVal flag = _get_flag(c_);
    if (flag == FL_ERROR) {
        if (to_throw) {
            throw std::runtime_error(std::to_string(row_) + " line, " + std::to_string(col_) +
                    " column: Unexpected character '" + std::string(1, c_) + "'.");
        }
        lex = Lex("", row_, col_);
        return false;
    }
    unsigned const row_lex = row_;
    unsigned const col_lex = col_;

    /* read following characters */
    std::string res = "";
    do {
        res += c_;
        iter();
    } while (!ifs_.eof() && _has_correct_flag(c_, flag));
    if (flag == FL_ERROR) {
        if (to_throw) {
            throw std::runtime_error(std::to_string(row_) + " line, " + std::to_string(col_) +
                    " column: Unexpected character '" + std::string(1, c_) + "'.");
        }
        lex = Lex("", row_, col_);
        return false;
    }

    /* return a result */
    if (ifs_.eof()) {
        lex = Lex::eof(row_lex, col_lex);
    }
    lex = Lex(res, row_lex, col_lex);
    return true;
}
