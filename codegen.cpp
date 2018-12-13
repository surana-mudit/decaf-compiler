#include"codegen.h"
#include<string.h>
class CodeGenContext;
static IRBuilder<> Builder(getGlobalContext());

void CodeGenContext::generateCode(AST *start) {
    FunctionType *ftype = FunctionType::get(Type::getVoidTy(getGlobalContext()), false);
    mainFunction = Function::Create(ftype, GlobalValue::ExternalLinkage, "main", module);
    BasicBlock *block = BasicBlock::Create(getGlobalContext(), "entry", mainFunction, 0);

    pushBB(block);
    *start->codeGen(*this);
    BasicBlock *top = this->topBB();
    popBB();
    ReturnInst::Create(getGlobalContext(), top);
    

    legacy::PassManager PM;
    PM.add(createPrintModulePass(outs()));
    PM.run(*module);

}

Value * ErrorHandler(const char * error) {
    std::cerr << error <<std::endl;
    exit(-1);
}

int checkmain = 0, flag=0;
Value *AST::codeGen(CodeGenContext& context) {

    Function * iterator = NULL;
    Function * uMain = NULL;
    for(int i=0;i<(this->_fields)->size();i++) {
        // Field * fl = dynamic_cast<Field *> (_fields)[i];
        // if (fl) {
        //     fl->codeGen(context);
        // }
        // Method *meth = dynamic_cast<Method *> (_fields)[i];
        // if (meth) {
        //     iterator = static_cast<Function *> (*_fields)[i]->codeGen(context);
        //     if(strcmp(meth->_id->_id,"main")==0 && !uMain) {
        //         uMain = iterator;
        //     }
        // }
        Value *out = (*_fields)[i]->codeGen(context);
        if(flag){
            iterator = static_cast<Function *> (out);
            flag = 0;
        }

    }
    if(!checkmain){
        ErrorHandler("No main");        
    }
    else {
        CallInst::Create(iterator, "", context.topBB());        
    }
    // for(int i=0;i<(this->_methods)->size();i++){
    //     if((*_methods)[i]->_id->_id == "main" && uMain) {
    //         return ErrorHandler("Multiple Declaration of main");
    //     }
    //     iterator = static_cast<Function *> (*_methods)[i]->codeGen(context);
    //     if((*_methods)[i]->_id->_id == "main" && !uMain) {
    //         uMain = iterator;
    //     }
    // }

    // if(!uMain){
    //     ErrorHandler("No main");
    // }
    // else {
    //     CallInst::Create(uMain, "", context.topBB());
    // }
    return NULL;
}

Value *Field_method::codeGen(CodeGenContext& context) {
    Field *fl = dynamic_cast<Field *> (this);
    Method *meth = dynamic_cast<Method *> (this);
    if (fl) {
        return fl->codeGen(context);
    }
    if (meth) {
        // cout << meth->_id->_id;
        if (strcmp(meth->_id->_id,"main")==0){
            checkmain = 1;
            flag=1;
        }
        return meth->codeGen(context);
    }
}

Value *Variable::codeGen(CodeGenContext& context) {
    ErrorHandler("Aa jaa yahaaaaaaa");
    ArrayDecl *arrdec = dynamic_cast<ArrayDecl *> (this);
    if (arrdec) {
        return arrdec->codeGen(context);
    }
    Identifier *vardec = dynamic_cast<Identifier *> (this);
    if (vardec) {
        // cout << vardec->_id;
        AllocaInst * allocaInst = new AllocaInst(Type::getInt64Ty(getGlobalContext()), (vardec->_id), context.topBB());
        new StoreInst(ConstantInt::get(Type::getInt64Ty(getGlobalContext()), 0, true), allocaInst, false, context.topBB());
        context.locals()[vardec->_id] = allocaInst;
    }
    return NULL;
}


Value *Field::codeGen(CodeGenContext& context){
    Value * val;
    for(int i=0;i<(_vars)->size();i++){
        (*_vars)[i]->print();
        (*_vars)[i]->codeGen(context);
    }
    return NULL;
}


Value *VariableDeclaration::codeGen(CodeGenContext& context) {
    return NULL;
}

