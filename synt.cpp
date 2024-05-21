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

SyntTree parse_program(std::ifstream & ifs, TID & tid)
{
    RetVal ret = RET_OK;
    Lex lex = _get_lex(ifs, ret, LEX_BEGIN);
    SyntTree st(NODE_BEGIN);
    SyntTree * pst = &st;

    lex = _get_lex(ifs, ret, LEX_SCOPE_L);
    parse_scope(ifs, tid, pst, lex);

    /*lex = _get_lex(ifs, ret, false);
    if (ret != RET_EOF) {
        _err(lex);
    }*/

    return st;
}

void parse_scope(std::ifstream & ifs, TID & tid, SyntTree * pst, Lex & lex)
{
    pst = pst->add_suc(NODE_SCOPE);
    RetVal ret = RET_OK;

    lex = _get_lex(ifs, ret);
    while (lex.type() != LEX_SCOPE_R) {
        parse_statement(ifs, tid, pst, lex);
    }
    lex = _get_lex(ifs, ret, false);
}

void parse_statement(std::ifstream & ifs, TID & tid, SyntTree * pst, Lex & lex)
{
    //pst = pst->add_suc(NODE_STATEMENT);
    RetVal ret = RET_OK;

    if (lex.type() == LEX_SCOPE_L) {
        parse_scope(ifs, tid, pst, lex);
    } else if (lex.type() == LEX_TYPE) {
        parse_decl(ifs, tid, pst, lex);
    } else if (lex.type() == LEX_IF) {
        parse_if(ifs, tid, pst, lex);
    } else {
        //
    }
}

void parse_if(std::ifstream & ifs, TID & tid, SyntTree * pst, Lex & lex)
{
    pst = pst->add_suc(NODE_IF);
    RetVal ret = RET_OK;

    lex = _get_lex(ifs, ret, LEX_PARENTHESIS_L);
    parse_expr(ifs, tid, pst, lex);
    if (lex.type() != LEX_PARENTHESIS_R) {
        _err(lex);
    }
    lex = _get_lex(ifs, ret);
    parse_statement(ifs, tid, pst, lex);
    if (lex.type() == LEX_ELSE) {
        lex = _get_lex(ifs, ret);
        parse_statement(ifs, tid, pst, lex);
    } else {
        // return;
    }
}

void parse_decl(std::ifstream & ifs, TID & tid, SyntTree * pst, Lex & lex)
{
    pst = pst->add_suc(NODE_DECL, lex.word());
    RetVal ret = RET_OK;

    do {
        lex = _get_lex(ifs, ret, LEX_VAR);
        parse_var(ifs, tid, pst, lex);
    } while (lex.type() == LEX_OPER_COMMA);
    if (lex.type() != LEX_OPER_END) {
        _err(lex);
    }
    lex = _get_lex(ifs, ret);
}

void parse_var(std::ifstream & ifs, TID & tid, SyntTree * pst, Lex & lex)
{
    pst = pst->add_suc(NODE_VAR, lex.word());
    RetVal ret = RET_OK;

    tid.add(pst->predecessor->lex, pst->lex);

    lex = _get_lex(ifs, ret);
    if (lex.type() == LEX_OPER_2_RET) {
        parse_var_init(ifs, tid, pst, lex);
    } else if(lex.type() == LEX_OPER_COMMA) {
        // return;
    } else if (lex.type() == LEX_OPER_END) {
        // return;
    } else {
        _err(lex);
    }
}

void parse_var_init(std::ifstream & ifs, TID & tid, SyntTree * pst, Lex & lex)
{
    RetVal ret = RET_OK;

    lex = _get_lex(ifs, ret);
    pst = pst->add_suc(NODE_VAR_INIT, lex.word());
    if (lex.type() == LEX_VAR) {
        //
    } else if (lex.type() == LEX_CONST) {
        //
    } else {
        _err(lex);
    }

    lex = _get_lex(ifs, ret);
    if (lex.type() == LEX_OPER_COMMA) {
        // return;
    } else if (lex.type() == LEX_OPER_END) {
        // return;
    } else {
        _err(lex);
    }
}

void parse_expr(std::ifstream & ifs, TID & tid, SyntTree * pst, Lex & lex)
{
    RetVal ret = RET_OK;

    lex = _get_lex(ifs, ret);
    /**/pst = pst->add_suc(NODE_EXPR, lex.word());
    if (lex.type() != LEX_VAR && lex.type() != LEX_CONST) {
        _err(lex);
    }

    lex = _get_lex(ifs, ret);
    // return;
}

SyntTree::SyntTree(std::ifstream & ifs, TID & tid)
{
    *this = parse_program(ifs, tid);
}
