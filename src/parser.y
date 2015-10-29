
%{
    #include <stdio.h>
    #include <string.h>
    #include "parseradapter.h"

    int yylex(void);
    int foundSyntaxError;
    void yyerror(char *message);
    void *result;
    char *yytext;
%}

%token LONG
%token DOUBLE
%token SYMBOL
%token INT_RANGE DOUBLE_RANGE
%token SIN COS TAN ASIN ACOS ATAN
%token SQRT
%token LOG

%union {
    char *stringVal;
    long longVal;
    double doubleVal;
    void *basePtr;
}

%type <stringVal> SYMBOL
%type <stringVal> INT_RANGE DOUBLE_RANGE
%type <doubleVal> DOUBLE
%type <longVal> LONG
%type <basePtr> parsedExpr expr function

%left '+' '-'
%left '*' '/'
%nonassoc UNARY
%right '^'

%start parsedExpr

%%

parsedExpr: expr {
        result = $1;
    }
    ;

expr: function
    | INT_RANGE {
        tsym_parserAdapter_logParsingError("Integer out of representable range: ", $1);
        result = $$ = tsym_parserAdapter_createMaxInt("Continue with max. integer: ");
    }
    | DOUBLE_RANGE {
        tsym_parserAdapter_logParsingError("Float out of representable range: ", $1);
        result = $$ = tsym_parserAdapter_createMaxDouble("Continue with maximum double: ");
    }
    | LONG {
        result = $$ = tsym_parserAdapter_createInteger($1);
    }
    | DOUBLE {
        result = $$ = tsym_parserAdapter_createDouble($1);
    }
    | SYMBOL {
        result = $$ = tsym_parserAdapter_createSymbol($1);
    }
    | expr '+' expr {
        result = $$ = tsym_parserAdapter_createSum($1, $3);
        tsym_parserAdapter_deletePtrs($1, $3);
    }
    | expr '-' expr {
        result = $$ = tsym_parserAdapter_createDifference($1, $3);
        tsym_parserAdapter_deletePtrs($1, $3);
    }
    | expr '*' expr {
        result = $$ = tsym_parserAdapter_createProduct($1, $3);
        tsym_parserAdapter_deletePtrs($1, $3);
    }
    | expr '/' expr {
        result = $$ = tsym_parserAdapter_createQuotient($1, $3);
        tsym_parserAdapter_deletePtrs($1, $3);
    }
    | '+' expr %prec UNARY {
        result = $$ = $2;
    }
    | '-' expr %prec UNARY {
        result = $$ = tsym_parserAdapter_createMinus($2);
        tsym_parserAdapter_deletePtr($2);
    }
    | expr '^' expr {
        result = $$ = tsym_parserAdapter_createPower($1, $3);
        tsym_parserAdapter_deletePtrs($1, $3);
    }
    | '(' expr ')' {
        result = $$ = $2;
    }
    ;


function: SIN '(' expr ')' {
        result = $$ = tsym_parserAdapter_createSine($3);
        tsym_parserAdapter_deletePtr($3);
    }
    | COS '(' expr ')' {
        result = $$ = tsym_parserAdapter_createCosine($3);
        tsym_parserAdapter_deletePtr($3);
    }
    | TAN '(' expr ')' {
        result = $$ = tsym_parserAdapter_createTangent($3);
        tsym_parserAdapter_deletePtr($3);
    }
    | ASIN '(' expr ')' {
        result = $$ = tsym_parserAdapter_createAsine($3);
        tsym_parserAdapter_deletePtr($3);
    }
    | ACOS '(' expr ')' {
        result = $$ = tsym_parserAdapter_createAcosine($3);
        tsym_parserAdapter_deletePtr($3);
    }
    | ATAN '(' expr ')' {
        result = $$ = tsym_parserAdapter_createAtangent($3);
        tsym_parserAdapter_deletePtr($3);
    }
    | LOG '(' expr ')' {
        result = $$ = tsym_parserAdapter_createLogarithm($3);
        tsym_parserAdapter_deletePtr($3);
    }
    | SQRT '(' expr ')' {
        result = $$ = tsym_parserAdapter_createSquareRoot($3);
        tsym_parserAdapter_deletePtr($3);
    }
    ;

%%

void yyerror(char *s)
{
    foundSyntaxError = 1;

    if (strcmp(s, "syntax error") != 0)
        tsym_parserAdapter_logParsingError(s, yytext);
    else
        tsym_parserAdapter_logParsingError("Yacc: found syntax error", "");
}
