#include "parser.h"

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

    get_lex(LEX_BRACE_L);
    parse_scope(pst);

    if (cur_lex.type() != LEX_EOF) {
        err();
    }
}

void Parser::parse_scope(SyntTree * pst)
{
    pst = pst->add_suc(NODE_SCOPE);

    get_lex();
    while (cur_lex.type() != LEX_BRACE_R) {
        parse_statement(pst);
    }
    get_lex();
}

void Parser::parse_statement(SyntTree * pst)
{
    if (cur_lex.type() == LEX_BRACE_L) {
        parse_scope(pst);
    } else if (is_type(cur_lex.type())) {
        parse_decl(pst);
        if (cur_lex.type() != LEX_SEMICOLON) {
            err();
        }
        get_lex();
    } else if (cur_lex.type() == LEX_ID || is_oper_1(cur_lex.type())) {
        parse_assign(pst);
        if (cur_lex.type() != LEX_SEMICOLON) {
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
    } else if (cur_lex.type() == LEX_BREAK) {
        parse_break(pst);
    } else if (cur_lex.type() == LEX_CONTINUE) {
        parse_continue(pst);
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
    } while (cur_lex.type() == LEX_COMMA);
}

void Parser::parse_decl_id(SyntTree * pst)
{
    pst = pst->add_suc(NODE_DECL_ID, cur_lex.word());

    bool ret = tid.add(pst->predecessor->lex, pst->lex);
    if (!ret) {
        err("The variable is re-declared");
    }

    get_lex();
    if (is_oper_2_ret(cur_lex.type())) {
        parse_decl_init(pst);
    } else if(cur_lex.type() == LEX_COMMA) {
        // return;
    } else if (cur_lex.type() == LEX_SEMICOLON) {
        // return;
    } else {
        err();
    }
}

void Parser::parse_decl_init(SyntTree * pst)
{
    get_lex();
    parse_expr(pst);

    if (cur_lex.type() == LEX_COMMA) {
        // return;
    } else if (cur_lex.type() == LEX_SEMICOLON) {
        // return;
    } else {
        err();
    }
}

void Parser::parse_assign(SyntTree * pst)
{
    pst = pst->add_suc(NODE_ASSIGN, cur_lex.word());

    if (tid.find(cur_lex.word()) == -1) {
        err("The variable is not declared");
    }

    do {
        get_lex();
        if (!is_oper_2_ret(cur_lex.type())) {
            err();
        }
        get_lex();
        parse_expr(pst);
    } while (cur_lex.type() == LEX_COMMA);
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
    if (cur_lex.type() != LEX_SEMICOLON) {
        parse_for_init(pst);
        if (cur_lex.type() != LEX_SEMICOLON) {
            err();
        }
    } else {
        pst->add_suc(NODE_FOR_INIT);
    }

    get_lex();
    if (cur_lex.type() != LEX_SEMICOLON) {
        parse_expr(pst);
        if (cur_lex.type() != LEX_SEMICOLON) {
            err();
        }
    } else {
        pst->add_suc(NODE_EXPR);
    }

    get_lex();
    if (cur_lex.type() != LEX_PARENTHESIS_R) {
        parse_assign(pst);
        if (cur_lex.type() != LEX_PARENTHESIS_R) {
            err();
        }
    } else {
        pst->add_suc(NODE_ASSIGN);
    }

    get_lex();
    ++loop_depth;
    parse_statement(pst);
    --loop_depth;
}

void Parser::parse_for_init(SyntTree * pst)
{
    if (is_type(cur_lex.type())) {
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
    ++loop_depth;
    parse_statement(pst);
    --loop_depth;
}

void Parser::parse_until(SyntTree * pst)
{
    pst = pst->add_suc(NODE_UNTIL);

    get_lex();
    ++loop_depth;
    parse_statement(pst);
    --loop_depth;
    if (cur_lex.type() != LEX_UNTIL) {
        err();
    }
    get_lex(LEX_PARENTHESIS_L);
    get_lex();
    parse_expr(pst);
    if (cur_lex.type() != LEX_PARENTHESIS_R) {
        err();
    }
    get_lex(LEX_SEMICOLON);
    get_lex();
}

void Parser::parse_break(SyntTree * pst)
{
    pst = pst->add_suc(NODE_BREAK);

    if (loop_depth == 0) {
        err("A break statement may only be used within a loop");
    }

    get_lex(LEX_SEMICOLON);
    get_lex();
}

void Parser::parse_continue(SyntTree * pst)
{
    pst = pst->add_suc(NODE_CONTINUE);

    if (loop_depth == 0) {
        err("A continue statement may only be used within a loop");
    }

    get_lex(LEX_SEMICOLON);
    get_lex();
}

void Parser::parse_oper_in(SyntTree * pst)
{
    get_lex(LEX_ID);
    pst = pst->add_suc(NODE_OPER_IN, cur_lex.word());

    if (tid.find(cur_lex.word()) == -1) {
        err("The variable is not declared");
    }

    get_lex(LEX_SEMICOLON);
    get_lex();
}

void Parser::parse_oper_out(SyntTree * pst)
{
    pst = pst->add_suc(NODE_OPER_OUT);
    
    do {
        get_lex();
        parse_expr(pst);
    } while (cur_lex.type() == LEX_COMMA);
    if (cur_lex.type() != LEX_SEMICOLON) {
        err();
    }
    get_lex();
}

void Parser::parse_expr(SyntTree * pst)
{
    pst = pst->add_suc(NODE_EXPR);

    if (is_oper_1(cur_lex.type())) {
        pst->add_suc(NODE_OPER_1, cur_lex.word());
        get_lex();
        parse_expr2(pst, false);
    } else {
        parse_expr2(pst, true);
    }
}

void Parser::parse_expr2(SyntTree * pst, bool is_lval)
{
    if (cur_lex.type() == LEX_ID) {
        if (tid.find(cur_lex.word()) == -1) {
            err("The variable is not declared");
        }
        pst->add_suc(NODE_OPERAND, cur_lex.word());
    } else if (is_const(cur_lex.type())) {
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
    if (is_oper_2_ret(cur_lex.type())) {
        if (!is_lval) {
            err("Invalid left operand");
        }
        pst->add_suc(NODE_OPER_2, cur_lex.word());
        get_lex();
        parse_expr2(pst, true);
    } else if (is_oper_2_noret(cur_lex.type())) {
        pst->add_suc(NODE_OPER_2, cur_lex.word());
        get_lex();
        parse_expr2(pst, false);
    } else {
        // return;
    }
}

Parser::Parser(std::ifstream & ifs): ifs(ifs), tid(), st(NODE_BEGIN), cur_lex(), sc(ifs), loop_depth(0)
{
    parse_program();
}

std::ostream & operator<<(std::ostream & os, Parser const & par)
{
    os << par.st;
    os << par.tid;
    return os;
}
