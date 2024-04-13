#include <cstring> // strchr()
#include "lex.h"

const char ESCAPEBLE_CHARS[] = "\\nt\""; // chars that may appear after the escape char
const char SPEC_BEG_CHARS[] = "=!<>"; // chars that double special lexemes may start with
const char SPEC_END_CHARS[] = "+-*/%=<>~(){};,"; // chars that may end special lexemes

enum flag_vals {
    NAME,
    NUM,
    TEXT,
    TEXT_ESCAPE,
    TEXT_END,
    SPEC,
    SPEC_END,
    ERROR,
};

bool is_point(char c)
{
    return c == '.';
}

bool is_quot(char c)
{
    return c == '"';
}

bool is_escape(char c)
{
    return c == '\\';
}

bool is_escapeble(char c)
{
    return strchr(ESCAPEBLE_CHARS, c);
}

bool is_spec_beg(char c)
{
    return strchr(SPEC_BEG_CHARS, c);
}

bool is_spec_end(char c)
{
    return strchr(SPEC_END_CHARS, c);
}

bool is_num_quant(char c)
{
    return std::isdigit(c) || is_point(c);
}

bool is_name_quant(char c)
{
    return std::isalpha(c) || c == '_';
}

flag_vals get_flag(char c)
{
    if (is_name_quant(c)) {
        return NAME;
    } else if (is_num_quant(c)) {
        return NUM;
    } else if (is_quot(c)) {
        return TEXT;
    } else if (is_spec_beg(c)) {
        return SPEC;
    } else if (is_spec_end(c)) {
        return SPEC_END;
    } else {
        return ERROR;
    }
}

bool has_correct_flag(char c, flag_vals & flag)
{
    if (flag == NAME) {
        return is_name_quant(c);
    } else if (flag == NUM) {
        return is_num_quant(c);
    } else if (flag == TEXT) {
        if (is_escape(c)) {
            flag = TEXT_ESCAPE;
        } else if (is_quot(c)) {
            flag = TEXT_END;
        }
        return true;
    } else if (flag == TEXT_ESCAPE) {
        if (is_escapeble(c)) {
            flag = TEXT;
            return true;
        }
        flag = ERROR;
        return false;
    } else if (flag == TEXT_END) {
        return false;
    } else if (flag == SPEC) {
        flag = SPEC_END;
        return is_spec_end(c);
    } else if (flag == SPEC_END) {
        return false;
    }
    return false; /* unknown flag */
}

std::string get_lex(std::ifstream & ifs, ret_vals & ret)
{
    static char c = ' ';

    /* read space characters */
    while (!ifs.eof() && std::isspace(c)) {
        c = ifs.get();
    }
    if (ifs.eof()) {
        ret = RET_EOF;
        return "";
    }

    /* read the first non-space character */
    flag_vals flag = get_flag(c);
    if (flag == ERROR) {
        ret = RET_ERR;
        return "";
    }

    /* read following characters */
    std::string res = "";
    do {
        res += c;
        c = ifs.get();
    } while (!ifs.eof() && has_correct_flag(c, flag));
    if (flag == ERROR) {
        ret = RET_ERR;
        return "";
    }

    /* return a result */
    if (ifs.eof()) {
        ret = RET_EOF;
    }
    ret = RET_OK;
    return res;
}
