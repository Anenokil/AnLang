#ifndef RESERVED_WORDS_H
#define RESERVED_WORDS_H

#include <string>

namespace rw
{
    std::string const BEGIN = "program";

    std::string const TYPE_INT = "int";
    std::string const TYPE_FLOAT = "float";
    std::string const TYPE_BOOL = "bool";
    std::string const TYPE_STR = "str";

    std::string const BOOL_TRUE = "true";
    std::string const BOOL_FALSE = "false";

    std::string const OPER_IF = "if";
    std::string const OPER_ELSE = "else";
    std::string const OPER_FOR = "for";
    std::string const OPER_WHILE = "while";
    std::string const OPER_DO = "do";
    std::string const OPER_UNTIL = "until";
    std::string const OPER_BREAK = "break";
    std::string const OPER_CONT = "continue";
    std::string const OPER_SCAN = "read";
    std::string const OPER_PRINT = "write";

    std::string const OPER_ADD = "+";
    std::string const OPER_SUB = "-";
    std::string const OPER_MULT = "*";
    std::string const OPER_DIV = "/";
    std::string const OPER_REM = "%";
    std::string const OPER_AND = "*";
    std::string const OPER_OR = "+";
    std::string const OPER_NOT = "~";
    std::string const OPER_EQ = "==";
    std::string const OPER_NE = "!=";
    std::string const OPER_L = "<";
    std::string const OPER_LE = "<=";
    std::string const OPER_G = ">";
    std::string const OPER_GE = ">=";
    std::string const OPER_ASSIGN = "=";
    std::string const OPER_COMMA = ",";
    std::string const OPER_END = ";";

    std::string const PAR_BEG = "(";
    std::string const PAR_END = ")";
    std::string const SCOPE_BEG = "{";
    std::string const SCOPE_END = "}";
}

#endif
