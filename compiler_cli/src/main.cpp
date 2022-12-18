#include <parser.h>

extern int yylex_destroy(void);

int main()
{
    yyparse();
    yylex_destroy();
    return 0;
}