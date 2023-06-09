%option noyywrap
%option nounput
%option noinput
%x comment 
%{
    #include "ast/ast.hpp"
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <vector>
    #include <utility>
    #include "parser.tab.hpp"
    
    int line_index = 1;
%}

%%
"print" { return PRINT; }
"scan" { return SCAN; }
"if" { return IF; }
"else" { return ELSE; }
"while" { return WHILE; }
"ret" { return RET; }
"def" { return DEF; }


[a-zA-Z][a-zA-Z_0-9]* {
    yylval.id = new std::string(yytext);
    return ID; 
}

[0-9][0-9]* {
    yylval.number = atoi(yytext);
    return NUM;
}

"/*" { BEGIN(comment); }
<comment>[^*\n]* {}
<comment>"*"* {}
<comment>\n { line_index++; }
<comment>\*+"/" { BEGIN(INITIAL); }

"+" { return PLUS; }
"-" { return MINUS; }
"*" { return MUL; }
"/" { return DIV; }
"%" { return MOD; }

":=" { return ASSIGN; }
";" { return SEMIC; }
"," { return COMMA; }
"::" { return DCOL; }

"(" { return LP; }
")" { return RP; }

"{" { return LCP; }
"}" { return RCP; }

"[" { return LQP; }
"]" { return RQP; }

">>" { return SHR; }
"<<" { return SHL; }

"&&" { return AND; }
"||" { return OR; }
"!" { return NOT; }

"<" { return LESS; }
">" { return GREAT; }
"=" { return EQ; }
"/=" { return NEQ; }
"<=" { return LEQ; }
">=" { return GEQ; }

[ \t] {};
\n { line_index++; }

. {
    fprintf(stderr, "Error in %s, line %d : \n", __FILE__, line_index);
    fprintf(stderr, "Uknown lexema -> {%s}\n", yytext);
    return 1;
}

%%
