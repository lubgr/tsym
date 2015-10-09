
%{
    #include "parser.h"
    #include <stdlib.h>
    void yyerror(char *);
%}

%%

[0-9]+ {
    yylval = atoi(yytext);
    return INTEGER;
}


%%

int yywrap(void)
{
    return 1;
}
