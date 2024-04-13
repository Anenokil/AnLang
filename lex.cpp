#include "lex.h"
#include <cstring> // strchr()

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

bool _is_point(char c)
{
    return c == '.';
}

bool _is_quot(char c)
{
    return c == '"';
}

bool _is_escape(char c)
{
    return c == '\\';
}

bool _is_escapeble(char c)
{
    return strchr(ESCAPEBLE_CHARS, c);
}

bool _is_spec_beg(char c)
{
    return strchr(SPEC_BEG_CHARS, c);
}

bool _is_spec_end(char c)
{
    return strchr(SPEC_END_CHARS, c);
}

bool _is_num_quant(char c)
{
    return std::isdigit(c) || _is_point(c);
}

bool _is_name_quant(char c)
{
    return std::isalpha(c) || c == '_';
}

flag_vals _get_flag(char c)
{
    if (_is_name_quant(c)) {
        return NAME;
    } else if (_is_num_quant(c)) {
        return NUM;
    } else if (_is_quot(c)) {
        return TEXT;
    } else if (_is_spec_beg(c)) {
        return SPEC;
    } else if (_is_spec_end(c)) {
        return SPEC_END;
    } else {
        return ERROR;
    }
}

bool _has_correct_flag(char c, flag_vals & flag)
{
    if (flag == NAME) {
        return _is_name_quant(c);
    } else if (flag == NUM) {
        return _is_num_quant(c);
    } else if (flag == TEXT) {
        if (_is_escape(c)) {
            flag = TEXT_ESCAPE;
        } else if (_is_quot(c)) {
            flag = TEXT_END;
        }
        return true;
    } else if (flag == TEXT_ESCAPE) {
        if (_is_escapeble(c)) {
            flag = TEXT;
            return true;
        }
        flag = ERROR;
        return false;
    } else if (flag == TEXT_END) {
        return false;
    } else if (flag == SPEC) {
        flag = SPEC_END;
        return _is_spec_end(c);
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
    flag_vals flag = _get_flag(c);
    if (flag == ERROR) {
        ret = RET_ERR;
        return "";
    }

    /* read following characters */
    std::string res = "";
    do {
        res += c;
        c = ifs.get();
    } while (!ifs.eof() && _has_correct_flag(c, flag));
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
