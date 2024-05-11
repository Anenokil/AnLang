#ifndef SYNT_H
#define SYNT_H

#include <iostream>
#include <fstream>
#include <string>
#include "tid.h"
#include "lex.h"

enum node_types {
    NODE_TMP,
    NODE_BEGIN,
    NODE_SCOPE,
    NODE_BREAK,
    NODE_CONT,
    NODE_READ,
    NODE_WRITE,
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
    NODE_FOR_INIT,
    NODE_FOR_ITER,
};

class SyntTree
{
private:
    node_types type{};
    std::string lex{};
    SyntTree * predecessor{};
    SyntTree * successors{};
    unsigned suc_cnt{};

    SyntTree * add_suc(node_types type = NODE_TMP, std::string lex = "");
    void del_all_suc();
    SyntTree & assign(SyntTree const & st);
    std::string get_typename() const;
    std::ostream & print(std::ostream & os, unsigned tab = 0) const;

    SyntTree(node_types type = NODE_BEGIN, std::string lex = "", SyntTree * predecessor = nullptr);
    SyntTree(SyntTree const & st);
    SyntTree & operator=(SyntTree const & st);

public:
    SyntTree(std::ifstream & ifs, TID & tid);
    ~SyntTree();
    friend std::ostream & operator<<(std::ostream & os, SyntTree const & st);
    friend SyntTree build_synt_tree(std::ifstream & ifs, TID & tid);
};

#endif
