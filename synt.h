#ifndef SYNT_H
#define SYNT_H

#include <iostream>
#include <fstream>
#include <string>
#include "lex.h"

class SyntTree
{
private:
    lex_types type{};
    std::string lex{};
    SyntTree * successors{};
    unsigned suc_cnt{};

    void del_suc();
    SyntTree & assign(SyntTree const & st);
    std::ostream & print(std::ostream & os, unsigned tab = 0) const;
    void build(std::ifstream & ifs);

public:
    SyntTree(std::string const & str = "");
    SyntTree(SyntTree const & st);
    SyntTree(std::ifstream & ifs);
    ~SyntTree();
    SyntTree & operator=(SyntTree const & st);
    SyntTree & operator=(std::string const & str);
    SyntTree & operator+=(std::string const & str);
    friend std::ostream & operator<<(std::ostream & os, SyntTree const & st);
};

#endif
