#include "chtype.h"
#include <cstring> // strchr()
#include <string> // isdigit(), isalpha()

const char ESCAPEBLE_CHARS[] = "\\nt\""; // chars that may appear after the escape char
const char SPEC_BEG_CHARS[] = "=!<>"; // chars that double special lexemes may start with
const char SPEC_END_CHARS[] = "+-*/%=<>~(){};,"; // chars that may end special lexemes

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
