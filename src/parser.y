
%{
    #include <stdio.h>
    #include "parseradapter.h"

    int yylex(void);
    void yyerror(char *message);
    void *globalResult;
    unsigned column;
    char *yytext;
%}

%token LONG
%token DOUBLE
%token SYMBOL
%token INT_RANGE DOUBLE_RANGE UNDEFINED
%token SIN COS TAN ASIN ACOS ATAN
%token SQRT
%token LOG
%token PI

%union {
    char *stringVal;
    long longVal;
    double doubleVal;
    void *basePtr;
}

%type <stringVal> SYMBOL
%type <stringVal> UNDEFINED INT_RANGE DOUBLE_RANGE
%type <doubleVal> DOUBLE
%type <longVal> LONG
%type <basePtr> result expr function error

%left '+' '-'
%left '*' '/'
%nonassoc UNARY
%right '^'

%start result

%%

result: expr {
        globalResult = $1;
    }
    | error
    ;

expr: function
    | UNDEFINED {
        tsym_parserAdapter_logParsingError("Invalid input:", $1, column);
        $$ = tsym_parserAdapter_createUndefined();
    }
    | INT_RANGE {
        tsym_parserAdapter_logParsingError("Integer out of representable range: ", $1, column);
        $$ = tsym_parserAdapter_createMaxInt("Continue with max. integer: ");
    }
    | DOUBLE_RANGE {
        tsym_parserAdapter_logParsingError("Float out of representable range:", $1, column);
        $$ = tsym_parserAdapter_createMaxDouble("Continue with maximum double: ");
    }
    | LONG {
        $$ = tsym_parserAdapter_createInteger($1);
    }
    | DOUBLE {
        $$ = tsym_parserAdapter_createDouble($1);
    }
    | SYMBOL {
        $$ = tsym_parserAdapter_createSymbol($1);
    }
    | expr '+' expr {
        $$ = tsym_parserAdapter_createSum($1, $3);
        tsym_parserAdapter_deletePtrs($1, $3);
    }
    | expr '-' expr {
        $$ = tsym_parserAdapter_createDifference($1, $3);
        tsym_parserAdapter_deletePtrs($1, $3);
    }
    | expr '*' expr {
        $$ = tsym_parserAdapter_createProduct($1, $3);
        tsym_parserAdapter_deletePtrs($1, $3);
    }
    | expr '/' expr {
        $$ = tsym_parserAdapter_createQuotient($1, $3);
        tsym_parserAdapter_deletePtrs($1, $3);
    }
    | '+' expr %prec UNARY {
        $$ = $2;
    }
    | '-' expr %prec UNARY {
        $$ = tsym_parserAdapter_createMinus($2);
        tsym_parserAdapter_deletePtr($2);
    }
    | expr '^' expr {
        $$ = tsym_parserAdapter_createPower($1, $3);
        tsym_parserAdapter_deletePtrs($1, $3);
    }
    | '(' expr ')' {
        $$ = $2;
        tsym_parserAdapter_deletePtr($2);
    }
    ;

function: SIN '(' expr ')' {
        $$ = tsym_parserAdapter_createSine($3);
        tsym_parserAdapter_deletePtr($3);
    }
    | COS '(' expr ')' {
        $$ = tsym_parserAdapter_createCosine($3);
        tsym_parserAdapter_deletePtr($3);
    }
    | TAN '(' expr ')' {
        $$ = tsym_parserAdapter_createTangent($3);
        tsym_parserAdapter_deletePtr($3);
    }
    | ASIN '(' expr ')' {
        $$ = tsym_parserAdapter_createAsine($3);
        tsym_parserAdapter_deletePtr($3);
    }
    | ACOS '(' expr ')' {
        $$ = tsym_parserAdapter_createAcosine($3);
        tsym_parserAdapter_deletePtr($3);
    }
    | ATAN '(' expr ')' {
        $$ = tsym_parserAdapter_createAtangent($3);
        tsym_parserAdapter_deletePtr($3);
    }
    | LOG '(' expr ')' {
        $$ = tsym_parserAdapter_createLogarithm($3);
        tsym_parserAdapter_deletePtr($3);
    }
    | SQRT '(' expr ')' {
        $$ = tsym_parserAdapter_createSquareRoot($3);
        tsym_parserAdapter_deletePtr($3);
    }
    ;

%%

void yyerror(char *s)
{
    tsym_parserAdapter_logParsingError(s, yytext, column);
}
