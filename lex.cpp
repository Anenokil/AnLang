#include <cstring> // strchr()
#include "lex.h"

const char ESCAPEBLE_SYMS[] = "\\nt\"";
const char SPEC_SYMS[] = "+-*/%=<>!~(){};,";

enum flag_vals {
    NAME,
    NUM,
    TEXT,
    TEXT_ESCAPE,
    TEXT_END,
    SPEC,
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
    return strchr(ESCAPEBLE_SYMS, c);
}

bool is_spec(char c)
{
    return strchr(SPEC_SYMS, c);
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
    } else if (is_spec(c)) {
        return SPEC;
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
        return is_spec(c);
    }
    return false; /* unknown flag */
}

std::string get_lex(std::ifstream & ifs, bool & ret)
{
    static char c = ' ';
    while (!ifs.eof() && std::isspace(c)) {
        c = ifs.get();
    }

    flag_vals flag = get_flag(c);
    if (flag == ERROR) {
        ret = false;
        return "";
    }

    std::string res = "";
    do {
        res += c;
        c = ifs.get();
    } while (!ifs.eof() && has_correct_flag(c, flag));
    if (flag == ERROR) {
        ret = false;
        return "";
    }
    return res;
}
