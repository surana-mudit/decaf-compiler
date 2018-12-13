%{
	#include "ast.hh"
	#include<stdio.h>
	#include<string.h>
	#include<stdlib.h>	
	#include "parser.tab.hh"
	int lineno=0;
    extern "C" int yylex();

%}

digit [0-9]
alpha [a-zA-Z]
id_reg {alpha}({alpha}|{digit})*
dec {digit}+(\.{digit}+)?(E[+-]?{digit}+)?
hex 0x(digit|[A-Fa-f])+ 
str \"(\\.|[^"\\])*\"

%%
"(" 				{/*printf("(");*/     yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1); return '(';}
")" 				{/*printf(")");*/     yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1); return ')';}
"[" 				{/*printf("[");*/     yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1); return '[';}
"]" 				{/*printf("]");*/     yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1); return ']';}
"{" 				{/*printf("{");*/     yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1); return '{';}
"}" 				{/*printf("}");*/     yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1); return '}';}
"+" 				{/*printf("+");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return '+';}
"-" 				{/*printf("-");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return '-';}
"*" 				{/*printf("*");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return '*';}
";"					{/*printf(";");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return ';';}
"/" 				{/*printf("/");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return '/';}
"%" 				{/*printf("%%");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return '%';}
\n					{/*printf("\n");*/ ++lineno;}
"||" 				{/*printf("||");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return OROR;}
"&&" 				{/*printf("&&");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return ANDAND;}
"!" 				{/*printf("!");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return '!';}
"==" 				{/*printf("==");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return EE;}
"!=" 				{/*printf("!=");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return NE;}
">=" 				{/*printf(">=");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return GE;}
"<=" 				{/*printf("<=");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return LE;}
">" 				{/*printf(">");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return '>';}
"<" 				{/*printf("<");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return '<';}
"+="				{/*printf("+=");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return PE;}
"-="				{/*printf("-=");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return ME;}
"="					{/*printf("=");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return '=';}
"," 				{/*printf(",");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return ',';}
"'"					{/*printf("'");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return SIC;}
" "					{/*printf("SPACE");*/}
"class"				{/*printf("class");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return CLASS;}
"Program"			{/*printf("Program");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return PROGRAM;}
"int"				{/*printf("int");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return INT;}
"boolean"			{/*printf("boolean");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return BOOLEAN;}
"void"				{/*printf("void");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return VOID;}
"if"				{/*printf("if");*/ return IF;}
"else"				{/*printf("else");*/ return ELSE;}
"for"				{/*printf("for");*/ return FOR;}
{dec}				{/*printf("dec");*/ 
					yylval.integer_val = atoi(yytext);return DECIMAL;}
"return"			{/*printf("return");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return RETURN;}
"break"				{/*printf("break");*/  return BREAK;}
"continue"			{/*printf("continue");*/ return CONTINUE;}
"callout" 			{/*printf("CALLOUT");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return CALLOUT;}
"true" 				{/*printf("true");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return TRUE_INP;}
"false" 			{/*printf("false");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return FALSE_INP;}
{str}				{/*printf("str");*/ 
yylval.str_type=(char*)malloc((yyleng+1)*(sizeof(char)));
                        strncpy(yylval.str_type,yytext+1,yyleng-2);
                        yylval.str_type[yyleng-2]='\0';return STR;}
{hex}				{/*printf("hex");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return HEXADECIMAL;}
{id_reg}			{/*printf("ID");*/ 
    yylval.str_type=(char *)malloc((yyleng+1)*(sizeof(char)));
    strncpy(yylval.str_type, yytext, yyleng + 1);return ID;}
.					{ printf("Line No %d: Unrecognized Character\n", lineno);}
%%