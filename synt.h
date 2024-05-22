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
    NODE_ASSIGN,
    NODE_OPER_LOOP,
    NODE_OPER_IN,
    NODE_OPER_OUT,
    NODE_OPER_2,
    NODE_OPERAND,
    NODE_TYPE,
    NODE_VAR,
    NODE_VAR_INIT,
    NODE_EXPR,
    NODE_IF,
    NODE_FOR,
    NODE_WHILE,
    NODE_UNTIL,
    NODE_COND,
    NODE_BODY,
    NODE_FOR_INIT,
    NODE_FOR_COND,
    NODE_FOR_ITER,
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

    void err();
    void err(LexType exp);
    bool get_lex(std::ifstream & ifs, Lex & lex, bool to_throw_on_err = true);
    bool get_lex(std::ifstream & ifs, Lex & lex, LexType req_type, bool to_throw_on_err = true);

    void parse_program();
    void parse_scope(SyntTree * pst);
    void parse_statement(SyntTree * pst);
    void parse_decl(SyntTree * pst);
    void parse_assign(SyntTree * pst);
    void parse_if(SyntTree * pst);
    void parse_var(SyntTree * pst);
    void parse_var_init(SyntTree * pst);
    void parse_expr(SyntTree * pst);

public:
    Parser(std::ifstream & ifs);
    friend std::ostream & operator<<(std::ostream & os, Parser const & par);
};

#endif
