#ifndef SYNT_H
#define SYNT_H

#include <iostream>
#include <fstream>
#include <string>
#include "tid.h"
#include "lex.h"

enum NodeType {
    NODE_TMP, // initial temporary value
    NODE_BEGIN,
    NODE_SCOPE,
    NODE_DECL,
    NODE_DECL_ID,
    NODE_DECL_INIT,
    NODE_ASSIGN,
    NODE_IF,
    NODE_FOR,
    NODE_FOR_INIT,
    NODE_FOR_COND,
    NODE_FOR_ITER,
    NODE_WHILE,
    NODE_UNTIL,
    NODE_BREAK,
    NODE_CONTINUE,
    NODE_OPER_IN,
    NODE_OPER_OUT,
    NODE_EXPR,
    NODE_OPERAND,
    NODE_OPER_2,
};

class SyntTree
{
private:
    NodeType type{};
    SyntTree * successors{};
    unsigned suc_cnt{};

    void del_all_suc();
    SyntTree & assign(SyntTree const & st);
    std::string get_typename() const;
    std::ostream & print(std::ostream & os, unsigned tab = 0) const;

    SyntTree(SyntTree const & st);
    SyntTree & operator=(SyntTree const & st);

public:
    std::string lex{};
    SyntTree * predecessor{};

    SyntTree(NodeType type = NODE_BEGIN, std::string lex = "", SyntTree * predecessor = nullptr);
    ~SyntTree();
    SyntTree * add_suc(NodeType type = NODE_TMP, std::string lex = "");
    friend std::ostream & operator<<(std::ostream & os, SyntTree const & st);
};

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
