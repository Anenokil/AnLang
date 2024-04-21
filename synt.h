#ifndef SYNT_H
#define SYNT_H

#include <iostream>
#include <fstream>
#include <string>
#include "lex.h"

enum node_types {
    NODE_NULL, /* temporary type */
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

    SyntTree * add_suc(node_types type = NODE_NULL, std::string lex = "");
    void del_all_suc();
    SyntTree & assign(SyntTree const & st);
    std::ostream & print(std::ostream & os, unsigned tab = 0) const;

public:
    SyntTree(node_types type = NODE_ROOT, std::string lex = "", SyntTree * predecessor = nullptr);
    SyntTree(SyntTree const & st);
    SyntTree(std::ifstream & ifs);
    ~SyntTree();
    SyntTree & operator=(SyntTree const & st);
    friend std::ostream & operator<<(std::ostream & os, SyntTree const & st);
    friend SyntTree build_synt_tree(std::ifstream & ifs);
};

#endif
