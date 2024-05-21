#include "lex.h"

#include "reserved.h"
#include "chtype.h"

enum FlagVal {
    FL_NAME,
    FL_NUM,
    FL_TEXT,
    FL_TEXT_ESCAPE,
    FL_TEXT_END,
    FL_SPEC,
    FL_SPEC_END,
    FL_ERROR,
};

bool _is_var_name(std::string const & lex)
{
    for (char c: lex) {
        if (!is_name_quant(c)) {
            return false;
        }
    }
    return true;
}

bool _is_num_const(std::string const & lex)
{
    bool has_point = false;
    for (char c: lex) {
        if (is_point(c)) {
            if (has_point) {
                return false;
            }
            has_point = true;
        } if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

bool _is_str_const(std::string const & lex)
{
    return lex.length() >= 2 && lex[0] == '"' && lex[lex.length() - 1] == '"';
}

LexType Lex::define_lex_type(std::string const & lex)
{
    if (lex == "") return LEX_UNKNOWN;
    if (lex == rw::BEGIN) return LEX_BEGIN;
    if (lex == rw::OPER_COMMA) return LEX_OPER_COMMA;
    if (lex == rw::OPER_END) return LEX_OPER_END;
    if (lex == rw::OPER_BREAK || lex == rw::OPER_CONT) return LEX_OPER_LOOP;
    if (lex == rw::OPER_READ) return LEX_OPER_IN;
    if (lex == rw::OPER_WRITE) return LEX_OPER_OUT;
    if (lex == rw::OPER_ADD || lex == rw::OPER_SUB || lex == rw::OPER_MULT || lex == rw::OPER_DIV || lex == rw::OPER_REM ||
        lex == rw::OPER_AND || lex == rw::OPER_OR || lex == rw::OPER_NOT ||
        lex == rw::OPER_EQ || lex == rw::OPER_NE || lex == rw::OPER_L || lex == rw::OPER_LE || lex == rw::OPER_G || lex == rw::OPER_GE) return LEX_OPER_2_NORET;
    if (lex == rw::OPER_ASSIGN) return LEX_OPER_2_RET;
    if (lex == rw::OPER_IF) return LEX_IF;
    if (lex == rw::OPER_ELSE) return LEX_ELSE;
    if (lex == rw::OPER_FOR) return LEX_FOR;
    if (lex == rw::OPER_WHILE) return LEX_WHILE;
    if (lex == rw::OPER_DO) return LEX_DO;
    if (lex == rw::OPER_UNTIL) return LEX_UNTIL;
    if (lex == rw::SCOPE_BEG) return LEX_SCOPE_L;
    if (lex == rw::SCOPE_END) return LEX_SCOPE_R;
    if (lex == rw::PAR_BEG) return LEX_PARENTHESIS_L;
    if (lex == rw::PAR_END) return LEX_PARENTHESIS_R;
    if (lex == rw::TYPE_INT || lex == rw::TYPE_FLOAT || lex == rw::TYPE_BOOL || lex == rw::TYPE_STR) return LEX_TYPE;
    if (lex == rw::BOOL_TRUE || lex == rw::BOOL_FALSE || _is_num_const(lex) || _is_str_const(lex)) return LEX_CONST;
    if (_is_var_name(lex)) return LEX_VAR;
    return LEX_UNKNOWN;
}

Lex::Lex(): word_(""), type_(LEX_TMP), row_(0), col_(0)
{}

Lex::Lex(std::string const & str, unsigned row, unsigned col): word_(str), type_(define_lex_type(str)), row_(row), col_(col)
{}

Lex::Lex(unsigned row, unsigned col): word_(""), type_(LEX_EOF), row_(row), col_(col)
{}

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
    if (is_name_quant(c)) {
        return FL_NAME;
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
    if (flag == FL_NAME) {
        return is_name_quant(c);
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
        return is_spec_end(c);
    } else if (flag == FL_SPEC_END) {
        return false;
    }
    return false; /* unknown flag */
}

bool get_lex(std::ifstream & ifs, Lex & lex, bool to_throw)
{
    static unsigned row = 1;
    static unsigned col = 0;
    static char c = ' ';
    static auto iter = [&ifs](){
        c = ifs.get();
        if (c == '\n') {
            col = 0;
            ++row;
        } else {
            ++col;
        }
    };

    /* read space characters */
    while (!ifs.eof() && (std::isspace(c) || is_comment_beg(c))) {
        if (is_comment_beg(c)) {
            iter();
            while (!ifs.eof() && !is_comment_end(c)) {
                iter();
            }
        }
        iter();
    }
    if (ifs.eof()) {
        lex = Lex(row, col);
        return true;
    }

    /* read the first non-space character */
    FlagVal flag = _get_flag(c);
    if (flag == FL_ERROR) {
        if (to_throw) {
            throw std::runtime_error(std::to_string(row) + " line, " + std::to_string(col) + " column: Unexpected character '" + std::string(1, c) + "'.");
        }
        lex = Lex("", row, col);
        return false;
    }
    unsigned const row_lex = row;
    unsigned const col_lex = col;

    /* read following characters */
    std::string res = "";
    do {
        res += c;
        iter();
    } while (!ifs.eof() && _has_correct_flag(c, flag));
    if (flag == FL_ERROR) {
        if (to_throw) {
            throw std::runtime_error(std::to_string(row) + " line, " + std::to_string(col) + " column: Unexpected character '" + std::string(1, c) + "'.");
        }
        lex = Lex("", row, col);
        return false;
    }

    /* return a result */
    if (ifs.eof()) {
        lex = Lex(row_lex, col_lex);
    }
    lex = Lex(res, row_lex, col_lex);
    return true;
}
