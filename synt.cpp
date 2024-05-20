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

SyntTree * SyntTree::add_suc(NodeType type, std::string lex)
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

SyntTree::SyntTree(NodeType type, std::string lex, SyntTree * predecessor): type(type), lex(lex), predecessor(predecessor), successors(nullptr), suc_cnt(0)
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
    } else if (type == NODE_FOR) {
        return "FOR";
    } else if (type == NODE_FOR_INIT) {
        return "FOR_INIT";
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

void _err(Lex const & got, LexType exp)
{
    throw std::runtime_error(std::to_string(got.row()) + " line, " + std::to_string(got.col()) + " column: Invalid lexeme: type '" + std::to_string(exp) + "' is expected but got '" + got.word() + "'.");
}

void _err(Lex const & got)
{
    throw std::runtime_error(std::to_string(got.row()) + " line, " + std::to_string(got.col()) + " column: Invalid lexeme: '" + got.word() + "'.");
}

Lex _get_lex(std::ifstream & ifs, RetVal & ret, bool to_throw_on_eof = true, bool to_throw_on_err = true)
{
    Lex lex = get_lex(ifs, ret, to_throw_on_err);
    if (ret == RET_EOF && to_throw_on_eof) {
        _err(lex);
    }
    return lex;
}

Lex _get_lex(std::ifstream & ifs, RetVal & ret, LexType req_type, bool to_throw_on_eof = true, bool to_throw_on_err = true)
{
    Lex lex = get_lex(ifs, ret, to_throw_on_err);
    if (ret == RET_EOF && to_throw_on_eof || lex.type() != req_type) {
        _err(lex, req_type);
    }
    return lex;
}

bool _is_multi_separator(LexType sep_type)
{
    return sep_type == LEX_OPER_END;
}

void create_node(std::ifstream & ifs, TID & tid, SyntTree * pst, Lex & lex, LexType until, int & scope_depth, int & loop_depth)
{
    RetVal ret;

    if (pst->type == NODE_SCOPE) {
        if (lex.type() == LEX_SCOPE_L) {
            ++scope_depth;
            pst = pst->add_suc(NODE_SCOPE);
            lex = _get_lex(ifs, ret);
            create_node(ifs, tid, pst, lex, LEX_SCOPE_R, scope_depth, loop_depth);
            pst = pst->predecessor;
        } else if (lex.type() == LEX_SCOPE_R) {
            --scope_depth;
            return;
        } else if (lex.type() == LEX_TYPE) {
            pst = pst->add_suc(NODE_DECL, lex.word());
            lex = _get_lex(ifs, ret);
            create_node(ifs, tid, pst, lex, LEX_OPER_END, scope_depth, loop_depth);
            pst = pst->predecessor;
        } else if (lex.type() == LEX_IF) {
            pst = pst->add_suc(NODE_IF);

            _get_lex(ifs, ret, LEX_PARENTHESIS_L);
            pst = pst->add_suc(NODE_EXPR);
            lex = _get_lex(ifs, ret);
            create_node(ifs, tid, pst, lex, LEX_PARENTHESIS_R, scope_depth, loop_depth);
            pst = pst->predecessor;

            ++scope_depth;
            pst = pst->add_suc(NODE_SCOPE);
            lex = _get_lex(ifs, ret);
            if (lex.type() == LEX_SCOPE_L) {
                lex = _get_lex(ifs, ret);
                create_node(ifs, tid, pst, lex, LEX_SCOPE_R, scope_depth, loop_depth);
            } else {
                create_node(ifs, tid, pst, lex, LEX_OPER_END, scope_depth, loop_depth);
            }
            pst = pst->predecessor;

            pst = pst->predecessor;
        /*} else if (lex.type() == LEX_FOR) {
            pst = pst->add_suc(NODE_FOR);
        } else if (lex.type() == LEX_WHILE) {
            pst = pst->add_suc(NODE_WHILE);
        } else if (lex.type() == LEX_DO) {
            pst = pst->add_suc(NODE_UNTIL);
        } else if (lex.type() == LEX_OPER_LOOP) {
            pst = pst->add_suc(NODE_OPER_LOOP, lex.word());
        } else if (lex.type() == LEX_OPER_IN) {
            pst = pst->add_suc(NODE_OPER_IN);
        } else if (lex.type() == LEX_OPER_OUT) {
            pst = pst->add_suc(NODE_OPER_OUT);
        } else if (lex.type() == LEX_CONST || lex.type() == LEX_VAR) {
            pst = pst->add_suc(NODE_EXPR);
            pst = pst->add_suc(NODE_OPERAND, lex.word());
            pst = pst->predecessor;*/
        } else {
            _err(lex);
        }
    } else if (pst->type == NODE_DECL) {
        if (lex.type() == LEX_VAR) {
            pst = pst->add_suc(NODE_VAR, lex.word());
        } else {
            _err(lex, LEX_VAR);
        }
    } else if (pst->type == NODE_VAR) {
        if (lex.type() == LEX_OPER_2_RET) {
            pst = pst->add_suc(NODE_VAR_INIT);
        } else if (lex.type() == LEX_OPER_COMMA) {
            tid.add(pst->predecessor->lex, pst->lex);
            pst = pst->predecessor;
        } else if (lex.type() == LEX_OPER_END) {
            tid.add(pst->predecessor->lex, pst->lex);
            return;
        } else {
            _err(lex);
        }
    } else if (pst->type == NODE_VAR_INIT) {
        if (lex.type() == LEX_CONST || lex.type() == LEX_VAR) {
            pst = pst->add_suc(NODE_OPERAND, lex.word());
            pst = pst->predecessor;
        } else if (lex.type() == LEX_OPER_2_RET || lex.type() == LEX_OPER_2_NORET) {
            pst = pst->add_suc(NODE_OPER_2, lex.word());
            pst = pst->predecessor;
        } else if (lex.type() == LEX_PARENTHESIS_L) {
            pst = pst->add_suc(NODE_EXPR);
            lex = _get_lex(ifs, ret);
            create_node(ifs, tid, pst, lex, LEX_PARENTHESIS_R, scope_depth, loop_depth);
            pst = pst->predecessor;
        } else if (lex.type() == LEX_OPER_COMMA) {
            tid.add(pst->predecessor->predecessor->lex, pst->predecessor->lex);
            pst = pst->predecessor;
            pst = pst->predecessor;
        } else if (lex.type() == LEX_OPER_END) {
            tid.add(pst->predecessor->predecessor->lex, pst->predecessor->lex);
            return;
        } else {
            _err(lex);
        }
    } else if (pst->type == NODE_EXPR) {
        if (lex.type() == LEX_CONST || lex.type() == LEX_VAR) {
            pst = pst->add_suc(NODE_OPERAND, lex.word());
            pst = pst->predecessor;
        } else if (lex.type() == LEX_OPER_2_RET || lex.type() == LEX_OPER_2_NORET) {
            pst = pst->add_suc(NODE_OPER_2, lex.word());
            pst = pst->predecessor;
        } else if (lex.type() == LEX_PARENTHESIS_L) {
            pst = pst->add_suc(NODE_EXPR);
            lex = _get_lex(ifs, ret);
            create_node(ifs, tid, pst, lex, LEX_PARENTHESIS_R, scope_depth, loop_depth);
            pst = pst->predecessor;
        } else if (lex.type() == LEX_PARENTHESIS_R) {
            return;
        } else {
            _err(lex);
        }
    } else if (pst->type == NODE_FOR) {
        /**/
    } else if (pst->type == NODE_WHILE) {
        /**/
    } else if (pst->type == NODE_UNTIL) {
        /**/
    } else if (pst->type == NODE_OPER_LOOP) {
        /**/
    } else if (pst->type == NODE_OPER_IN) {
        /**/
    } else if (pst->type == NODE_OPER_OUT) {
        /**/
    }

    if (lex.type() == until && _is_multi_separator(until)) {
        return;
    }
    lex = _get_lex(ifs, ret);
    create_node(ifs, tid, pst, lex, until, scope_depth, loop_depth);
}

SyntTree build_synt_tree(std::ifstream & ifs, TID & tid)
{
    RetVal ret = RET_OK;
    Lex lex = _get_lex(ifs, ret, LEX_BEGIN);
    SyntTree st(NODE_BEGIN);
    SyntTree * pst = &st;

    lex = _get_lex(ifs, ret, LEX_SCOPE_L);
    pst = pst->add_suc(NODE_SCOPE);

    int scope_depth = 1;
    int loop_depth = 0;
    lex = _get_lex(ifs, ret);
    create_node(ifs, tid, pst, lex, LEX_SCOPE_R, scope_depth, loop_depth);

    lex = _get_lex(ifs, ret, false);
    if (ret != RET_EOF) {
        _err(lex);
    }

    return st;
}

SyntTree::SyntTree(std::ifstream & ifs, TID & tid)
{
    *this = build_synt_tree(ifs, tid);
}
