
%{
    #include "parser.h"
    #include <stdlib.h>
    #include <errno.h>

    void *globalResult;
    unsigned column;
    void yyerror(char *message);
%}

NAME [A-Za-z][A-Za-z0-9]*
SUBSCRIPT "_"([A-Za-z0-9]|\{[-+_A-Za-z0-9]+\})
EXPONENT [eE][-+]?[0-9]+

%%

[0-9]+ {
    errno = 0;
    column += yyleng;

    yylval.longVal = strtol(yytext, NULL, 10);
    
    if (errno == ERANGE) {
        yylval.stringVal = yytext;
        return INT_RANGE;
    } else
        return LONG;
}

([0-9]+"."[0-9]*) |
([0-9]+"."?[0-9]*){EXPONENT} |
([0-9]*"."[0-9]+){EXPONENT}? {
    errno = 0;

    column += yyleng;
    yylval.doubleVal = strtod(yytext, NULL);
    
    if (errno == ERANGE) {
        yylval.stringVal = yytext;
        return DOUBLE_RANGE;
    } else
        return DOUBLE;
}

[Ss][Ii][Nn] {
    column += yyleng;
    return SIN;
}

[Cc][Oo][Ss] {
    column += yyleng;
    return COS;
}

[Tt][Aa][Nn] {
    column += yyleng;
    return COS;
}

[Aa][Ss][Ii][Nn] {
    column += yyleng;
    return SIN;
}

[Aa][Cc][Oo][Ss] {
    column += yyleng;
    return COS;
}

[Aa][Tt][Aa][Nn] {
    column += yyleng;
    return COS;
}

[Ll][Oo][Gg] {
    column += yyleng;
    return LOG;
}

[Ss][Qq][Rr][Tt] {
    column += yyleng;
    return SQRT;
}

{NAME}{SUBSCRIPT}? {
    column += yyleng;
    yylval.stringVal = yytext;
    return SYMBOL;
}

[ \t\n\b] {
    column += yyleng;
}

[-+()/*^] {
    column += yyleng;
    return *yytext;
}

. {
    column += yyleng;
    yylval.stringVal = yytext;
    return UNDEFINED;
}

%%

int yywrap(void)
{
    return 1;
}

/* The function controlling parsing of a string. This has to be declared (as extern "C") and invoked
 * from C++ code, it thus represents the interface for parsing strings. While it would be more
 * intuitive to have this function defined in the (yacc) parser source, types and functions used in
 * this functions are only defined in the .c file generated from this file, it's thus the easiest
 * way to keep it in the same compilation unit. The function returns the resulting expression as a
 * void*, which has to be casted to *BasePtr and should then be deleted. */
void *parseString(const char *string)
{
    column = 0;

    YY_BUFFER_STATE buffer = yy_scan_string(string);
    yy_switch_to_buffer(buffer);

    yyparse();

    yy_delete_buffer(buffer);

    /* Defined in the parser source and set at the toplevel rule: */
    return globalResult;
}
