#ifndef SYNT_TREE_H
#define SYNT_TREE_H

#include <iostream>
#include <string>

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
    NODE_WHILE,
    NODE_UNTIL,
    NODE_BREAK,
    NODE_CONTINUE,
    NODE_OPER_IN,
    NODE_OPER_OUT,
    NODE_EXPR,
    NODE_OPERAND,
    NODE_OPER_1,
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

#endif