Value *ArrayDecl::codeGen(CodeGenContext& context) {
    GlobalVariable* variable = new GlobalVariable(*context.module, ArrayType::get(Type::getInt64Ty(getGlobalContext()), (this->_size)), false, GlobalValue::CommonLinkage, NULL, (this->_id->_id));
    variable->setInitializer(ConstantAggregateZero::get(ArrayType::get(Type::getInt64Ty(getGlobalContext()), (this->_size))));
    context.locals()[(this)->_id->_id] = variable;
    return variable;    
}

Type *parse_type(string type) 
{
    if (type.compare("int") == 0) {
        return Type::getInt64Ty(getGlobalContext());
    }
    else if (type.compare("boolean")==0)
        return Type::getInt1Ty(getGlobalContext());

    return Type::getVoidTy(getGlobalContext());
}

Value *Method::codeGen(CodeGenContext& context) {
    if(strcmp(this->_id->_id,"main")==0){
        flag = 1;
        checkmain = 1;
    }
    vector<Type *> arg_types;
    for(int i=0;i<(this->_args->_arg)->size();i++){
        arg_types.push_back(parse_type((*(this->_args->_arg))[i].first));
    }

    FunctionType *ftype = FunctionType::get(parse_type(this->_type),makeArrayRef(arg_types),false);
    Function *function = Function::Create(ftype,GlobalValue::InternalLinkage,this->_id->_id,context.module);
    BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", function, 0);
    Builder.SetInsertPoint(bblock);
    context.pushBB(bblock);

    for(int i=0;i<(this->_args->_arg)->size();i++){
        AllocaInst *alloc = new AllocaInst(parse_type((*(this->_args->_arg))[i].first), (*(this->_args->_arg))[i].second->_id, context.topBB());
        context.locals()[(*(this->_args->_arg))[i].second->_id] = alloc;
    }
    this->_block->codeGen(context);
    if(strcmp(this->_type, "void") == 0){
        ReturnInst::Create(getGlobalContext(), context.topBB());
    }
    else{
        ReturnInst::Create(getGlobalContext(), ConstantInt::get(Type::getInt64Ty(getGlobalContext()), 0, true), context.topBB());       
    }
    context.popBB();
    return function;

}

Value * Expression::codeGen(CodeGenContext& context) {
    BinaryExp *bxp = dynamic_cast<BinaryExp *> (this);
    Literal *lit = dynamic_cast<Literal *> (this);
    UnaryExp *uxp = dynamic_cast<UnaryExp *> (this);
    Location *loc = dynamic_cast<Location *> (this);  
    MethodCall *meth = dynamic_cast<MethodCall *> (this);
    if(bxp) {
        return bxp->codeGen(context);
    }
    if(lit) {
        return lit->codeGen(context);
    }
    if(uxp) {
        return uxp->codeGen(context);
    }
    if(loc) {
        return loc->codeGen(context);
    }
}

Value *Block::codeGen(CodeGenContext& context) {
    for(int i=0;i<(this->_vars)->size();i++) {
        (*_vars)[i]->codeGen(context);
    }
    for(int i=0;i<(this->_statements)->size();i++){
        (*_statements)[i]->codeGen(context);
    }
    return NULL;
}

