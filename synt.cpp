#include "synt.h"
#include "lex.h"

void SyntTree::del_suc()
{
    if (suc_cnt != 0) {
        delete[] successors;
    }
}

SyntTree & SyntTree::assign(SyntTree const & st)
{
    data = st.data;
    suc_cnt = st.suc_cnt;
    if (suc_cnt == 0) {
        successors = nullptr;
    } else {
        successors = new SyntTree[suc_cnt];
        for (unsigned i = 0; i < suc_cnt; ++i) {
            successors[i] = st.successors[i];
        }
    }
    return *this;
}

const char TAB[] = "   ";

std::ostream & _print_tab(std::ostream & os, unsigned tab)
{
    for (unsigned i = 0; i < tab; ++i) {
        os << TAB;
    }
    return os;
}

std::ostream & SyntTree::print(std::ostream & os, unsigned tab) const
{
    _print_tab(os, tab);
    os << data << std::endl;
    if (suc_cnt != 0) {
        _print_tab(os, tab);
        os << "{" << std::endl;
        for (unsigned i = 0; i < suc_cnt; ++i) {
            successors[i].print(os, tab + 1);
        }
        _print_tab(os, tab);
        os << "}" << std::endl;
    }
    return os;
}

void SyntTree::build(std::ifstream & ifs)
{
    if (ifs.is_open()) {
        /* recieve lexemes */
        ret_vals ret = RET_OK;
        while (ret == RET_OK) {
            std::string lex = get_lex(ifs, ret);
            if (lex == "{") {
                successors[suc_cnt - 1].build(ifs);
            } else if (lex == "}") {
                break;
            } else {
                *this += lex;
            }
        }
        /* check that reading ended normally */
        if (ret == RET_ERR) {
            throw std::runtime_error("Invalid lexeme");
        }
    }
}

SyntTree::SyntTree(std::string const & str): data(str), successors(nullptr), suc_cnt(0)
{}

SyntTree::SyntTree(SyntTree const & st)
{
    assign(st);
}

SyntTree::SyntTree(std::ifstream & ifs): data(""), successors(nullptr), suc_cnt(0)
{
    build(ifs);
}

SyntTree::~SyntTree()
{
    del_suc();
}

SyntTree & SyntTree::operator=(SyntTree const & st)
{
    if (this == &st) {
        return *this;
    }

    del_suc();
    return assign(st);
}

SyntTree & SyntTree::operator=(std::string const & str)
{
    data = str;
    return *this;
}

SyntTree & SyntTree::operator+=(std::string const & str)
{
    SyntTree * tmp = new SyntTree[suc_cnt + 1];
    for (unsigned i = 0; i < suc_cnt; ++i) {
        tmp[i] = std::move(successors[i]);
    }
    tmp[suc_cnt] = str;

    del_suc();
    successors = tmp;
    ++suc_cnt;
    return *this;
}

std::ostream & operator<<(std::ostream & os, SyntTree const & st)
{
    return st.print(os);
}
