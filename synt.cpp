#include "synt.h"

#include "reserved.h"

void SyntTree::del_all_suc()
{
    if (suc_cnt != 0) {
        delete[] successors;
    }
}

/* Не меняет predecessor, не удаляет successors */
SyntTree & SyntTree::assign(SyntTree const & st)
{
    type = st.type;
    lex = st.lex;
    suc_cnt = st.suc_cnt;
    if (suc_cnt == 0) {
        successors = nullptr;
    } else {
        successors = new SyntTree[suc_cnt];
        for (unsigned i = 0; i < suc_cnt; ++i) {
            successors[i].assign(st.successors[i]);
            successors[i].predecessor = this;
        }
    }
    return *this;
}

SyntTree * SyntTree::add_suc(node_types type, std::string lex)
{
    if (suc_cnt == 0) {
        successors = new SyntTree[1];
    } else {
        SyntTree * tmp = new SyntTree[suc_cnt + 1];
        for (unsigned i = 0; i < suc_cnt; ++i) {
            tmp[i].assign(successors[i]);
            tmp[i].predecessor = this;
        }
        del_all_suc();
        successors = tmp;
    }
    successors[suc_cnt].type = type;
    successors[suc_cnt].lex = lex;
    successors[suc_cnt].predecessor = this;
    ++suc_cnt;

    return &(successors[suc_cnt - 1]);
}

SyntTree::SyntTree(node_types type, std::string lex, SyntTree * predecessor): type(type), lex(lex), predecessor(predecessor), successors(nullptr), suc_cnt(0)
{}

SyntTree::SyntTree(SyntTree const & st)
{
    assign(st);
}

SyntTree::~SyntTree()
{
    del_all_suc();
}

/* Не меняет predecessor */
SyntTree & SyntTree::operator=(SyntTree const & st)
{
    if (this == &st) {
        return *this;
    }

    del_all_suc();
    return assign(st);
}

const char TAB[] = "   ";

std::ostream & _print_tab(std::ostream & os, unsigned tab)
{
    for (unsigned i = 0; i < tab; ++i) {
        os << TAB;
    }
    return os;
}

std::string SyntTree::get_typename() const
{
    if (type == NODE_BEGIN) {
        return "BEGIN";
    } else if (type == NODE_SCOPE) {
        return "SCOPE";
    } else if (type == NODE_OPER_LOOP) {
        return "OPER_LOOP";
    } else if (type == NODE_OPER_IN) {
        return "READ";
    } else if (type == NODE_OPER_OUT) {
        return "WRITE";
    } else if (type == NODE_OPERAND) {
        return "OPERAND";
    } else if (type == NODE_ASSIGN) {
        return "ASSIGN";
    } else if (type == NODE_DECL) {
        return "DECL";
    } else if (type == NODE_OPER_2) {
        return "OPER_2";
    } else if (type == NODE_TYPE) {
        return "TYPE";
    } else if (type == NODE_VAR) {
        return "VAR";
    } else if (type == NODE_VAR_INIT) {
        return "VAR_INIT";
    } else if (type == NODE_EXPR) {
        return "EXPR";
    } else if (type == NODE_IF) {
        return "IF";
    } else if (type == NODE_COND) {
        return "COND";
    } else if (type == NODE_BODY) {
        return "BODY";
    } else if (type == NODE_FOR) {
        return "FOR";
    } else if (type == NODE_FOR_INIT) {
        return "FOR_INIT";
    } else if (type == NODE_FOR_COND) {
        return "FOR_COND";
    } else if (type == NODE_FOR_ITER) {
        return "FOR_ITER";
    } else if (type == NODE_WHILE) {
        return "WHILE";
    } else if (type == NODE_UNTIL) {
        return "UNTIL";
    } else {
        return "???";
    }
}

std::ostream & SyntTree::print(std::ostream & os, unsigned tab) const
{
    _print_tab(os, tab);
    os << "[" << get_typename() << "] " << lex << std::endl;
    if (suc_cnt != 0) {
        for (unsigned i = 0; i < suc_cnt; ++i) {
            successors[i].print(os, tab + 1);
        }
    }
    return os;
}

std::ostream & operator<<(std::ostream & os, SyntTree const & st)
{
    return st.print(os);
}

