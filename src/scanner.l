
%{
    #include "parser.h"
    #include <stdlib.h>
    #include <errno.h>

    extern void yyerror(char *message);

    extern void *result;
    extern int foundSyntaxError;

    static unsigned currentColumn;
    static unsigned errorColumn;

    void updateColumnCount()
    {
        if (!foundSyntaxError)
            errorColumn = currentColumn + 1;

        currentColumn += yyleng;
    }
%}

NAME [A-Za-z][A-Za-z0-9]*
SUBSCRIPT "_"([A-Za-z0-9]|\{[A-Za-z0-9]+\})
EXPONENT [eE][-+]?[0-9]+

%%

[0-9]+ {
    errno = 0;

    yylval.longVal = strtol(yytext, NULL, 10);
    
    if (errno == ERANGE) {
        updateColumnCount();
        foundSyntaxError = 1;
        yylval.stringVal = yytext;
        return LONG_RANGE;
    } else {
        updateColumnCount();
        return LONG;
    }
}

([0-9]+"."[0-9]*) |
([0-9]+"."?[0-9]*){EXPONENT} |
([0-9]*"."[0-9]+){EXPONENT}? {
    errno = 0;

    yylval.doubleVal = strtod(yytext, NULL);
    
    if (errno == ERANGE) {
        updateColumnCount();
        foundSyntaxError = 1;
        yylval.stringVal = yytext;
        return DOUBLE_RANGE;
    } else {
        updateColumnCount();
        return DOUBLE;
    }
}

[Ss][Ii][Nn] {
    updateColumnCount();
    return SIN;
}

[Cc][Oo][Ss] {
    updateColumnCount();
    return COS;
}

[Tt][Aa][Nn] {
    updateColumnCount();
    return TAN;
}

[Aa][Ss][Ii][Nn] {
    updateColumnCount();
    return ASIN;
}

[Aa][Cc][Oo][Ss] {
    updateColumnCount();
    return ACOS;
}

[Aa][Tt][Aa][Nn] {
    updateColumnCount();
    return ATAN;
}

[Ll][Oo][Gg] {
    updateColumnCount();
    return LOG;
}

[Ss][Qq][Rr][Tt] {
    updateColumnCount();
    return SQRT;
}

{NAME}{SUBSCRIPT}? {
    updateColumnCount();
    yylval.stringVal = yytext;
    return SYMBOL;
}

[ \t\n\b] {
    updateColumnCount();
}

[-+()/*^] {
    updateColumnCount();
    return *yytext;
}

. {
    updateColumnCount();

    yyerror("Character not recognized: ");

    yylval.stringVal = yytext;
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
    YY_BUFFER_STATE buffer;
    result = NULL;
    foundSyntaxError = 0;
    currentColumn = 0;
    errorColumn = 0;
    
    buffer = yy_scan_string(string);
    yy_switch_to_buffer(buffer);

    yyparse();

    yy_delete_buffer(buffer);

    /* Defined in the parser source and set at the toplevel rule: */
    return result;
}

unsigned getErrorColumn()
{
    return errorColumn;
}