Value * BinaryExp::codeGen(CodeGenContext& context) {
    char *op = this->_bin_op;
    if(strcmp(op, "+")==0)
    {
        return llvm::BinaryOperator::Create(llvm::Instruction::Add, static_cast<llvm::Value*>((this->_lc)->codeGen(context)), static_cast<llvm::Value*>((this->_rc)->codeGen(context)), "tmp", context.topBB());
    }

    else if(strcmp(op, "-")==0)
    {
        return llvm::BinaryOperator::Create(llvm::Instruction::Sub, static_cast<llvm::Value*>((this->_lc)->codeGen(context)), static_cast<llvm::Value*>((this->_rc)->codeGen(context)), "tmp", context.topBB());
    }

    else if(strcmp(op, "*")==0)
    {
        return llvm::BinaryOperator::Create(llvm::Instruction::Mul, static_cast<llvm::Value*>((this->_lc)->codeGen(context)), static_cast<llvm::Value*>((this->_rc)->codeGen(context)), "tmp", context.topBB());
    }

    else if(strcmp(op, "/")==0)
    {
        return llvm::BinaryOperator::Create(llvm::Instruction::SDiv, static_cast<llvm::Value*>((this->_lc)->codeGen(context)), static_cast<llvm::Value*>((this->_rc)->codeGen(context)), "tmp", context.topBB());
    }
    
    else if(strcmp(op, "%")==0)
    {
        return llvm::BinaryOperator::Create(llvm::Instruction::SRem, static_cast<llvm::Value*>((this->_lc)->codeGen(context)), static_cast<llvm::Value*>((this->_rc)->codeGen(context)), "tmp", context.topBB());
    }
    
    else if(strcmp(op, "&&")==0)
    {
        return llvm::BinaryOperator::Create(llvm::Instruction::And, static_cast<llvm::Value*>((this->_lc)->codeGen(context)), static_cast<llvm::Value*>((this->_rc)->codeGen(context)), "tmp", context.topBB());
    }
    
    else if(strcmp(op, "||")==0)
    {
        return llvm::BinaryOperator::Create(llvm::Instruction::Or, static_cast<llvm::Value*>((this->_lc)->codeGen(context)), static_cast<llvm::Value*>((this->_rc)->codeGen(context)), "tmp", context.topBB());
    }

    else if(strcmp(op, "==")==0)
    {
        return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_EQ, static_cast<llvm::Value*>((this->_lc)->codeGen(context)), static_cast<llvm::Value*>((this->_rc)->codeGen(context)),"tmp", context.topBB()), llvm::Type::getInt64Ty(llvm::getGlobalContext()), "zext", context.topBB());
    }
    
    else if(strcmp(op, "<=")==0)
    {
        return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SLE, static_cast<llvm::Value*>((this->_lc)->codeGen(context)), static_cast<llvm::Value*>((this->_rc)->codeGen(context)),"tmp", context.topBB()), llvm::Type::getInt64Ty(llvm::getGlobalContext()), "zext", context.topBB());
    }

    else if(strcmp(op, ">=")==0)
    {
        return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SGT, static_cast<llvm::Value*>((this->_lc)->codeGen(context)), static_cast<llvm::Value*>((this->_rc)->codeGen(context)),"tmp", context.topBB()), llvm::Type::getInt64Ty(llvm::getGlobalContext()), "zext", context.topBB());
    }
    
    else if(strcmp(op, "<")==0)
    {
        return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SLT, static_cast<llvm::Value*>((this->_lc)->codeGen(context)), static_cast<llvm::Value*>((this->_rc)->codeGen(context)),"tmp", context.topBB()), llvm::Type::getInt64Ty(llvm::getGlobalContext()), "zext", context.topBB());
    }

    else if(strcmp(op, ">")==0)
    {
        return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_SGT, static_cast<llvm::Value*>((this->_lc)->codeGen(context)), static_cast<llvm::Value*>((this->_rc)->codeGen(context)),"tmp", context.topBB()), llvm::Type::getInt64Ty(llvm::getGlobalContext()), "zext", context.topBB());
    }

    else if(strcmp(op, "!=")==0)
    {
        return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_NE, static_cast<llvm::Value*>((this->_lc)->codeGen(context)), static_cast<llvm::Value*>((this->_rc)->codeGen(context)),"tmp", context.topBB()), llvm::Type::getInt64Ty(llvm::getGlobalContext()), "zext", context.topBB());
    }
}

Value *UnaryExp::codeGen(CodeGenContext& context) {
    char *op = this->_op;
    if(strcmp(op, "-")==0)
        return llvm::BinaryOperator::Create(llvm::Instruction::Sub, llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), 0, true), static_cast<llvm::Value*>((this->_expr)->codeGen(context)), "tmp", context.topBB());
    else if(strcmp(op, "!")==0)
    return new llvm::ZExtInst(llvm::CmpInst::Create(llvm::Instruction::ICmp, llvm::ICmpInst::ICMP_EQ, llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), 0, true), static_cast<llvm::Value*>((this->_expr)->codeGen(context)),"tmp", context.topBB()), llvm::Type::getInt64Ty(llvm::getGlobalContext()), "zext", context.topBB());    
}

