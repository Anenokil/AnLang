#ifndef SYNT_H
#define SYNT_H

#include <iostream>
#include <fstream>
#include <string>
#include "tid.h"
#include "lex.h"

enum node_types {
    NODE_TMP,
    NODE_ROOT,
    NODE_SCOPE,
    NODE_DECL,
    NODE_OPER,
    NODE_TYPE,
    NODE_VAR,
    NODE_VAL,
    NODE_EXPR,
    NODE_IF,
    NODE_COND,
    NODE_BODY_IF,
    NODE_BODY_ELSE,
    NODE_FOR,
    NODE_FOR_INIT,
    NODE_FOR_ITER,
    NODE_BODY,
    NODE_WHILE,
    NODE_DO,
    NODE_UNTIL,
    NODE_CMD,
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

    SyntTree(node_types type = NODE_ROOT, std::string lex = "", SyntTree * predecessor = nullptr);
    SyntTree(SyntTree const & st);
    SyntTree & operator=(SyntTree const & st);

public:
    SyntTree(std::ifstream & ifs, TID & tid);
    ~SyntTree();
    friend std::ostream & operator<<(std::ostream & os, SyntTree const & st);
    friend SyntTree build_synt_tree(std::ifstream & ifs, TID & tid);
};

#endif
