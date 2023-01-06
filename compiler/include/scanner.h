#ifndef SCANNER_H
#define SCANNER_H

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL yy::parser::symbol_type Scanner::get_next_token()

#include "parser.h"

class Scanner : public yyFlexLexer
{
private:
    Driver &drv;

public:
    explicit Scanner(Driver &drv) : drv(drv){};
    virtual ~Scanner() {}
    virtual yy::parser::symbol_type get_next_token();
};

#endif