Value *Literal::codeGen(CodeGenContext& context) {
    IntLiteral *intlit = dynamic_cast<IntLiteral *> (this);
    CharLit *charlit = dynamic_cast<CharLit *> (this);
    BoolLit *boollit = dynamic_cast<BoolLit *> (this);
    if(intlit){
        return intlit->codeGen(context);
    }
    if(charlit){
        return charlit->codeGen(context);
    }
    if(boollit){
        return boollit->codeGen(context);
    }
}

Value * IntLiteral::codeGen(CodeGenContext& context) {
    Number *numb = static_cast<Number *> (this);
    return numb->codeGen(context);
}

Value *Number::codeGen(CodeGenContext& context) {
    return ConstantInt::get(Type::getInt64Ty(getGlobalContext()),this->_val,true);
}

Value *BoolLit::codeGen(CodeGenContext& context) {
    int num = this->_c;
    return ConstantInt::get(Type::getInt64Ty(getGlobalContext()),num,true);
}

Value * Location::codeGen(CodeGenContext& context) {
    ArrayInd *aind = dynamic_cast<ArrayInd *> (this);
    Identifier *idx = dynamic_cast<Identifier *> (this);
    if(idx){
        return idx->codeGen(context);
    }
    else return aind->codeGen(context);
}

Value *ArrayInd::codeGen(CodeGenContext& context) {
    if(context.locals().find(this->_id->_id) == context.locals().end()) {
        return ErrorHandler("Variable ArrayInd Not Declared");
    }
    std::vector<Value *> index;
    index.push_back(llvm::ConstantInt::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), 0, true));
    index.push_back(static_cast<llvm::Value *>((this->_expr)->codeGen(context)));
    llvm::Value * val = context.locals()[this->_id->_id];
    llvm::Value * offset = llvm::GetElementPtrInst::CreateInBounds(val, index, "tmp", context.topBB());
    if (val) {
        llvm::LoadInst * load = new llvm::LoadInst(offset, "tmp",context.topBB());
        return load;
    }
    return ErrorHandler("Variable Not Initilized");
}

Value *Identifier::codeGen(CodeGenContext& context) {
    AllocaInst * allocaInst = new AllocaInst(Type::getInt64Ty(getGlobalContext()), (this->_id), context.topBB());
    new StoreInst(ConstantInt::get(Type::getInt64Ty(getGlobalContext()), 0, true), allocaInst, false, context.topBB());
    context.locals()[this->_id] = allocaInst;
    if(context.locals().find(this->_id) == context.locals().end()) {
        // cout << this->_id;
        return ErrorHandler("Variable Identifier Not Declared");
    }   
    llvm::Value * val = context.locals()[this->_id];
    if (val)
        return new llvm::LoadInst(val, "tmp", context.topBB());
    return ErrorHandler("Variable Not Initilized");
}

Value *Statement::codeGen(CodeGenContext& context) {
    AssignStmt *asgn = dynamic_cast<AssignStmt *> (this);
    IfElseStmt * iest = dynamic_cast<IfElseStmt *> (this);
    ForStmt *fst = dynamic_cast<ForStmt *> (this);
    ReturnVoidStmt *rvd = dynamic_cast<ReturnVoidStmt *> (this);
    RetExpStmt *retst = dynamic_cast<RetExpStmt *> (this);
    BreakStmt *brk = dynamic_cast<BreakStmt *> (this);
    ContinueStmt *cnt = dynamic_cast<ContinueStmt *> (this);
    MethodCall *mc = dynamic_cast<MethodCall *> (this);
    if(asgn) {
        return asgn->codeGen(context);
    }
    if(iest) {
        return iest->codeGen(context);
    }
    if(fst) {
        return fst->codeGen(context);
    }
    if(rvd) {
        return rvd->codeGen(context);
    }
    if(brk) {
        return brk->codeGen(context);
    }
    if(retst) {
        return retst->codeGen(context);
    }
    if(cnt) {
        return cnt->codeGen(context);
    }
    if(mc) {
        return mc->codeGen(context);
    }
}

