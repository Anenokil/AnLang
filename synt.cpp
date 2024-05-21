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

void Parser::err()
{
    throw std::runtime_error(std::to_string(cur_lex.row()) + " line, " + std::to_string(cur_lex.col()) +
            " column: Invalid lexeme: '" + cur_lex.word() + "'.");
}

void Parser::err(LexType exp)
{
    throw std::runtime_error(std::to_string(cur_lex.row()) + " line, " + std::to_string(cur_lex.col()) +
            " column: Invalid lexeme: type '" + std::to_string(exp) + "' is expected but got '" +
            cur_lex.word() + "'.");
}

bool Parser::get_lex(std::ifstream & ifs, Lex & lex, bool to_throw_on_err)
{
    return ::get_lex(ifs, lex, to_throw_on_err);
}

bool Parser::get_lex(std::ifstream & ifs, Lex & lex, LexType req_type, bool to_throw_on_err)
{
    bool ret = ::get_lex(ifs, lex, to_throw_on_err);
    if (lex.type() != req_type) {
        err(req_type);
    }
    return ret;
}

void Parser::parse_program()
{
    get_lex(ifs, cur_lex, LEX_BEGIN);
    SyntTree * pst = &st;

    get_lex(ifs, cur_lex, LEX_SCOPE_L);
    parse_scope(pst);

    if (cur_lex.type() != LEX_EOF) {
        err();
    }
}

void Parser::parse_scope(SyntTree * pst)
{
    pst = pst->add_suc(NODE_SCOPE);

    get_lex(ifs, cur_lex);
    while (cur_lex.type() != LEX_SCOPE_R) {
        parse_statement(pst);
    }
    get_lex(ifs, cur_lex);
}

void Parser::parse_statement(SyntTree * pst)
{
    //pst = pst->add_suc(NODE_STATEMENT);

    if (cur_lex.type() == LEX_SCOPE_L) {
        parse_scope(pst);
    } else if (cur_lex.type() == LEX_TYPE) {
        parse_decl(pst);
    } else if (cur_lex.type() == LEX_IF) {
        parse_if(pst);
    } else {
        //
    }
}

void Parser::parse_if(SyntTree * pst)
{
    pst = pst->add_suc(NODE_IF);

    get_lex(ifs, cur_lex, LEX_PARENTHESIS_L);
    parse_expr(pst);
    if (cur_lex.type() != LEX_PARENTHESIS_R) {
        err();
    }
    get_lex(ifs, cur_lex);
    parse_statement(pst);
    if (cur_lex.type() == LEX_ELSE) {
        get_lex(ifs, cur_lex);
        parse_statement(pst);
    } else {
        // return;
    }
}

void Parser::parse_decl(SyntTree * pst)
{
    pst = pst->add_suc(NODE_DECL, cur_lex.word());

    do {
        get_lex(ifs, cur_lex, LEX_VAR);
        parse_var(pst);
    } while (cur_lex.type() == LEX_OPER_COMMA);
    if (cur_lex.type() != LEX_OPER_END) {
        err();
    }
    get_lex(ifs, cur_lex);
}

void Parser::parse_var(SyntTree * pst)
{
    pst = pst->add_suc(NODE_VAR, cur_lex.word());

    tid.add(pst->predecessor->lex, pst->lex);

    get_lex(ifs, cur_lex);
    if (cur_lex.type() == LEX_OPER_2_RET) {
        parse_var_init(pst);
    } else if(cur_lex.type() == LEX_OPER_COMMA) {
        // return;
    } else if (cur_lex.type() == LEX_OPER_END) {
        // return;
    } else {
        err();
    }
}

void Parser::parse_var_init(SyntTree * pst)
{
    get_lex(ifs, cur_lex);
    pst = pst->add_suc(NODE_VAR_INIT, cur_lex.word());
    if (cur_lex.type() == LEX_VAR) {
        //
    } else if (cur_lex.type() == LEX_CONST) {
        //
    } else {
        err();
    }

    get_lex(ifs, cur_lex);
    if (cur_lex.type() == LEX_OPER_COMMA) {
        // return;
    } else if (cur_lex.type() == LEX_OPER_END) {
        // return;
    } else {
        err();
    }
}

void Parser::parse_expr(SyntTree * pst)
{
    get_lex(ifs, cur_lex);
    /**/pst = pst->add_suc(NODE_EXPR, cur_lex.word());
    if (cur_lex.type() != LEX_VAR && cur_lex.type() != LEX_CONST) {
        err();
    }

    get_lex(ifs, cur_lex);
    // return;
}

Parser::Parser(std::ifstream & ifs): ifs(ifs), tid(), st(NODE_BEGIN), cur_lex()
{
    parse_program();
}

std::ostream & operator<<(std::ostream & os, Parser const & par)
{
    os << par.st;
    os << par.tid;
    return os;
}
