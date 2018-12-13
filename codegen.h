#include<stack>
#include<map>
#include<list>
#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Type.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Function.h>
using namespace llvm;

class CodeGenBB {
public:
    llvm::BasicBlock * block;
    std::map<std::string, llvm::Value *> locals;

    CodeGenBB() {}
    CodeGenBB(llvm::BasicBlock * block) {
        this->block = block;
    }
};


class CodeGenContext {
private:
	Function *mainFunction;
	std::stack<CodeGenBB *> blocks;
public:
	Module * module;
	CodeGenContext() {
		this->module = new Module("main", getGlobalContext()); 
	}
	void generateCode(AST *start);
	std::map<std::string, Value *>& locals() {
		return blocks.top()->locals;
	} 
	BasicBlock *topBB() {
		return blocks.top()->block;
	}
	GenericValue runCode();
	void pushBB(BasicBlock *BB){
		blocks.push(new CodeGenBB());
		blocks.top()->block = BB;
	}
	void popBB() {
		blocks.pop();
	}
};