Value *AssignStmt::codeGen(CodeGenContext& context) {
    if(context.locals().find(this->_loc->_id) == context.locals().end())
    {
        cerr<<"Undeclared Variable in assignment statement"<<endl;
        return NULL;
    }
    Value *st = (this->_expr)->codeGen(context);

    if(st!=NULL){
        st = new StoreInst(st, context.locals()[_loc->_id], context.topBB());
    }
    return st;
}

// Value *IfElseStmt::codeGen(CodeGenContext& context) {
//     BasicBlock * entryBlock = context.topBB();
//     Value * condition = static_cast<Value *>(this->_expr->codeGen(context);
//     ICmpInst * comparison = new ICmpInst(*entryBlock, ICmpInst::ICMP_NE, condition, ConstantInt::get(Type::getInt64Ty(getGlobalContext()), 0, true), "tmp");
//     BasicBlock * ifBlock = BasicBlock::Create(getGlobalContext(), "ifBlock", entryBlock->getParent());
//     BasicBlock * mergeBlock = BasicBlock::Create(getGlobalContext(), "mergeBlock", entryBlock->getParent());

//     BasicBlock * returnedBlock = NULL;

//     context.pushBB(ifBlock);
//     this->_block1->codeGen(context);
//     returnedBlock = context.topBB();
//     if (!returnedBlock->getTerminator()) {
//         BranchInst::Create(mergeBlock, returnedBlock);
//     }
//     if (this->_block2) {
//         BasicBlock * elseBlock = BasicBlock::Create(getGlobalContext(), "elseBlock", entryBlock->getParent());

//         context.pushBB(elseBlock);
//         this->_block2->generateCode(context);
//         returnedBlock = context.topBB();
//         context.popBB();
//         if (!returnedBlock->getTerminator()) {
//             BranchInst::Create(mergeBlock, returnedBlock);
//         }
//         BranchInst::Create(ifBlock, elseBlock, comparison, entryBlock);
//     } else {
//         BranchInst::Create(ifBlock, mergeBlock, comparison, entryBlock);
//     }
//     auto localVariables = context.locals();
//     symbolTable.popBlock();
//     symbolTable.pushBlock(mergeBlock);
//     symbolTable.setLocalVariables(localVariables);
//     return NULL;
// }


Value *IfElseStmt::codeGen(CodeGenContext& context)
{
    Value *CondV = this->_expr->codeGen(context);
    if(CondV!=NULL)
    {
        CondV = Builder.CreateFCmpONE(CondV,ConstantFP::get(getGlobalContext(),APFloat(0.0)),"ifcond");
        Function *TheFunction = Builder.GetInsertBlock()->getParent();

        BasicBlock *IfBB = BasicBlock::Create(getGlobalContext(), "if", TheFunction);
        BasicBlock *ElseBB = BasicBlock::Create(getGlobalContext(), "else");
        BasicBlock *MergeBB = BasicBlock::Create(getGlobalContext(), "ifcont");


        Builder.CreateCondBr(CondV, IfBB, ElseBB);
        Builder.SetInsertPoint(IfBB);

        Value *IfV = this->_block1->codeGen(context);
        Builder.CreateBr(MergeBB);

        IfBB = Builder.GetInsertBlock();
        TheFunction->getBasicBlockList().push_back(ElseBB);
        Builder.SetInsertPoint(ElseBB);
        Value *ElseV = this->_block2->codeGen(context);
        if(ElseV == NULL)return NULL;
        Builder.CreateBr(MergeBB);
        ElseBB = Builder.GetInsertBlock();
        TheFunction->getBasicBlockList().push_back(MergeBB);
        Builder.SetInsertPoint(MergeBB);
        return NULL;
    }
    
    return NULL;
}
Value *IfStmt::codeGen(CodeGenContext& context) {return NULL;}
Value *BreakStmt::codeGen(CodeGenContext& context){return NULL;}
Value *ContinueStmt::codeGen(CodeGenContext& context){return NULL;}

