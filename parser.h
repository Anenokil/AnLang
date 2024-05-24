#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include "tid.h"
#include "lex.h"
#include "synt_tree.h"

class Parser
{
private:
    std::ifstream & ifs;
    TID tid{};
    SyntTree st{};
    Lex cur_lex{};
    Scanner sc;
    unsigned loop_depth{};

    void err();
    void err(LexType exp);
    void err(std::string const & cause);
    bool get_lex();
    bool get_lex(LexType req_type);

    void parse_program();
    void parse_scope(SyntTree * pst);
    void parse_statement(SyntTree * pst);
    void parse_decl(SyntTree * pst);
    void parse_decl_id(SyntTree * pst);
    void parse_decl_init(SyntTree * pst);
    void parse_assign(SyntTree * pst);
    void parse_if(SyntTree * pst);
    void parse_for(SyntTree * pst);
    void parse_for_init(SyntTree * pst);
    void parse_while(SyntTree * pst);
    void parse_until(SyntTree * pst);
    void parse_break(SyntTree * pst);
    void parse_continue(SyntTree * pst);
    void parse_oper_in(SyntTree * pst);
    void parse_oper_out(SyntTree * pst);
    void parse_expr(SyntTree * pst);
    void parse_expr2(SyntTree * pst, bool is_lval);

public:
    Parser(std::ifstream & ifs);
    friend std::ostream & operator<<(std::ostream & os, Parser const & par);
};

#endif
