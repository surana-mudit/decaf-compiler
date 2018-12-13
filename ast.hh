#include<list>
#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
#include<llvm/IR/Value.h>
#include<bits/stdc++.h>

using namespace std;
class CodeGenContext;
class Field_method{
public:
	virtual void print(){
		cout << "Field_method\n";
	}
	virtual llvm::Value* codeGen(CodeGenContext& context);

};

class Variable{
public:
	virtual void print(){}	
virtual llvm::Value* codeGen(CodeGenContext& context);

};

class Statement{
public:
	virtual void print(){}
    virtual llvm::Value* codeGen(CodeGenContext& context);

};

class Callarg_method{
public:
	virtual void print(){}
    virtual llvm::Value* codeGen(CodeGenContext& context);

};

class Expression: public Callarg_method{
public:
	char *_id;
	virtual void print(){}
    virtual llvm::Value* codeGen(CodeGenContext& context);

};

class Location:public Expression{
public:
    virtual void print(){}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class Identifier:public Location,public Variable{
private:
public:
	char * _id;
	Identifier(char *id){
		this->_id = id;
	}
	Identifier(){}
	void print(){
		printf("%s", this->_id);
	}
    virtual llvm::Value* codeGen(CodeGenContext& context);

};

class VariableDeclaration: public Variable{
private:
	char * _type;
	std::vector<Variable *> * _variables;
public:
	VariableDeclaration(char *type, vector<Variable *> * var){
		this->_type = type;
		this->_variables = var;
	}
	VariableDeclaration(){}
	void print(){
		printf("%s ", this->_type);
		for(int i=0;i<(this->_variables)->size();i++){
			(*_variables)[i]->print();
			printf(" ");
		}
		printf("\n");
	}
    virtual llvm::Value* codeGen(CodeGenContext& context);

};

class Block: public Statement{
private:
	vector<VariableDeclaration *> * _vars;
	vector<Statement *> * _statements;
public:
	Block(vector<VariableDeclaration *> * vars, vector<Statement *> * states){
		this->_vars = vars;
		this->_statements = states;
	}
	Block(){}

	void print(){
		for(int i=0;i<(this->_vars)->size();i++){
			(*_vars)[i]->print();
		}
		fflush(stdout);
		for(int i=0;i<(this->_statements)->size();i++){
			(*_statements)[i]->print();
			printf("\n");
		}
	}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};


class Field: public Field_method{
private:
	char *_type;
	std::vector<Variable *> * _vars;
public:
	Field(char *type, vector<Variable *> * vars){
		this->_type = type;
		this->_vars = vars;
	}
	Field(){}

	void print(){
		printf("%s\n", this->_type);
		for(int i=0;i<(_vars)->size();i++){
			(*_vars)[i]->print();
		}
	}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class MethodArgs{
// private:
public:
	vector< pair< char *, Identifier * > > * _arg;
	MethodArgs(vector< pair< char *, Identifier * > > * arg){
		this->_arg = arg;
	}
	MethodArgs(){
		(this->_arg) = new vector< pair< char *, Identifier * > >;
	}

	void print(){
		printf("( ");
		for(int i=0;i<(this->_arg)->size();i++){
			printf("%s ", (*_arg)[i].first);
			(*_arg)[i].second->print();
			printf(" ");
		}
		printf(")");
	}
    // virtual llvm::Value* codeGen(CodeGenContext& context){}

};

class Method:public Field_method{
private:
	char *_type;
	MethodArgs *_args;
	Block * _block;
public:
	Identifier *_id;
	Method(char *type, Identifier *id, MethodArgs * arg, Block * block){
		this->_type = type;
		this->_id = id;
		this->_args = arg;
		this->_block = block;
	}
	Method(){}

	void print(){
		printf("%s\n", this->_type);
		(this->_id)->print();
		printf(" ");
		(this->_args)->print();
		printf("\n");
		(this->_block)->print();
	}
    virtual llvm::Value* codeGen(CodeGenContext& context);

};

class AST{
private:
	Identifier * _id;
	vector<Field_method *> * _fields;
public:
	AST(Identifier * id, vector<Field_method *> * fields){
		this->_id = id;
		this->_fields = fields;
	}
	AST(){}
	void print(){
		printf("CLASS ");
		this->_id->print();
		printf("\n");
		for(int i=0;i<(this->_fields)->size();i++){
			(*_fields)[i]->print();
			printf("\n");
		}
	}
    virtual llvm::Value* codeGen(CodeGenContext& context);

};


class UnaryExp:public Expression{
private:
    char* _op;
    Expression* _expr;

public:
    UnaryExp(char* op,Expression* expr)
    {
        (this->_op)=op;
        this->_expr = expr;
    }
    UnaryExp(){}

    void print() {
        printf("%s ",this->_op);
        (this->_expr)->print();
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);

};

class BinaryExp:public Expression{
private:
    Expression* _lc;
    Expression* _rc;
    char* _bin_op;
public:
    BinaryExp(Expression* lc,char* bin_op,Expression* rc)
    {
        this->_lc=lc;
        this->_bin_op=bin_op;
        this->_rc=rc;
    }
    BinaryExp(){}

