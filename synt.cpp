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
    } else if (type == NODE_DECL) {
        return "DECL";
    } else if (type == NODE_DECL_ID) {
        return "DECL_ID";
    } else if (type == NODE_DECL_INIT) {
        return "DECL_INIT";
    } else if (type == NODE_ASSIGN) {
        return "ASSIGN";
    } else if (type == NODE_IF) {
        return "IF";
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
    } else if (type == NODE_OPER_LOOP) {
        return "OPER_LOOP";
    } else if (type == NODE_OPER_IN) {
        return "OPER_IN";
    } else if (type == NODE_OPER_OUT) {
        return "OPER_OUT";
    } else if (type == NODE_EXPR) {
        return "EXPR";
    } else if (type == NODE_OPER_2) {
        return "OPER_2";
    } else if (type == NODE_OPERAND) {
        return "OPERAND";
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

void Parser::err(std::string const & cause)
{
    throw std::runtime_error(std::to_string(cur_lex.row()) + " line, " + std::to_string(cur_lex.col()) +
            " column: " + cause + ": '" + cur_lex.word() + "'.");
}

bool Parser::get_lex()
{
    return sc.get_lex(cur_lex, true);
}

bool Parser::get_lex(LexType req_type)
{
    bool ret = sc.get_lex(cur_lex, true);
    if (cur_lex.type() != req_type) {
        err(req_type);
    }
    return ret;
}

void Parser::parse_program()
{
    get_lex(LEX_BEGIN);
    SyntTree * pst = &st;

    get_lex(LEX_SCOPE_L);
    parse_scope(pst);

    if (cur_lex.type() != LEX_EOF) {
        err();
    }
}

void Parser::parse_scope(SyntTree * pst)
{
    pst = pst->add_suc(NODE_SCOPE);

    get_lex();
    while (cur_lex.type() != LEX_SCOPE_R) {
        parse_statement(pst);
    }
    get_lex();
}

void Parser::parse_statement(SyntTree * pst)
{
    if (cur_lex.type() == LEX_SCOPE_L) {
        parse_scope(pst);
    } else if (cur_lex.type() == LEX_TYPE) {
        parse_decl(pst);
        if (cur_lex.type() != LEX_OPER_END) {
            err();
        }
        get_lex();
    } else if (cur_lex.type() == LEX_ID) {
        parse_assign(pst);
        if (cur_lex.type() != LEX_OPER_END) {
            err();
        }
        get_lex();
    } else if (cur_lex.type() == LEX_IF) {
        parse_if(pst);
    } else if (cur_lex.type() == LEX_FOR) {
        parse_for(pst);
    } else if (cur_lex.type() == LEX_WHILE) {
        parse_while(pst);
    } else if (cur_lex.type() == LEX_DO) {
        parse_until(pst);
    } else if (cur_lex.type() == LEX_OPER_LOOP) {
        parse_oper_loop(pst);
    } else if (cur_lex.type() == LEX_OPER_IN) {
        parse_oper_in(pst);
    } else if (cur_lex.type() == LEX_OPER_OUT) {
        parse_oper_out(pst);
    } else {
        err();
    }
}

void Parser::parse_decl(SyntTree * pst)
{
    pst = pst->add_suc(NODE_DECL, cur_lex.word());

    do {
        get_lex(LEX_ID);
        parse_decl_id(pst);
    } while (cur_lex.type() == LEX_OPER_COMMA);
}

void Parser::parse_decl_id(SyntTree * pst)
{
    pst = pst->add_suc(NODE_DECL_ID, cur_lex.word());

    tid.add(pst->predecessor->lex, pst->lex);

    get_lex();
    if (cur_lex.type() == LEX_OPER_2_RET) {
        parse_decl_init(pst);
    } else if(cur_lex.type() == LEX_OPER_COMMA) {
        // return;
    } else if (cur_lex.type() == LEX_OPER_END) {
        // return;
    } else {
        err();
    }
}

void Parser::parse_decl_init(SyntTree * pst)
{
    get_lex();
    parse_expr(pst);

    if (cur_lex.type() == LEX_OPER_COMMA) {
        // return;
    } else if (cur_lex.type() == LEX_OPER_END) {
        // return;
    } else {
        err();
    }
}

void Parser::parse_assign(SyntTree * pst)
{
    pst = pst->add_suc(NODE_ASSIGN, cur_lex.word());

    do {
        get_lex(LEX_OPER_2_RET);
        get_lex();
        parse_expr(pst);
    } while (cur_lex.type() == LEX_OPER_COMMA);
}

void Parser::parse_if(SyntTree * pst)
{
    pst = pst->add_suc(NODE_IF);

    get_lex(LEX_PARENTHESIS_L);
    get_lex();
    parse_expr(pst);
    if (cur_lex.type() != LEX_PARENTHESIS_R) {
        err();
    }
    get_lex();
    parse_statement(pst);
    if (cur_lex.type() == LEX_ELSE) {
        get_lex();
        parse_statement(pst);
    } else {
        // return;
    }
}

void Parser::parse_for(SyntTree * pst)
{
    pst = pst->add_suc(NODE_FOR);

    get_lex(LEX_PARENTHESIS_L);

    get_lex();
    parse_for_init(pst);
    if (cur_lex.type() != LEX_OPER_END) {
        err();
    }

    get_lex();
    parse_expr(pst);
    if (cur_lex.type() != LEX_OPER_END) {
        err();
    }

    get_lex();
    parse_assign(pst);
    if (cur_lex.type() != LEX_PARENTHESIS_R) {
        err();
    }

    get_lex();
    parse_statement(pst);
}

void Parser::parse_for_init(SyntTree * pst)
{
    if (cur_lex.type() == LEX_TYPE) {
        parse_decl(pst);
    } else if (cur_lex.type() == LEX_ID) {
        parse_assign(pst);
    }
}

void Parser::parse_while(SyntTree * pst)
{
    pst = pst->add_suc(NODE_WHILE);

    get_lex(LEX_PARENTHESIS_L);
    get_lex();
    parse_expr(pst);
    if (cur_lex.type() != LEX_PARENTHESIS_R) {
        err();
    }
    get_lex();
    parse_statement(pst);
}

void Parser::parse_until(SyntTree * pst)
{
    pst = pst->add_suc(NODE_UNTIL);

    get_lex();
    parse_statement(pst);
    if (cur_lex.type() != LEX_UNTIL) {
        err();
    }
    get_lex(LEX_PARENTHESIS_L);
    get_lex();
    parse_expr(pst);
    if (cur_lex.type() != LEX_PARENTHESIS_R) {
        err();
    }
    get_lex(LEX_OPER_END);
    get_lex();
}

void Parser::parse_oper_loop(SyntTree * pst)
{
    pst = pst->add_suc(NODE_OPER_LOOP, cur_lex.word());

    get_lex(LEX_OPER_END);
    get_lex();
}

void Parser::parse_oper_in(SyntTree * pst)
{
    get_lex(LEX_ID);
    pst = pst->add_suc(NODE_OPER_IN, cur_lex.word());

    get_lex(LEX_OPER_END);
    get_lex();
}

void Parser::parse_oper_out(SyntTree * pst)
{
    pst = pst->add_suc(NODE_OPER_OUT);
    
    do {
        get_lex();
        parse_expr(pst);
    } while (cur_lex.type() == LEX_OPER_COMMA);
    if (cur_lex.type() != LEX_OPER_END) {
        err();
    }
    get_lex();
}

void Parser::parse_expr(SyntTree * pst)
{
    pst = pst->add_suc(NODE_EXPR);

    parse_expr2(pst, true);
}

void Parser::parse_expr2(SyntTree * pst, bool is_lval)
{
    if (cur_lex.type() == LEX_ID) {
        pst->add_suc(NODE_OPERAND, cur_lex.word());
    } else if (cur_lex.type() == LEX_INT_CONST || cur_lex.type() == LEX_FLOAT_CONST || cur_lex.type() == LEX_BOOL_CONST || cur_lex.type() == LEX_STR_CONST) {
        is_lval = false;
        pst->add_suc(NODE_OPERAND, cur_lex.word());
    } else if (cur_lex.type() == LEX_PARENTHESIS_L) {
        is_lval = false;
        get_lex();
        parse_expr(pst);
        if (cur_lex.type() != LEX_PARENTHESIS_R) {
            err();
        }
    } else {
        err();
    }

    get_lex();
    if (cur_lex.type() == LEX_OPER_2_RET) {
        if (!is_lval) {
            err("Invalid left operand");
        }
        pst->add_suc(NODE_OPER_2, cur_lex.word());
        get_lex();
        parse_expr2(pst, true);
    } else if (cur_lex.type() == LEX_OPER_2_NORET) {
        pst->add_suc(NODE_OPER_2, cur_lex.word());
        get_lex();
        parse_expr2(pst, false);
    } else {
        // return;
    }
}

Parser::Parser(std::ifstream & ifs): ifs(ifs), tid(), st(NODE_BEGIN), cur_lex(), sc(ifs)
{
    parse_program();
}

std::ostream & operator<<(std::ostream & os, Parser const & par)
{
    os << par.st;
    os << par.tid;
    return os;
}
