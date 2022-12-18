#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

extern int tokenval;
extern int lineno;
struct entry
{
  char *lexptr;
  int token;
};
extern struct entry symtable[];
int insert (char s[], int tok);
void error (char *m) ;
int lookup (char s[]) ;