    void print() 
    {
        (this->_lc)->print();
        printf("%s ", this->_bin_op);
        (this->_rc)->print();
    }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class Literal: public Expression{
public:
	virtual void print(){}
    virtual llvm::Value* codeGen(CodeGenContext& context);

};

class IntLiteral: public Literal{
public:
	virtual void print(){}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class Number: public IntLiteral{
private:
	int _val;
public:
	Number(int value){
		this->_val = value;
	}
	Number(){}

	void print(){
		printf("%d ", this->_val);
	}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class HexLiteral: public IntLiteral{
private:
	int _val;
public:
	HexLiteral(int value){
		this->_val = value;
	}
	HexLiteral(){}

	void print(){
		cout << this->_val << endl;
	}
    virtual llvm::Value* codeGen(CodeGenContext& context){}
};

class CharLit: public Literal{
private:
	char *_c;
public:
	CharLit(char *c){
		this->_c = c;
	}
	CharLit(){}

	void print(){
		printf("%s\n", this->_c);
	}
    virtual llvm::Value* codeGen(CodeGenContext& context){}
};

class BoolLit: public Literal{
private:
	bool _c;
public:
	BoolLit(bool c){
		this->_c = c;
	}
	BoolLit(){}

	void print(){
		cout << this->_c << endl;
	}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class StringLit: public Callarg_method{
private:
public:
	char *_str;
	StringLit(char *str){
		this->_str = str;
	}
	StringLit(){}

	void print(){
		printf("%s\n", this->_str);
	}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class ArrayInd:public Location{
private:
	Expression *_expr;
	Identifier *_id;
public:
	ArrayInd(Identifier *id, Expression *expr){
		this->_id = id;
		this->_expr = expr;
	}
	ArrayInd(){}

	void print(){
		(this->_id)->print();
		printf("[");
		(this->_expr)->print();
		printf("]");
	}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class MethodCall:public Expression, public Statement{
public:
	virtual void print(){}
    virtual llvm::Value* codeGen(CodeGenContext& context);

};

class MethodCall1: public MethodCall{
private:
	Identifier *_method_name;
	vector<Expression *> * _exprs;
public:
	MethodCall1(Identifier *method_name, vector<Expression *> * exprs){
		this->_method_name = method_name;
		this->_exprs = exprs;
	}
	MethodCall1(){}

	void print(){
		(this->_method_name)->print();
		printf("( ");
		for(int i=0;i<(_exprs)->size();i++){
			(*_exprs)[i]->print();
			printf(", ");
		}
		printf(" )");
	}
    virtual llvm::Value* codeGen(CodeGenContext& context);

};

class MethodCall2: public MethodCall{
private:
	StringLit *_method_name;
	vector<Callarg_method *> * _exprs;
public:
	MethodCall2(StringLit *method_name, vector<Callarg_method *> * exprs){
		this->_method_name = method_name;
		this->_exprs = exprs;
	}
	MethodCall2(){}

	void print(){
		(this->_method_name)->print();
		for(int i=0;i<(this->_exprs)->size();i++){
			(*(this->_exprs))[i]->print();
		}
	}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class AssignStmt:public Statement{
private:
	Expression * _loc;
	char * _op;
	Expression * _expr;
public:
	AssignStmt(Expression *loc, char *op, Expression *expr){
		this->_loc = loc;
		this->_op = op;
		this->_expr = expr;
	}
	AssignStmt(){}

	void print(){
		(this->_loc)->print();
		printf(" %s ", this->_op);
		(this->_expr)->print();
		printf("\n");
	}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class IfStmt: public Statement{
private:
	Expression *_expr;
	Block *_block1;
	Block *_block2;
public:
	IfStmt(Expression *expr, Block *block){
		this->_block1 = block;
		this->_expr = expr;
		this->_block2 = NULL;
	}
	IfStmt(){}

	void print(){
		printf("if (");
		(this->_expr)->print();
		printf(" )\n");
		(this->_block1)->print();
	}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class IfElseStmt: public Statement{
private:
	Expression *_expr;
	Block *_block1;
	Block *_block2;
public:
	IfElseStmt(Expression *expr, Block *block1, Block *block2){
		this->_block1 = block1;
		this->_block2 = block2;
		this->_expr = expr;
	}
	IfElseStmt(){}

	void print(){
		printf("if\n");
		(this->_expr)->print();
		(this->_block1)->print();
		printf("else\n");
		(this->_block2)->print();
		printf("\n");

	}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class ForStmt: public Statement{
private:
	Block *_block;
	Identifier *_i;
	Expression *_start;
	Expression *_end;
public:
	ForStmt(Identifier *i, Expression *start, Expression *end, Block *block){
		this->_block = block;
		this->_i = i;
		this->_start = start;
		this->_end = end;
	}
	ForStmt(){}

	void print(){
		printf("FOR ");
		(this->_i)->print();
		printf("=");
		(this->_start)->print();
		(this->_end)->print();
		printf("\n");
		(this->_block)->print();
	}
    virtual llvm::Value* codeGen(CodeGenContext& context);

};

class ReturnVoidStmt:public Statement{
public:
	void print(){
		printf("return");
	}
    virtual llvm::Value* codeGen(CodeGenContext& context);

};

class BreakStmt:public Statement{
public:
	virtual void print(){}
    virtual llvm::Value* codeGen(CodeGenContext& context);

};

class RetExpStmt:public Statement{
private:
	Expression *_expr;
public:
	RetExpStmt(Expression *expr){
		this->_expr = expr;
	}
	RetExpStmt();

	void print(){
		printf("return ");
		(this->_expr)->print();
	}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

class ContinueStmt:public Statement{
public:
	virtual void print(){}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};




class ArrayDecl:public Variable{
private:
	Identifier *_id;
	int _size;
public:
	ArrayDecl(Identifier *id, int size){
		this->_id = id;
		this->_size = size;
	}
	ArrayDecl(){}

	void print(){
		(this->_id)->print();
		printf("[ %d ]", (this->_size));
	}
    virtual llvm::Value* codeGen(CodeGenContext& context);
};