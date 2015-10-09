
%{
    #include <stdio.h>

    void yyerror(char *);
    int yylex(void);
%}

%token INTEGER

%%

program:
        program '\n'
        |
        ;

%%

void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}