#define ERRMSG(EXP, GOT) "Invalid lexeme: '" + EXP + "' is expected but got '" + GOT + "'."

void err(std::string const & exp, std::string const & got)
{
    throw std::runtime_error(ERRMSG(exp, got));
}

SyntTree build_synt_tree(std::ifstream & ifs, TID & tid)
{
    ret_vals ret = RET_OK;
    std::string lex = get_lex(ifs, ret);
    if (ret != RET_OK || define_lex_type(lex) != LEX_BEGIN) {
        err(rw::BEGIN, lex);
    }
    SyntTree st(NODE_BEGIN);
    SyntTree * pst = &st;

    unsigned scope_depth = 0;
    unsigned loop_depth = 0;
    while (lex = get_lex(ifs, ret), ret == RET_OK) {
        lex_types lex_type = define_lex_type(lex);
        if (pst->type == NODE_BEGIN) {
            if (lex_type == LEX_SCOPE_L) {
                ++scope_depth;
                pst = pst->add_suc(NODE_SCOPE);
            } else {
                // error
            }
        } else if (pst->type == NODE_SCOPE) {
            if (lex_type == LEX_SCOPE_L) {
                ++scope_depth;
                pst = pst->add_suc(NODE_SCOPE);
            } else if (lex_type == LEX_SCOPE_R) {
                --scope_depth;
                pst = pst->predecessor;
            } else if (lex_type == LEX_TYPE) {
                pst = pst->add_suc(NODE_DECL, lex);
            } else if (lex_type == LEX_IF) {
                /**/pst = pst->add_suc(NODE_IF);
            } else if (lex_type == LEX_FOR) {
                /**/pst = pst->add_suc(NODE_FOR);
            } else if (lex_type == LEX_WHILE) {
                /**/pst = pst->add_suc(NODE_WHILE);
            } else if (lex_type == LEX_DO) {
                /**/pst = pst->add_suc(NODE_UNTIL);
            } else if (lex_type == LEX_OPER_LOOP) {
                /**/pst = pst->add_suc(NODE_OPER_LOOP, lex);
            } else if (lex_type == LEX_OPER_IN) {
                /**/pst = pst->add_suc(NODE_OPER_IN);
            } else if (lex_type == LEX_OPER_OUT) {
                /**/pst = pst->add_suc(NODE_OPER_OUT);
            } else if (lex_type == LEX_CONST || lex_type == LEX_VAR) {
                /**/pst = pst->add_suc(NODE_EXPR);
                pst = pst->add_suc(NODE_OPERAND, lex);
                pst = pst->predecessor;
            } else if (lex_type == LEX_PARENTHESIS_L) {
                /**/pst = pst->add_suc(NODE_EXPR);
                pst = pst->add_suc(NODE_EXPR);
            } else {
                // error
            }
        } else if (pst->type == NODE_DECL) {
            if (lex_type == LEX_VAR) {
                pst = pst->add_suc(NODE_VAR, lex);
            } else {
                // error
            }
        } else if (pst->type == NODE_VAR) {
            if (lex_type == LEX_OPER_2_RET) {
                pst = pst->add_suc(NODE_VAR_INIT);
            } else if (lex_type == LEX_OPER_COMMA) {
                tid.add(pst->predecessor->lex, pst->lex);
                pst = pst->predecessor;
            } else if (lex_type == LEX_OPER_END) {
                tid.add(pst->predecessor->lex, pst->lex);
                pst = pst->predecessor;
                pst = pst->predecessor;
            } else {
                // error
            }
        } else if (pst->type == NODE_VAR_INIT) {
            if (lex_type == LEX_CONST || lex_type == LEX_VAR) {
                /*?*/pst = pst->add_suc(NODE_OPERAND, lex);
                pst = pst->predecessor;
            } else if (lex_type == LEX_OPER_2_RET || lex_type == LEX_OPER_2_NORET) {
                /*?*/pst = pst->add_suc(NODE_OPER_2, lex);
                pst = pst->predecessor;
            } else if (lex_type == LEX_PARENTHESIS_L) {
                /*?*/pst = pst->add_suc(NODE_EXPR);
            } else if (lex_type == LEX_OPER_COMMA) {
                /*?*/tid.add(pst->predecessor->predecessor->lex, pst->predecessor->lex);
                pst = pst->predecessor;
                pst = pst->predecessor;
            } else if (lex_type == LEX_OPER_END) {
                /*?*/tid.add(pst->predecessor->predecessor->lex, pst->predecessor->lex);
                pst = pst->predecessor;
                pst = pst->predecessor;
                pst = pst->predecessor;
            } else {
                // error
            }
        } else if (pst->type == NODE_EXPR) {
            if (lex_type == LEX_CONST || lex_type == LEX_VAR) {
                /*?*/pst = pst->add_suc(NODE_OPERAND, lex);
                pst = pst->predecessor;
            } else if (lex_type == LEX_OPER_2_RET || lex_type == LEX_OPER_2_NORET) {
                /*?*/pst = pst->add_suc(NODE_OPER_2, lex);
                pst = pst->predecessor;
            } else if (lex_type == LEX_PARENTHESIS_L) {
                /*?*/pst = pst->add_suc(NODE_EXPR);
            } else if (lex_type == LEX_PARENTHESIS_R) {
                pst = pst->predecessor;
            } else if (lex_type == LEX_OPER_COMMA) {
                while (pst->type == NODE_EXPR) {
                    pst = pst->predecessor;
                }
                if (pst->type == NODE_VAR_INIT) {
                    /*?*/tid.add(pst->predecessor->predecessor->lex, pst->predecessor->lex);
                    pst = pst->predecessor;
                    pst = pst->predecessor;
                }
            } else if (lex_type == LEX_OPER_END) {
                while (pst->type == NODE_EXPR) {
                    pst = pst->predecessor;
                }
                if (pst->type == NODE_VAR_INIT) {
                    /*?*/tid.add(pst->predecessor->predecessor->lex, pst->predecessor->lex);
                    pst = pst->predecessor;
                    pst = pst->predecessor;
                    pst = pst->predecessor;
                }
            }
        } else if (pst->type == NODE_IF) {
            if (lex_type == LEX_PARENTHESIS_L) {
                pst = pst->add_suc(NODE_COND);
            } else {
                // error
            }
        } else if (pst->type == NODE_FOR) {
            if (lex_type == LEX_PARENTHESIS_L) {
                pst = pst->add_suc(NODE_FOR_INIT);
            } else {
                // error
            }
        } else if (pst->type == NODE_WHILE) {
            if (lex_type == LEX_PARENTHESIS_L) {
                pst = pst->add_suc(NODE_COND);
            } else {
                // error
            }
        } else if (pst->type == NODE_UNTIL) {
            /**/
        } else if (pst->type == NODE_OPER_LOOP) {
            if (lex_type == LEX_OPER_END) {
                pst = pst->predecessor;
            } else {
                // error
            }
        } else if (pst->type == NODE_OPER_IN) {
            /**/
        } else if (pst->type == NODE_OPER_OUT) {
            /**/
        } else if (pst->type == NODE_COND) {
            if (lex_type == LEX_CONST || lex_type == LEX_VAR) {
                /*?*/pst = pst->add_suc(NODE_OPERAND, lex);
                pst = pst->predecessor;
            } else if (lex_type == LEX_OPER_2_RET || lex_type == LEX_OPER_2_NORET) {
                /*?*/pst = pst->add_suc(NODE_OPER_2, lex);
                pst = pst->predecessor;
            } else if (lex_type == LEX_PARENTHESIS_L) {
                /*?*/pst = pst->add_suc(NODE_EXPR);
            } else if (lex_type == LEX_PARENTHESIS_R) {
                pst = pst->predecessor;
                pst = pst->add_suc(NODE_BODY);
            } else {
                // error
            }
        } else if (pst->type == NODE_BODY) {
            /**/
        } else if (pst->type == NODE_FOR_INIT) {
            /**/
        } else if (pst->type == NODE_FOR_COND) {
            /**/
        } else if (pst->type == NODE_FOR_ITER) {
            /**/
        }
    }
    if (ret == RET_ERR) {
        throw std::runtime_error("Invalid lexeme");
    }

    return st;
}

SyntTree::SyntTree(std::ifstream & ifs, TID & tid)
{
    *this = build_synt_tree(ifs, tid);
}
