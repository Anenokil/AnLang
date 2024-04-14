#include "lex.h"
#include "chtype.h"

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

flag_vals _get_flag(char c)
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

bool _has_correct_flag(char c, flag_vals & flag)
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