Value *ForStmt::codeGen(CodeGenContext& context)
{
    cout << "lalalal" << endl;
    Value *st = this->_start->codeGen(context);

    if(st!=NULL)
        st = new StoreInst(st, context.locals()[_i->_id], context.topBB());
    
    if (!st) return NULL;

    Function *TheFunction = Builder.GetInsertBlock()->getParent();
    BasicBlock *PreheaderBB = Builder.GetInsertBlock();
    BasicBlock *LoopBB = BasicBlock::Create(getGlobalContext(), "loop", TheFunction);

    Builder.CreateBr(LoopBB);
    Builder.SetInsertPoint(LoopBB);

    Value *X = this->_block->codeGen(context);
    if (!X)
        return NULL;


    Value *EndCond = (this->_end)->codeGen(context);
    if (EndCond == NULL) return EndCond;

    EndCond = Builder.CreateFCmpONE(EndCond, 
            ConstantFP::get(getGlobalContext(), APFloat(0.0)),
            "loopcond");

    BasicBlock *LoopEndBB = Builder.GetInsertBlock();
    BasicBlock *AfterBB = BasicBlock::Create(getGlobalContext(), "afterloop", TheFunction);
    Builder.CreateCondBr(EndCond, LoopBB, AfterBB);
    Builder.SetInsertPoint(AfterBB);
    return NULL;
}

Value *ReturnVoidStmt::codeGen(CodeGenContext& context) {
    return ReturnInst::Create(getGlobalContext(),context.topBB());  
}

Value *RetExpStmt::codeGen(CodeGenContext& context) {
    Value *expr = this->_expr->codeGen(context);
    return ReturnInst::Create(getGlobalContext(),expr,context.topBB()); 
}


Value *MethodCall::codeGen(CodeGenContext& context) {
    MethodCall1 * normal = dynamic_cast<MethodCall1 *> (this);
    MethodCall2 * callout = dynamic_cast<MethodCall2 *> (this);
    if(normal)
        return normal->codeGen(context);
    if(callout)
        return callout->codeGen(context);
    return NULL;
}

Value *MethodCall1::codeGen(CodeGenContext& context) {
    Function *function = context.module->getFunction(this->_method_name->_id);
    if(function==NULL)
    {
        cerr<<"function not declared"<<endl;
        return NULL;
    }
    vector<Value *> args; 
    for(int i=0;i<(this->_exprs)->size();i++)
    {
        args.push_back((*_exprs)[i]->codeGen(context));
    }

    CallInst *call = NULL;
    call = CallInst::Create(function,makeArrayRef(args),this->_method_name->_id,context.topBB());
    return call;
}

Value *Callarg_method::codeGen(CodeGenContext& context) {
    Expression *ex = dynamic_cast<Expression *> (this);
    StringLit *str = dynamic_cast<StringLit *> (this);
    if(str) {
        return str->codeGen(context);
    }
    if(ex) {
        return ex->codeGen(context);
    }
}

Value *MethodCall2::codeGen(CodeGenContext& context) {
    llvm::Function * function = context.module->getFunction(this->_method_name->_str);
    if (!function) {
        llvm::FunctionType *ftype = llvm::FunctionType::get(llvm::Type::getInt64Ty(llvm::getGlobalContext()), true);
        function = llvm::Function::Create(ftype, llvm::GlobalValue::ExternalLinkage, this->_method_name->_str, context.module);
    }
    std::vector<llvm::Value *> args;
    for(int i=0;i<(this->_exprs)->size();i++) {
        args.push_back(static_cast<llvm::Value *>((*_exprs)[i]->codeGen(context)));
    }
    llvm::CallInst *call = llvm::CallInst::Create(function, llvm::makeArrayRef(args), this->_method_name->_str, context.topBB());
    return call;
}


Value *StringLit::codeGen(CodeGenContext& context) {
    std::string argument = this->_str;
    llvm::GlobalVariable* variable = new llvm::GlobalVariable(*context.module, llvm::ArrayType::get(llvm::IntegerType::get(llvm::getGlobalContext(), 8), argument.size() + 1), true, llvm::GlobalValue::InternalLinkage, NULL, "string");
    variable->setInitializer(llvm::ConstantDataArray::getString(llvm::getGlobalContext(), argument, true));
    return variable;
}