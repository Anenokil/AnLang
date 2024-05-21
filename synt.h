#ifndef SYNT_H
#define SYNT_H

#include <iostream>
#include <fstream>
#include <string>
#include "tid.h"
#include "lex.h"

enum NodeType {
    NODE_TMP,
    NODE_BEGIN,
    NODE_SCOPE,
    NODE_DECL,
    NODE_OPER_LOOP,
    NODE_OPER_IN,
    NODE_OPER_OUT,
    NODE_OPER_2,
    NODE_OPERAND,
    NODE_ASSIGN,
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
    std::string lex{};
    SyntTree * predecessor{};
    SyntTree * successors{};
    unsigned suc_cnt{};

    SyntTree * add_suc(NodeType type = NODE_TMP, std::string lex = "");
    void del_all_suc();
    SyntTree & assign(SyntTree const & st);
    std::string get_typename() const;
    std::ostream & print(std::ostream & os, unsigned tab = 0) const;

    SyntTree(NodeType type = NODE_BEGIN, std::string lex = "", SyntTree * predecessor = nullptr);
    SyntTree(SyntTree const & st);
    SyntTree & operator=(SyntTree const & st);

    friend void parse_scope(std::ifstream & ifs, TID & tid, SyntTree * pst, Lex & lex);
    friend void parse_statement(std::ifstream & ifs, TID & tid, SyntTree * pst, Lex & lex);
    friend void parse_if(std::ifstream & ifs, TID & tid, SyntTree * pst, Lex & lex);
    friend void parse_decl(std::ifstream & ifs, TID & tid, SyntTree * pst, Lex & lex);
    friend void parse_var(std::ifstream & ifs, TID & tid, SyntTree * pst, Lex & lex);
    friend void parse_var_init(std::ifstream & ifs, TID & tid, SyntTree * pst, Lex & lex);
    friend void parse_expr(std::ifstream & ifs, TID & tid, SyntTree * pst, Lex & lex);

public:
    SyntTree(std::ifstream & ifs, TID & tid);
    ~SyntTree();
    friend std::ostream & operator<<(std::ostream & os, SyntTree const & st);
    friend SyntTree parse_program(std::ifstream & ifs, TID & tid);
};

#endif
