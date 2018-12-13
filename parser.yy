%{
	#include<cstdio>
	#include<cstring>
	#include "ast.hh"
	#include<vector>
    #include "codegen.cpp"
	#include<iostream>

	void yyerror(const char *s)
	{
		fprintf(stderr, "error: %s\n", s);
		exit(0);
	}
	AST *MAIN;
	extern "C" int yylex();
%}

%start program


%union
{
	AST *ast;
	int integer_val;
	vector<Field_method *> * fields_list;
	Field_method * field_val;
	vector<Variable *> * variable_list;
	Variable * var_val;
	MethodArgs *method_argums;
	Block *blockobj;
	vector<VariableDeclaration *> *vardeclist;
	vector<Statement *> *statement_list;
	VariableDeclaration *vardec;
	Statement *stmt_type;
	MethodCall * mcall;
	vector<Callarg_method *> * call_args_list;
	Location *loc;
	Expression *expr_type;
	char *str_type;
	vector<Expression *> * meth_args_list;
	Callarg_method * callarg_type;
	Literal * lit_type;
	IntLiteral *int_littype;
	StringLit *str_littype;
	CharLit *char_littype;
}
%left <str_type> ','
%right <str_type> '=' PE ME
%left <str_type> OROR
%left <str_type> ANDAND
%left <str_type> EE NE
%left <str_type> '<' '>' LE GE
%left <str_type> '!'
%left <str_type> '+' '-'
%left <str_type> '*' '/' '%'
%left <str_type> '{' '}' '(' ')' '[' ']'

%type <ast> program
%type <fields_list> fields
%type <field_val> field_declaration
%type <variable_list> vars
%type <var_val> var
%type <field_val> method_declaration
%type <method_argums> method_declaration_args
%type <blockobj> block
%type <vardeclist> variable_declarations
%type <statement_list> statement_declarations
%type <vardec> variable_decl
%type <variable_list> variables
%type <stmt_type> statement
%type <mcall> method_call
%type <call_args_list> callout_arguments
%type <loc> location
%type <expr_type> expr
%type <str_type> bin_op
%type <str_type> arith_op
%type <str_type> rel_op
%type <str_type> eq_op
%type <str_type> cond_op
%type <str_type> assign_op
%type <str_type> bool_lit
%type <str_type> type
%type <meth_args_list> method_arguments
%type <callarg_type> common_arg
%type <lit_type> literal
%type <integer_val> int_lit
%type <str_type> str_lit
%type <str_type> char_lit



%token CLASS
%token <str_type> PROGRAM
%token <str_type> ID
%token <integer_val> DECIMAL
%token <integer_val> HEXADECIMAL
%token <str_type> TRUE_INP
%token <str_type> FALSE_INP
%token <str_type> ALPHA
%token <str_type> CALLOUT
%token <str_type> SIC
%token <str_type> STR
%token <str_type> INT
%token <str_type> BOOLEAN
%token <str_type> VOID
%token <str_type> IF
%token <str_type> ELSE
%token <str_type> FOR
%token <str_type> RETURN
%token <str_type> BREAK
%token <str_type> CONTINUE
%%

program:	CLASS PROGRAM '{' fields '}'	{
				Identifier *program_id = new Identifier($2);
				reverse($4->begin(), $4->end());
				MAIN = new AST(program_id, $4);
			}
		;

fields:	%empty {$$ = new vector<Field_method * >; }
		|	field_declaration fields {
			$$ = $2;
			$$->push_back($1);
		}
		|	method_declaration fields {
			$$ = $2;
			$$->push_back($1);
		}


field_declaration:	type vars ';' {
						$$ = new Field($1, $2);
					}
				;

vars:	var {
			$$ = new vector<Variable *>;
			$$->push_back($1);	
		}
	|	vars ',' var {
			$$ = $1;
			$$->push_back($3);
		}
	;

var:	ID {
			$$ = new Identifier($1);
		}
	|	ID '[' int_lit ']' {
			Identifier *name = new Identifier($1);
			$$ = new ArrayDecl(name,  $3);
		}
	;

method_declaration:	type ID  '(' method_declaration_args ')' block {
					Identifier *idx = new Identifier($2);
					$$ = new Method($1, idx, $4, $6);
				}
				|	VOID ID  '(' method_declaration_args ')' block {
					Identifier *idx = new Identifier($2);
					$$ = new Method($1, idx, $4, $6);					
				}
				;

method_declaration_args: %empty {
						$$ = new MethodArgs();
					}
					|	type ID {
						$$ = new MethodArgs();
						Identifier *idx = new Identifier($2);
						($$->_arg)->push_back(make_pair($1, idx));

					}
					|		method_declaration_args ',' type ID {
						$$ = $1;
						Identifier *idx = new Identifier($4);
						($$->_arg)->push_back(make_pair($3, idx));
					}
					;


block:	'{' variable_declarations statement_declarations '}'  {
					reverse($2->begin(), $2->end());
					reverse($3->begin(), $3->end());
					$$ = new Block($2, $3);
				}
	;

variable_declarations:	%empty {
						$$ = new vector<VariableDeclaration *>;
					}
					|	variable_decl variable_declarations {
						$$ = $2;
						$$->push_back($1);
					}
					;

statement_declarations:	%empty	{
						$$ = new vector<Statement *>;
					}
					|	statement statement_declarations {
						$$ = $2;
						$$->push_back($1);
					}
					;

variable_decl:	type variables ';' {
					$$ = new VariableDeclaration($1, $2);
				}	
			;

