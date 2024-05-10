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
    if (type == NODE_ROOT) {
        return "ROOT";
    } else if (type == NODE_SCOPE) {
        return "SCOP";
    } else if (type == NODE_DECL) {
        return "DECL";
    } else if (type == NODE_OPER) {
        return "OPER";
    } else if (type == NODE_TYPE) {
        return "TYPE";
    } else if (type == NODE_VAR) {
        return "VAR";
    } else if (type == NODE_VAL) {
        return "VAL";
    } else if (type == NODE_EXPR) {
        return "EXPR";
    } else if (type == NODE_IF) {
        return "IF";
    } else if (type == NODE_COND) {
        return "COND";
    } else if (type == NODE_BODY_IF) {
        return "BODY_IF";
    } else if (type == NODE_BODY_ELSE) {
        return "BODY_ELSE";
    } else if (type == NODE_FOR) {
        return "FOR";
    } else if (type == NODE_FOR_INIT) {
        return "FOR_INIT";
    } else if (type == NODE_FOR_ITER) {
        return "FOR_ITER";
    } else if (type == NODE_BODY) {
        return "BODY";
    } else if (type == NODE_WHILE) {
        return "WHIL";
    } else if (type == NODE_DO) {
        return "DO";
    } else if (type == NODE_UNTIL) {
        return "UNTIL";
    } else if (type == NODE_CMD) {
        return "CMD";
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

SyntTree build_synt_tree(std::ifstream & ifs, TID & tid)
{
    ret_vals ret = RET_OK;
    std::string lex = get_lex(ifs, ret);
    if (ret != RET_OK || define_lex_type(lex) != LEX_ROOT) {
        throw std::runtime_error("Invalid lexeme: '" + rw::BEGIN + "' is expected");
    }
    SyntTree st(NODE_ROOT);
    SyntTree * pst = &st;

    while (lex = get_lex(ifs, ret), ret == RET_OK) {
        lex_types lex_type = define_lex_type(lex);
        if (lex_type == LEX_SCOPE_L) {
            if (pst->type == NODE_ROOT || pst->type == NODE_SCOPE || pst->type == NODE_IF) {
                pst = pst->add_suc(NODE_SCOPE);
            } else {
                // error
            }
        } else if (lex_type == LEX_SCOPE_R) {
            while (pst->type != NODE_SCOPE) {
                if (pst->type == NODE_ROOT) {
                    // error
                }
                pst = pst->predecessor;
            }
            pst = pst->predecessor;
        } else if (lex_type == LEX_TYPE) {
            if (pst->type == NODE_SCOPE) {
                pst = pst->add_suc(NODE_DECL, lex);
            } else if (pst->type == NODE_FOR) {
                // for (int x;;)
            } else {
                // error
            }
        } else if (lex_type == LEX_VAR) {
            if (pst->type == NODE_DECL || pst->type == NODE_EXPR || pst->type == NODE_COND) {
                pst = pst->add_suc(NODE_VAR, lex);
                tid.add(pst->predecessor->lex, pst->lex);
                pst = pst->predecessor;
            } else if (pst->type == NODE_SCOPE) {
                pst = pst->add_suc(NODE_EXPR);
                pst = pst->add_suc(NODE_VAR, lex);
                pst = pst->predecessor;
            } else {
                // ...
            }
        } else if (lex_type == LEX_OPER_RET) {
            if (pst->type == NODE_DECL || pst->type == NODE_EXPR || pst->type == NODE_COND) {
                pst = pst->add_suc(NODE_EXPR);
            } else {
                // ...
            }
        } else if (lex_type == LEX_OPER_NORET) {
            if (pst->type == NODE_EXPR || pst->type == NODE_COND) {
                pst = pst->add_suc(NODE_OPER, lex);
                pst = pst->predecessor;
            } else {
                // error
            }
        } else if (lex_type == LEX_COMMA) {
            while (pst->type == NODE_EXPR) {
                pst = pst->predecessor;
            }
            if (pst->type == NODE_DECL) {
                std::string var_type = pst->lex;
                pst = pst->predecessor;
                pst = pst->add_suc(NODE_DECL, var_type);
            } else {
                // ...
            }
        } else if (lex_type == LEX_END) {
            if (pst->type == NODE_FOR) {
                // for (;;)
            } else {
                while (pst->type != NODE_SCOPE) {
                    if (pst->type == NODE_ROOT) {
                        // error
                    }
                    pst = pst->predecessor;
                }
            }
        } else if (lex_type == LEX_CONST) {
            if (pst->type == NODE_EXPR || pst->type == NODE_COND) {
                pst = pst->add_suc(NODE_VAL, lex);
                pst = pst->predecessor;
            } else {
                // ...
            }
        } else if (lex_type == LEX_IF) {
            if (pst->type == NODE_SCOPE) {
                pst = pst->add_suc(NODE_IF);
            } else {
                // error
            }
        } else if (lex_type == LEX_PARENTHESIS_L) {
            if (pst->type == NODE_IF) {
                pst = pst->add_suc(NODE_COND);
            } else {
                // ...
            }
        } else if (lex_type == LEX_PARENTHESIS_R) {
            if (pst->type == NODE_COND || pst->type == NODE_EXPR) {
                pst = pst->predecessor;
            } else {
                // ...
            }
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