variables:	ID {
			$$ = new vector<Variable *>;
			Identifier *idx = new Identifier($1);
			$$->push_back(idx);
		}
		|	variables ',' ID {
			$$ = $1;
			Identifier *idx = new Identifier($3);
			$$->push_back(idx);
		}
		|	ID '[' int_lit ']'  {
			$$ = new vector<Variable *>;
			Identifier *idx = new Identifier($1);
			ArrayDecl *arr = new ArrayDecl(idx, $3);
			$$->push_back(arr);
		}
		|	variables ',' ID '[' int_lit ']' {
			$$ = $1;
			Identifier *idx = new Identifier($3);
			ArrayDecl *arr = new ArrayDecl(idx, $5);
			$$->push_back(arr);			
		}
		;

statement:	location assign_op expr ';'	{
			$$ = new AssignStmt($1, $2, $3);
		}
		|	method_call ';' {
			$$ = new MethodCall();
			$$ = $1;
		}
		|	IF '(' expr ')' block  {
			$$ = new IfStmt($3, $5);
		}
		|	IF '(' expr ')' block ELSE block {
			$$ = new IfElseStmt($3, $5, $7);
		}
		|	FOR ID '=' expr ',' expr block {
			Identifier *idx = new Identifier($2);
			$$ = new ForStmt(idx, $4, $6, $7);
		}
		|	RETURN ';' {
			$$ = new ReturnVoidStmt();
		}
		|	RETURN expr ';' {
			$$ = new RetExpStmt($2);
		}
		|	BREAK ';'	{
			$$ = new BreakStmt();
		}
		|	CONTINUE ';' 	{
			$$ = new ContinueStmt();
		}
		|	block 	{
			$$ = new Block;
			$$ = $1;
		}
		;

assign_op:	'='	{$$ = $1;}
		|	PE {$$ = $1;}
		|	ME {$$ = $1;}
		;

type:	INT {
		$$ = $1;	
	}
	| BOOLEAN {
		$$ = $1;
	}
	;

expr:	location	{
		$$ = new Location();
		$$ = $1;
	}
	|	literal	{
		$$ = new Literal();
		$$ = $1;
	}
	|	method_call	{
		$$ = new MethodCall();
		$$ = $1;
	}
	|	expr bin_op expr	{
		$$ = new BinaryExp($1, $2, $3);
	}
	|	'-' expr	{
		$$ = new UnaryExp($1, $2);
	}
	|	'!' expr	{
		$$ = new UnaryExp($1, $2);
	}
	|	'(' expr ')' {
		$$ = $2;
	}
	;

location:	ID {
				$$ = new Identifier($1);
			}
		|	ID '[' expr ']' {
			Identifier *idx = new Identifier($1);
			$$ = new ArrayInd(idx, $3);
		}
		;


method_call:	ID '(' method_arguments ')' {
				Identifier *idx = new Identifier($1);
				$$ = new MethodCall1(idx, $3);
			}
			|	CALLOUT '(' str_lit ')'	{
				vector<Callarg_method *> *callarg_list;
				StringLit *method_name = new StringLit($3);
				$$ = new MethodCall2(method_name, callarg_list);
			}
			|	CALLOUT '(' str_lit ',' callout_arguments ')' {
				StringLit *method_name = new StringLit($3);
				$$ = new MethodCall2(method_name, $5);
			}
			;

method_arguments:	%empty {
					$$ = new vector<Expression *>;
				}
				|	expr {
					$$ = new vector<Expression *>;
					$$->push_back($1);
				}
				|	method_arguments ',' expr {
					$$ = $1;
					$$->push_back($3);
				}
				;

callout_arguments:	common_arg	{
					$$ = new vector<Callarg_method *>;
					$$->push_back($1);
				}
				|	callout_arguments ',' common_arg {
					$$ = $1;
					$$->push_back($3);
				}
				;

common_arg:		expr {
					$$ = new Expression();
					$$ = $1;
				}
		|		str_lit {
					$$ = new StringLit($1);
				}
		;

literal:	int_lit {
			$$ = new Number($1);
		}
		|	char_lit {
			$$ = new CharLit($1);
		}
		|	bool_lit {
            if(strcmp($1, "true")==0) $$ = new BoolLit(1);
            else $$ = new BoolLit(0);
		}
		;

int_lit:	DECIMAL	{
				$$ = $1;
			}
		|	HEXADECIMAL {
				$$ = $1;
			}
		;

char_lit:	SIC ALPHA SIC { 
				$$ = $2;
			}
		;

str_lit:	STR {
				$$ = $1;
			}
		;

bin_op:		arith_op {$$ = $1;}
		|	rel_op {$$ = $1;}
		|	eq_op {$$ = $1;}
		|	cond_op {$$ = $1;}
		;

arith_op:	'+' {$$ = $1;}
		|	'-' {$$ = $1;}
		|	'*' {$$ = $1;}
		|	'/' {$$ = $1;}
		|	'%' {$$ = $1;}
		;

rel_op:		'<' {$$ = $1;}
		|	'>' {$$ = $1;}
		|	LE {$$ = $1;}
		|	GE {$$ = $1;}

eq_op:		EE {$$ = $1;}
		|	NE {$$ = $1;}

cond_op:	ANDAND {$$ = $1;}
		|	OROR {$$ = $1;}

bool_lit:	TRUE_INP { $$ = $1;}
		|	FALSE_INP { $$ = $1;}


%%

int main(int argc, char **argv)
{
	if(argc!=2){
	    std::cerr << "Wrong Input Format" <<std::endl;
	    exit(-1);
	}
	while(yyparse());
	if(strcmp(argv[1], "AST") == 0){
		MAIN->print();
	}
	else{
		if(strcmp(argv[1], "codegen") == 0) {
			CodeGenContext context;
			context.generateCode(MAIN);
		}
	}
	return 0;
}
