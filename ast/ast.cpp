#include "ast.hpp"
#include <iostream>
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <type_traits>
#include <string>
#include <map>
#include <tuple>
#include <vector>
#include <iterator>
#include <utility>

const int VAR_STEP = 4;

int get_size(int n) {
    int i = 16;    
    while (i < n) i += 16;
    return i;
}

void num_of_scans(ASTNode* ptr, int* num) {
    auto* num_node = dynamic_cast<NumNode*>(ptr);
    auto* var_node = dynamic_cast<VarNode*>(ptr);
    auto* arrElem_node = dynamic_cast<ArrayElemNode*>(ptr);
    auto* bin_op_node = dynamic_cast<BinaryNode*>(ptr);
    auto* main_node = dynamic_cast<MainNode*>(ptr);
    auto* assign_node = dynamic_cast<AssignNode*>(ptr);
    auto* print_node = dynamic_cast<PrintNode*>(ptr);
    auto* scan_node = dynamic_cast<ScanNode*>(ptr);
    auto* statArrDecl_node = dynamic_cast<StatArrayDeclNode*>(ptr);
    auto* dynArrDecl_node = dynamic_cast<DynArrayDeclNode*>(ptr);
    auto* arrElemAssign_node = dynamic_cast<ArrayElemAssignNode*>(ptr);
    auto* if_else_node = dynamic_cast<IfElseNode*>(ptr);
    auto* while_node = dynamic_cast<WhileNode*>(ptr);
    auto* funcDef_node = dynamic_cast<FuncDef*>(ptr);
    auto* funcCall_node = dynamic_cast<FuncCall*>(ptr);
    
    if (num_node) { return; }
    else if (var_node) { return; }
    else if (arrElem_node) { return; }
    else if (bin_op_node) {
        num_of_scans(bin_op_node->left, num);
        num_of_scans(bin_op_node->right, num);
    }
    else if (main_node) {
        num_of_scans(main_node->next, num);
    }
    else if (assign_node) {
        num_of_scans(assign_node->next, num);
    }
    else if (arrElemAssign_node) {
        num_of_scans(arrElemAssign_node->next, num);
    }
    else if (print_node) {
        num_of_scans(print_node->next, num);
    }
    else if (scan_node) {
        *num += 1;
        num_of_scans(scan_node->next, num);
    }
    else if (statArrDecl_node) {
        num_of_scans(statArrDecl_node->next, num);
    }
    else if (dynArrDecl_node) {
        num_of_scans(dynArrDecl_node->next, num);
    }
    else if (if_else_node) {
        int n = if_else_node->conds.size();
        
        if (n == 1) {
            num_of_scans(if_else_node->conds[0].second, num);
            num_of_scans(if_else_node->next, num);
        }
        else {
            for (int i = 0; i < n; ++i) {
                num_of_scans(if_else_node->conds[i].second, num);
            }
            
            num_of_scans(if_else_node->next, num);
        }
    }
    else if (while_node) {
        num_of_scans(while_node->stmts, num);
        num_of_scans(while_node->next, num); 
    }
    else if (funcDef_node) { num_of_scans(funcDef_node->next, num); }
    else if (funcCall_node) { return; }
    else { return; }
};

int num_vars(ASTNode* ptr, int res) {
    auto* num_node = dynamic_cast<NumNode*>(ptr);
    auto* var_node = dynamic_cast<VarNode*>(ptr);
    auto* bin_op_node = dynamic_cast<BinaryNode*>(ptr);
    
    if (num_node) { res = 0; }
    else if (var_node) { res = 1; }
    else if (bin_op_node) {
        res = num_vars(bin_op_node->left, res) + num_vars(bin_op_node->right, res);
    }
    else { return 0; }
    
    return res;
}

int num_expr_eval(ASTNode* ptr, int res) {
    auto* num_node = dynamic_cast<NumNode*>(ptr);
    auto* bin_op_node = dynamic_cast<BinaryNode*>(ptr);
    
    if (num_node) { res = num_node->num; }
    else if (bin_op_node) {
        switch (bin_op_node->tag) {
            case _ADD_:
                res = num_expr_eval(bin_op_node->left, res) + num_expr_eval(bin_op_node->right, res);
                break;
            case _SUB_:
                res = num_expr_eval(bin_op_node->left, res) - num_expr_eval(bin_op_node->right, res);
                break;
            case _MUL_:
                res = num_expr_eval(bin_op_node->left, res) * num_expr_eval(bin_op_node->right, res);
                break;
            case _DIV_:
                res = num_expr_eval(bin_op_node->left, res) / num_expr_eval(bin_op_node->right, res);
                break;
            case _MOD_:
                res = num_expr_eval(bin_op_node->left, res) % num_expr_eval(bin_op_node->right, res);
                break;
            case _SHL_:
                res = num_expr_eval(bin_op_node->left, res) << num_expr_eval(bin_op_node->right, res);
                break;
            case _SHR_:
                res = num_expr_eval(bin_op_node->left, res) % num_expr_eval(bin_op_node->right, res);
                break;
            case _LESS_:
                res = num_expr_eval(bin_op_node->left, res) < num_expr_eval(bin_op_node->right, res);
                break;
            case _GREAT_:
                res = num_expr_eval(bin_op_node->left, res) > num_expr_eval(bin_op_node->right, res);
                break;
            case _EQ_:
                res = num_expr_eval(bin_op_node->left, res) == num_expr_eval(bin_op_node->right, res);
                break;
            case _NEQ_:
                res = num_expr_eval(bin_op_node->left, res) != num_expr_eval(bin_op_node->right, res);
                break;
            case _GEQ_:
                res = num_expr_eval(bin_op_node->left, res) >= num_expr_eval(bin_op_node->right, res);
                break;
            case _LEQ_:
                res = num_expr_eval(bin_op_node->left, res) <= num_expr_eval(bin_op_node->right, res);
                break;
            case _AND_:
                res = num_expr_eval(bin_op_node->left, res) && num_expr_eval(bin_op_node->right, res);
                break;
            case _OR_:
                res = num_expr_eval(bin_op_node->left, res) || num_expr_eval(bin_op_node->right, res);
                break;
            case _NOT_:
                res = !num_expr_eval(bin_op_node->right, res);
                break;
            case _NEG_:
                res = -num_expr_eval(bin_op_node->right, res);
                break;
        }
    }
    
    return res;
}

bool errResult(Result* tmp) {
    switch (tmp->err) {
        case ErrType::_ERR_VAR_ : {
            return true; 
            break;
        }
        case ErrType::_ERR_ARR_ : {
            return true; 
            break;
        }
        case ErrType::_ERR_FUNC_EXIST_ : {
            return true; 
            break;
        }
        case ErrType::_ERR_CONST_ : {
            return true; 
            break;
        }
        default : {
            return false;
            break;
        }
    };
};

NumNode::NumNode(int _line_index, int _num) { line_index = _line_index; num = _num; };

VarNode::VarNode(int _line_index, std::string _var_name) { 
    line_index = _line_index; 
    var_name = _var_name; 
};

ArrayElemNode::ArrayElemNode(int _line_index, std::string _arr_name, ASTNode* _elem_index) {
    line_index = _line_index;
    arr_name = _arr_name;
    elem_index = _elem_index;
};

BinaryNode::BinaryNode(int _line_index, Tag _tag, ASTNode* _left, ASTNode* _right) {
    line_index = _line_index;
    tag = _tag;
    left = _left;
    right = _right;
};

FuncCall::FuncCall(int _line_index, std::string _func_name, std::vector<ASTNode*> _func_args) {
    line_index = _line_index;
    func_name = _func_name;
    func_args = _func_args;
};

MainNode::MainNode(int _line_index, ASTNode* _next) {
    line_index = _line_index;
    next = _next;
};

AssignNode::AssignNode(int _line_index, std::string _var_name, VarType _assign_ty, 
    ASTNode* _assign_val, ASTNode* _next) {
    line_index = _line_index;
    var_name = _var_name;
    assign_ty = _assign_ty;
    assign_val = _assign_val;
    next = _next;
};

StatArrayDeclNode::StatArrayDeclNode(int _line_index, std::string _arr_name, int _arr_size, 
    std::vector<ASTNode*> _arr_vals, ASTNode* _next) {
    line_index = _line_index;
    arr_name = _arr_name;
    arr_size = _arr_size;
    arr_vals = _arr_vals;
    next = _next;
};

DynArrayDeclNode::DynArrayDeclNode(int _line_index, int _arrayDecl_loop, std::string _arr_name, ASTNode* _arr_size, 
    ASTNode* _arr_val, ASTNode* _next) {
    line_index = _line_index;
    arrayDecl_loop = _arrayDecl_loop;
    arr_name = _arr_name;
    arr_size = _arr_size;
    arr_val = _arr_val;
    next = _next;
}

ArrayElemAssignNode::ArrayElemAssignNode(int _line_index, std::string _arr_name, ASTNode* _elem_index, 
    ASTNode* _assign_val, ASTNode* _next) {
    line_index = _line_index;
    arr_name = _arr_name;
    elem_index = _elem_index;
    assign_val = _assign_val;
    next = _next;
};

PrintNode::PrintNode(int _line_index, ASTNode* _print_val, ASTNode* _next) {
    line_index = _line_index;
    print_val = _print_val;
    next = _next;
};

ScanNode::ScanNode(int _line_index, std::string _var_name, ASTNode* _next) {
    line_index = _line_index;
    var_name = _var_name;
    next = _next;
};

IfElseNode::IfElseNode(int _line_index, int _if_num, int _main_num, std::vector<std::pair<ASTNode*, ASTNode*>> _conds, std::vector<int> _cond_num, ASTNode* _next) {
    line_index = _line_index;
    if_num = _if_num;
    main_num = _main_num;
    conds = _conds;
    cond_num = _cond_num;
    next = _next;
};

WhileNode::WhileNode(int _line_index, int _while_num, int _main_num, ASTNode* _cond, ASTNode* _stmts, ASTNode* _next) {
    line_index = _line_index;
    while_num = _while_num;
    main_num = _main_num;
    cond = _cond; 
    stmts = _stmts; 
    next = _next;
};

ReturnNode::ReturnNode(int _line_index, ASTNode* _return_val, ASTNode* _next) {
    line_index = _line_index;
    return_val = _return_val;
    next = _next;
};

FuncDef::FuncDef(int _line_index, std::string _func_name, std::vector<ASTNode*> _func_args, FuncState _func_state, 
        ASTNode* _func_stmts, ASTNode* _next) {
    line_index = _line_index;
    func_name = _func_name;
    func_args = _func_args;
    func_state = _func_state;
    func_stmts = _func_stmts;
    next = _next;
};

Result* FuncDef::traverse_func_tree(ASTNode* ptr, ProgState state) {
    auto* num_node = dynamic_cast<NumNode*>(ptr);
    auto* var_node = dynamic_cast<VarNode*>(ptr);
    auto* arrElem_node = dynamic_cast<ArrayElemNode*>(ptr);
    auto* bin_op_node = dynamic_cast<BinaryNode*>(ptr);
    auto* main_node = dynamic_cast<MainNode*>(ptr);
    auto* assign_node = dynamic_cast<AssignNode*>(ptr);
    auto* arrElemAssign_node = dynamic_cast<ArrayElemAssignNode*>(ptr);
    auto* statArrDecl_node = dynamic_cast<StatArrayDeclNode*>(ptr);
    auto* dynArrDecl_node = dynamic_cast<DynArrayDeclNode*>(ptr);
    auto* if_else_node = dynamic_cast<IfElseNode*>(ptr);
    auto* while_node = dynamic_cast<WhileNode*>(ptr);
    auto* return_node = dynamic_cast<ReturnNode*>(ptr);
    auto* funcCall_node = dynamic_cast<FuncCall*>(ptr);
    
    if (num_node) { 
        return new Result(ErrType::_OK_, -1, "") ; 
    }
    else if (var_node) {
        if (this->func_state.vars.find(var_node->var_name) == this->func_state.vars.end()) {
            return new Result(ErrType::_ERR_VAR_, var_node->line_index, "Variable '" + var_node->var_name + "' not defined!");
        }
    }
    else if (arrElem_node) {
        if (this->func_state.arrs.find(arrElem_node->arr_name) == this->func_state.arrs.end()) {
            std::string arr = arrElem_node->arr_name.substr(1, arrElem_node->arr_name.length()-2);
            return new Result(ErrType::_ERR_ARR_, arrElem_node->line_index, "Unknown array '" + arr + "'!");
        }
        else {
            Result* index = traverse_func_tree(arrElem_node->elem_index, state);
            if (errResult(index)) return index;
        }
    }
    else if (bin_op_node) {
        Result* lhs = traverse_func_tree(bin_op_node->left, state);
        Result* rhs = traverse_func_tree(bin_op_node->right, state);
        
        if (errResult(lhs)) return lhs;
        else if (errResult(rhs)) return rhs;
    }
    else if (main_node) {
        Result* res = traverse_func_tree(main_node->next, state);
        if (errResult(res)) return res;
    }
    else if (assign_node) {
        Result* val = traverse_func_tree(assign_node->assign_val, state);
        if (errResult(val)) return val;
        
        auto it = this->func_state.vars.find(assign_node->var_name);
        switch (assign_node->assign_ty) {
            case VarType::_VAR_ : {
                if (it == this->func_state.vars.end()) {
                    this->func_state.vars[assign_node->var_name] = this->func_state.var_counter;
                    this->func_state.var_counter += VAR_STEP;
                } 
                break;
            }
            case VarType::_CONST_ : {
                if (it == this->func_state.vars.end()) {
                    this->func_state.vars[assign_node->var_name] = this->func_state.var_counter;
                    this->func_state.var_counter += VAR_STEP;
                }
                else {
                    return new Result(ErrType::_ERR_CONST_, assign_node->line_index, "Can't redefine const '" + assign_node->var_name + "'!!");
                }
                break;
            }
            default : {break;}
        }
        Result* res = traverse_func_tree(assign_node->next, state);
        if (errResult(res)) return res;
    }
    else if (arrElemAssign_node) {
        if (this->func_state.arrs.find(arrElemAssign_node->arr_name) == this->func_state.arrs.end()) {
            std::string arr = arrElemAssign_node->arr_name.substr(1, arrElemAssign_node->arr_name.length()-2);
            return new Result(ErrType::_ERR_ARR_, arrElemAssign_node->line_index, "Unknown array '" + arr + "'!");
        }
        else {
            Result* index = traverse_func_tree(arrElemAssign_node->elem_index, state);
            Result* val = traverse_func_tree(arrElemAssign_node->assign_val, state);
            Result* res = traverse_func_tree(arrElemAssign_node->next, state);
            
            if (errResult(index)) return index;
            else if (errResult(val)) return val;
            else if (errResult(res)) return res;
        }
    }
    else if (statArrDecl_node) { //cnt = 4, n = 3 ==> {8, 12, 16}
        auto it = this->func_state.arrs.find(statArrDecl_node->arr_name);
        if (it == this->func_state.arrs.end()) { 
            this->func_state.arrs[statArrDecl_node->arr_name] = {
                this->func_state.var_counter + statArrDecl_node->arr_size*4, 
                _STAT_
            }; 
        }
        
        int n = statArrDecl_node->arr_size;
        for (int i = n-1; i >= 0; --i) {
            Result* val = traverse_func_tree(statArrDecl_node->arr_vals[i], state);
            
            if (errResult(val)) return val;
            
            std::string elem_name = statArrDecl_node->arr_name + std::to_string(i);
            
            this->func_state.vars[elem_name] = this->func_state.var_counter;
            this->func_state.var_counter += VAR_STEP;
        }
        
        Result* res = traverse_func_tree(statArrDecl_node->next, state);
        
        if (errResult(res)) return res;
    }
    else if (dynArrDecl_node) {
        dynArrDecl_node->arrayDecl_loop = this->func_state.arrayDecl_loop;
        this->func_state.arrayDecl_loop += 1;
        
        auto it = this->func_state.arrs.find(dynArrDecl_node->arr_name);
        if (it == this->func_state.arrs.end()) { this->func_state.arrs[dynArrDecl_node->arr_name] = {this->func_state.var_counter, _DYN_};}
        this->func_state.var_counter += VAR_STEP;
        
        Result* size = traverse_func_tree(dynArrDecl_node->arr_size, state);
        Result* val = traverse_func_tree(dynArrDecl_node->arr_val, state);
        Result* res = traverse_func_tree(dynArrDecl_node->next, state);
        
        if (errResult(size)) return size;
        else if (errResult(val)) return val;
        else if (errResult(res)) return res;
    }
    else if (if_else_node) {
        int n = if_else_node->conds.size();
        
        if (n == 1) {
            if_else_node->if_num = this->func_state.if_counter;
            if_else_node->main_num = this->func_state.main_counter;
            if_else_node->cond_num.push_back(this->func_state.cond_counter);
            
            this->func_state.if_counter += 1;
            this->func_state.main_counter += 1;
            this->func_state.cond_counter += 1;
            
            Result* cond = traverse_func_tree(if_else_node->conds[0].first, state);
            Result* stmts = traverse_func_tree(if_else_node->conds[0].second, state);
            Result* res = traverse_func_tree(if_else_node->next, state);
            
            if (errResult(cond)) return cond;
            else if (errResult(stmts)) return stmts;
            else if (errResult(res)) return res;
        }
        else {
            if_else_node->if_num = this->func_state.if_counter;
            if_else_node->main_num = this->func_state.main_counter;
            for (int i = 1; i < n; ++i) {
                if_else_node->cond_num.push_back(this->func_state.cond_counter + i - 1);
                Result* cond = traverse_func_tree(if_else_node->conds[i].first, state);
                
                if (errResult(cond)) return cond;
            }
            if_else_node->cond_num.push_back(this->func_state.cond_counter + n - 1);
            this->func_state.cond_counter += n;
            this->func_state.main_counter += n;
            
            for (int i = 1; i < n; ++i) {
                this->func_state.if_counter += 1;
                Result* stmts = traverse_func_tree(if_else_node->conds[i].second, state);

                if (errResult(stmts)) return stmts;
            }
            Result* stmts = traverse_func_tree(if_else_node->conds[0].second, state);
            Result* res = traverse_func_tree(if_else_node->next, state);
            
            if (errResult(stmts)) return stmts;
            else if (errResult(res)) return res;
        }
    }
    else if (while_node) {
        while_node->while_num = this->func_state.loop_counter;
        while_node->main_num = this->func_state.main_counter;
        this->func_state.loop_counter += 1;    
        this->func_state.main_counter += 1;
        
        Result* cond = traverse_func_tree(while_node->cond, state);
        Result* stmts = traverse_func_tree(while_node->stmts, state);
        Result* res = traverse_func_tree(while_node->next, state);
        
        if (errResult(cond)) return cond;
        else if (errResult(stmts)) return stmts;
        else if (errResult(res)) return res;
    }
    else if (return_node) {
        Result* val = traverse_func_tree(return_node->return_val, state);
        Result* res = traverse_func_tree(return_node->next, state);
        
        if (errResult(val)) return val;
        else if (errResult(res)) return res;
    }
    else if (funcCall_node) {
        if (state.funcs.find(funcCall_node->func_name) == state.funcs.end()) {
            return new Result(ErrType::_ERR_FUNC_EXIST_, funcCall_node->line_index, "Function '" + funcCall_node->func_name + "' not defined!"); 
        }
        
        for (auto it : funcCall_node->func_args) {
            Result* tmp = traverse_func_tree(it, state);
            if (errResult(tmp)) return tmp;
        }
    }
    
    return new Result(ErrType::_OK_, -1, "");
};

void FuncDef::print_func_asm(ASTNode* ptr) {
    auto* num_node = dynamic_cast<NumNode*>(ptr);
    auto* var_node = dynamic_cast<VarNode*>(ptr);
    auto* arrElem_node = dynamic_cast<ArrayElemNode*>(ptr);
    auto* bin_op_node = dynamic_cast<BinaryNode*>(ptr);
    auto* main_node = dynamic_cast<MainNode*>(ptr);
    auto* assign_node = dynamic_cast<AssignNode*>(ptr);
    auto* arrElemAssign_node = dynamic_cast<ArrayElemAssignNode*>(ptr);
    auto* print_node = dynamic_cast<PrintNode*>(ptr);
    auto* statArrDecl_node = dynamic_cast<StatArrayDeclNode*>(ptr);
    auto* dynArrDecl_node = dynamic_cast<DynArrayDeclNode*>(ptr);
    auto* if_else_node = dynamic_cast<IfElseNode*>(ptr);
    auto* while_node = dynamic_cast<WhileNode*>(ptr);
    auto* funcCall_node = dynamic_cast<FuncCall*>(ptr);
    auto* return_node = dynamic_cast<ReturnNode*>(ptr);
    
    if (num_node) {
        std::cout << "  mov rax, " << num_node->num << std::endl;
    }
    else if (var_node) {
        std::cout << "  mov rax, QWORD PTR [rbp-" << 2 * this->func_state.vars[var_node->var_name] << "]" << std::endl;
    }
    else if (arrElem_node) {
        auto it = this->func_state.arrs.find(arrElem_node->arr_name);
        if (it != this->func_state.arrs.end()) {
            ArrayType ty = it->second.second;
            if (ty == ArrayType::_STAT_) {
                std::cout << "  lea rdi, [rbp-" << 2*this->func_state.arrs[arrElem_node->arr_name].first-8 << "]" << std::endl;
                print_func_asm(arrElem_node->elem_index);
                std::cout << "  mov rsi, rax" << std::endl;
                std::cout << "  call get" << std::endl;
            }
            else if (ty == ArrayType::_DYN_) {
                std::cout << "  mov rdi, QWORD PTR [rbp-" << 2*this->func_state.arrs[arrElem_node->arr_name].first << "]" << std::endl;
                print_func_asm(arrElem_node->elem_index);
                std::cout << "  mov rsi, rax" << std::endl;
                std::cout << "  call get" << std::endl;
            }
        }
    }
    else if (funcCall_node) {
        std::vector<std::string> asm_args = {"rdi", "rsi", "rdx", "rcx"};
        std::vector<int> call_registers;
        std::reverse(funcCall_node->func_args.begin(), funcCall_node->func_args.end());
        int n = (int)funcCall_node->func_args.size();
        for (int i = 0; i < n; ++i) {
            print_func_asm(funcCall_node->func_args[i]);
            
            auto* it = dynamic_cast<FuncCall*>(funcCall_node->func_args[i]);
            
            if (it) {
                call_registers.push_back(i);
                std::cout << "  push rax" << std::endl;
            }
            else {
                std::cout << "  mov " << asm_args[i] << ", rax" << std::endl;
            }
        }
        std::reverse(call_registers.begin(), call_registers.end());
        int r = (int)call_registers.size();
        for (int i = 0; i < r; ++i) {
            std::cout << "  pop " << asm_args[call_registers[i]] << std::endl;
        }
        std::cout << "  call " + funcCall_node->func_name << std::endl;  
    }
    else if (bin_op_node) {
        switch (bin_op_node->tag) {
            case _ADD_ : {
                print_func_asm(bin_op_node->left);
                std::cout << "  push rax" << std::endl;
                print_func_asm(bin_op_node->right);
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  add rax, rbx" << std::endl;
                break;
            }
            case _SUB_ : {
                print_func_asm(bin_op_node->left);
                std::cout << "  push rax" << std::endl;
                print_func_asm(bin_op_node->right);
                std::cout << "  push rax" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  pop rax" << std::endl;
                std::cout << "  sub rax, rbx" << std::endl;
                break;
            }
            case _MUL_ : {
                print_func_asm(bin_op_node->left);
                std::cout << "  push rax" << std::endl;
                print_func_asm(bin_op_node->right);
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  imul rax, rbx" << std::endl;
                break;
            }
            case _DIV_ : {
                print_func_asm(bin_op_node->left);
                std::cout << "  push rax" << std::endl;
                print_func_asm(bin_op_node->right);
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  push rax" << std::endl;
                std::cout << "  mov rax, rbx" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  cqo" << std::endl;
                std::cout << "  xor rdx, rdx" << std::endl;
                std::cout << "  idiv rbx" << std::endl;
                break;
            }
            case _MOD_ : {
                print_func_asm(bin_op_node->left);
                std::cout << "  push rax" << std::endl;
                print_func_asm(bin_op_node->right);
                
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  push rax" << std::endl;
                std::cout << "  mov rax, rbx" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  cqo" << std::endl;
                std::cout << "  xor rdx, rdx" << std::endl;
                std::cout << "  idiv rbx" << std::endl;
                std::cout << "  mov rax, rdx" << std::endl;
                break;
            }
            case _SHL_ : {
                print_func_asm(bin_op_node->left);
                std::cout << "  push rax" << std::endl;
                print_func_asm(bin_op_node->right);
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  push rax" << std::endl;
                std::cout << "  mov rax, rbx" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  mov rdi, rax" << std::endl;
                std::cout << "  mov rsi, rbx" << std::endl;
                std::cout << "  call shlf" << std::endl;
                break;
            }
            case _SHR_ : {
                print_func_asm(bin_op_node->left);
                std::cout << "  push rax" << std::endl;
                print_func_asm(bin_op_node->right);
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  push rax" << std::endl;
                std::cout << "  mov rax, rbx" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  mov rdi, rax" << std::endl;
                std::cout << "  mov rsi, rbx" << std::endl;
                std::cout << "  call shrf" << std::endl;
                break;
            }
            case _AND_ : {
                print_func_asm(bin_op_node->left);
                std::cout << "  push rax" << std::endl;
                print_func_asm(bin_op_node->right);
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  and rax, rbx" << std::endl;
                break;
            }
            case _OR_ : {
                print_func_asm(bin_op_node->left);
                std::cout << "  push rax" << std::endl;
                print_func_asm(bin_op_node->right);
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  or rax, rbx" << std::endl;
                break;
            }
            case _NOT_ : {
                print_func_asm(bin_op_node->right);
                std::cout << "  not rax" << std::endl;
                break;
            }
            case _LESS_ : {
                print_func_asm(bin_op_node->left);
                std::cout << "  push rax" << std::endl;
                print_func_asm(bin_op_node->right);
                std::cout << "  push rax" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  pop rax" << std::endl;
                std::cout << "  mov rdi, rax" << std::endl;
                std::cout << "  mov rsi, rbx" << std::endl;
                std::cout << "  call cmp_less" << std::endl;  
                break;
            }
            case _GREAT_ : {
                print_func_asm(bin_op_node->left);
                std::cout << "  push rax" << std::endl;
                print_func_asm(bin_op_node->right);
                std::cout << "  push rax" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  pop rax" << std::endl;
                std::cout << "  mov rdi, rax" << std::endl;
                std::cout << "  mov rsi, rbx" << std::endl;
                std::cout << "  call cmp_great" << std::endl; 
                break;
            }
            case _EQ_ : {
                print_func_asm(bin_op_node->left);
                std::cout << "  push rax" << std::endl;
                print_func_asm(bin_op_node->right);
                std::cout << "  push rax" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  pop rax" << std::endl;
                std::cout << "  mov rdi, rax" << std::endl;
                std::cout << "  mov rsi, rbx" << std::endl;
                std::cout << "  call cmp_eq" << std::endl; 
                break;
            }
            case _NEQ_ : {
                print_func_asm(bin_op_node->left);
                std::cout << "  push rax" << std::endl;
                print_func_asm(bin_op_node->right);
                std::cout << "  push rax" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  pop rax" << std::endl;
                std::cout << "  mov rdi, rax" << std::endl;
                std::cout << "  mov rsi, rbx" << std::endl;
                std::cout << "  call cmp_neq" << std::endl; 
                break;
            }
            case _LEQ_ : {
                print_func_asm(bin_op_node->left);
                std::cout << "  push rax" << std::endl;
                print_func_asm(bin_op_node->right);
                std::cout << "  push rax" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  pop rax" << std::endl;
                std::cout << "  mov rdi, rax" << std::endl;
                std::cout << "  mov rsi, rbx" << std::endl;
                std::cout << "  call cmp_leq" << std::endl; 
                break;
            }
            case _GEQ_ : {
                print_func_asm(bin_op_node->left);
                std::cout << "  push rax" << std::endl;
                print_func_asm(bin_op_node->right);
                std::cout << "  push rax" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  pop rax" << std::endl;
                std::cout << "  mov rdi, rax" << std::endl;
                std::cout << "  mov rsi, rbx" << std::endl;
                std::cout << "  call cmp_geq" << std::endl; 
                break;
            }
            case _NEG_ : {
                print_func_asm(bin_op_node->right);
                std::cout << "  mov r8, -1" << std::endl;
                std::cout << "  mul r8" << std::endl;
                break;
            }
            default: break;
        }
    }
    else if (main_node) {
        std::cout << this->func_name + ":" << std::endl;
        std::cout << "  push rbp" << std::endl;
        std::cout << "  mov rbp, rsp" << std::endl;
        std::cout << "  sub rsp, " << 8*(this->func_state.vars.size() + this->func_state.arrs.size()) << std::endl; 
        
        for (int i = 0; i < (int)this->func_args.size(); ++i) {
            VarNode* jt = dynamic_cast<VarNode*>(this->func_args[i]);
            std::cout << "  mov QWORD PTR [rbp-" << 2 * this->func_state.vars[jt->var_name]  << "], " << this->asm_args[i] << std::endl;
        }
        
        print_func_asm(main_node->next);
        std::cout << "  leave" << std::endl;
        std::cout << "  ret\n" << std::endl;
    }
    else if (assign_node) {
        print_func_asm(assign_node->assign_val);
        std::cout << "  mov QWORD PTR [rbp-" << 2 * this->func_state.vars[assign_node->var_name] << "], rax" << std::endl;
        print_func_asm(assign_node->next);
    }
    else if (arrElemAssign_node) {
        auto it = this->func_state.arrs.find(arrElemAssign_node->arr_name);
        if (it != this->func_state.arrs.end()) { 
            ArrayType ty = it->second.second;
            if (ty == ArrayType::_STAT_) {
                std::cout << "  lea rdi, [rbp-" << 2*this->func_state.arrs[arrElemAssign_node->arr_name].first << "]" << std::endl;
                print_func_asm(arrElemAssign_node->elem_index);
                std::cout << "  mov rsi, rax" << std::endl;
                print_func_asm(arrElemAssign_node->assign_val);
                std::cout << "  mov rdx, rax" << std::endl;
                std::cout << "  call set" << std::endl;
            }
            else if (ty == ArrayType::_DYN_) {
                std::cout << "  mov rdi, QWORD PTR [rbp-" << 2*this->func_state.arrs[arrElemAssign_node->arr_name].first << "]" << std::endl;
                print_func_asm(arrElemAssign_node->elem_index);
                std::cout << "  mov rsi, rax" << std::endl;
                print_func_asm(arrElemAssign_node->assign_val);
                std::cout << "  mov rdx, rax" << std::endl;
                std::cout << "  call set" << std::endl;
            }
            else { return; }
        }
        
        print_func_asm(arrElemAssign_node->next);
    }
    else if (print_node) {
        print_func_asm(print_node->print_val);
        std::cout << "  lea rdi, print_format" << std::endl;
        std::cout << "  mov rsi, rax" << std::endl;
        std::cout << "  xor rax, rax" << std::endl;
        std::cout << "  call printf" << std::endl;
        print_func_asm(print_node->next);
    }
    else if (statArrDecl_node) {
        for (int i = 0; i < statArrDecl_node->arr_size; ++i) {
            print_func_asm(statArrDecl_node->arr_vals[i]);
            
            std::string elem_name = statArrDecl_node->arr_name + std::to_string(i);
            std::cout << "  mov QWORD PTR [rbp-" << 2 * this->func_state.vars[elem_name] << "], rax" << std::endl;
        }
        
        print_func_asm(statArrDecl_node->next);
    }
    else if (dynArrDecl_node) {
        int num = num_vars(dynArrDecl_node->arr_size, 0);
        if (num == 0) {
            int res = num_expr_eval(dynArrDecl_node->arr_size, 0);
            std::cout << "  mov rax, " << std::to_string(res) << std::endl;
        }
        else {
            print_func_asm(dynArrDecl_node->arr_size);
        }
        std::cout << "  mov rdi, rax" << std::endl;
        print_func_asm(dynArrDecl_node->arr_val);
        std::cout << "  mov rsi, rax" << std::endl;
        std::cout << "  call dyn_malloc" << std::endl;
        
        std::cout << "  mov QWORD PTR [rbp-" << 2*this->func_state.arrs[dynArrDecl_node->arr_name].first << "], rax" << std::endl;
            
        print_func_asm(dynArrDecl_node->next);
    }
    else if (if_else_node) {
        auto* next_if_else = dynamic_cast<IfElseNode*>(if_else_node->next);
        auto* next_while = dynamic_cast<WhileNode*>(if_else_node->next);
        int n = if_else_node->conds.size();
        
        if (n == 1) {
            std::cout << this->func_name << "_if" << if_else_node->if_num << ":" << std::endl;
            print_func_asm(if_else_node->conds[0].first);
            std::cout << "  cmp rax, 1" << std::endl;
            std::cout << "  je " << this->func_name << "_cond" << if_else_node->cond_num[0] << std::endl;
            
            if (next_if_else) {
                std::cout << "  jmp " << this->func_name << "_if" << next_if_else->if_num << std::endl;
            }
            else if (next_while) {
                std::cout << "  jmp " << this->func_name << "_loop" << next_while->while_num << std::endl;
            }
            else {
                std::cout << "  jmp " << this->func_name << "_main" << if_else_node->main_num << std::endl;
            }
            
            std::cout << this->func_name << "_cond" << if_else_node->cond_num[0] << ":" << std::endl;
            print_func_asm(if_else_node->conds[0].second);
                
            if (next_if_else) {
                std::cout << "  jmp " << this->func_name << "_if" << next_if_else->if_num << std::endl; 
            }
            else if (next_while) {
                std::cout << "  jmp " << this->func_name << "_loop" << next_while->while_num << std::endl;
            }
            else {
                std::cout << "  jmp " << this->func_name << "_main" << if_else_node->main_num << std::endl;
            }
            
            if (!(next_if_else || next_while)) {
                std::cout << this->func_name << "_main" << if_else_node->main_num << ":" << std::endl;
            }
            
            print_func_asm(if_else_node->next);
        }
        else {
            std::cout << this->func_name << "_if" << if_else_node->if_num << ":" << std::endl;
            int i;
            for (i = 1; i < n; ++i) {
                print_func_asm(if_else_node->conds[i].first);
                std::cout << "  cmp rax, 1" << std::endl;
                std::cout << "  je " << this->func_name << "_cond" << if_else_node->cond_num[i-1] << std::endl;
            }
            std::cout << "  jmp " << this->func_name << "_cond" << if_else_node->cond_num[i-1] << std::endl;
            
            for (i = 1; i < n; ++i) {
                std::cout << this->func_name << "_cond" << if_else_node->cond_num[i-1] << ":" << std::endl;
                print_func_asm(if_else_node->conds[i].second);
                
                if (next_if_else) {
                    std::cout << "  jmp " << this->func_name << "_if" << next_if_else->if_num << std::endl; 
                }
                else if (next_while) {
                    std::cout << "  jmp " << this->func_name << "_loop" << next_while->while_num << std::endl;
                }
                else {
                    std::cout << "  jmp " << this->func_name << "_main" << if_else_node->main_num << std::endl;
                }
            }
            std::cout << this->func_name << "_cond" << if_else_node->cond_num[i-1] << ":" << std::endl;
            print_func_asm(if_else_node->conds[0].second);
                
            if (next_if_else) {
                std::cout << "  jmp " << this->func_name << "_if" << next_if_else->if_num << std::endl; 
            }
            else if (next_while) {
                std::cout << "  jmp " << this->func_name << "_loop" << next_while->while_num << std::endl;
            }
            else {
                std::cout << "  jmp " << this->func_name << "_main" << if_else_node->main_num << std::endl;
                std::cout << this->func_name << "_main" << if_else_node->main_num << ":" << std::endl;
            }
            print_func_asm(if_else_node->next);
        }
        
    }
    else if (while_node) {
        auto* next_while = dynamic_cast<WhileNode*>(while_node->next);
        auto* next_if_else = dynamic_cast<IfElseNode*>(while_node->next);
        
        std::cout << this->func_name << "_loop" << while_node->while_num << ":" << std::endl;
        print_func_asm(while_node->cond);
        std::cout << "  cmp rax, 0" << std::endl;
        if (next_while) {
            std::cout << "  je " << this->func_name << "_loop" << next_while->while_num << std::endl;
        }
        else if (next_if_else) {
            std::cout << "  je " << this->func_name << "_if" << next_if_else->if_num << std::endl;
        }
        else {
            std::cout << "  je " << this->func_name << "_main" << while_node->main_num << std::endl;
        }
        print_func_asm(while_node->stmts);
        std::cout << "  jmp " << this->func_name << "_loop" << while_node->while_num << std::endl;
        
        if (!(next_while || next_if_else)) {
            std::cout << this->func_name << "_main" << while_node->main_num << ":" << std::endl;
        }
        print_func_asm(while_node->next);
    }
    else if (return_node) {
        print_func_asm(return_node->return_val);
    }
};

Result* traverse_tree(ASTNode* ptr, ProgState* state) {
    auto* num_node = dynamic_cast<NumNode*>(ptr);
    auto* var_node = dynamic_cast<VarNode*>(ptr);
    auto* arrElem_node = dynamic_cast<ArrayElemNode*>(ptr);
    auto* bin_op_node = dynamic_cast<BinaryNode*>(ptr);
    auto* main_node = dynamic_cast<MainNode*>(ptr);
    auto* assign_node = dynamic_cast<AssignNode*>(ptr);
    auto* print_node = dynamic_cast<PrintNode*>(ptr);
    auto* scan_node = dynamic_cast<ScanNode*>(ptr);
    auto* statArrDecl_node = dynamic_cast<StatArrayDeclNode*>(ptr);
    auto* dynArrDecl_node = dynamic_cast<DynArrayDeclNode*>(ptr);
    auto* arrElemAssign_node = dynamic_cast<ArrayElemAssignNode*>(ptr);
    auto* if_else_node = dynamic_cast<IfElseNode*>(ptr);
    auto* while_node = dynamic_cast<WhileNode*>(ptr);
    auto* funcDef_node = dynamic_cast<FuncDef*>(ptr);
    auto* funcCall_node = dynamic_cast<FuncCall*>(ptr);
    
    if (num_node) { return new Result(ErrType::_OK_, -1, ""); }
    else if (var_node) { 
        if (state->vars.find(var_node->var_name) == state->vars.end()) {
            return new Result(ErrType::_ERR_VAR_, var_node->line_index, "Variable '" + var_node->var_name + "' not defined!");
        }
    }
    else if (arrElem_node) {
        if (state->arrs.find(arrElem_node->arr_name) == state->arrs.end()) {
            std::string arr = arrElem_node->arr_name.substr(1, arrElem_node->arr_name.length()-2);
            return new Result(ErrType::_ERR_ARR_, arrElem_node->line_index, "Unknown array '" + arr + "'!");
        }
        else {
            Result* index = traverse_tree(arrElem_node->elem_index, state);
            if (errResult(index)) return index;
        }
    }
    else if (bin_op_node) {
        Result* lhs = traverse_tree(bin_op_node->left, state);
        Result* rhs = traverse_tree(bin_op_node->right, state);
        
        if (errResult(lhs)) return lhs;
        else if (errResult(rhs)) return rhs;
    }
    else if (main_node) {
        Result* res = traverse_tree(main_node->next, state);
        if (errResult(res)) return res;
    }
    else if (assign_node) {
        Result* val = traverse_tree(assign_node->assign_val, state);
        if (errResult(val)) return val;
        
        auto it = state->vars.find(assign_node->var_name);
        switch (assign_node->assign_ty) {
            case VarType::_VAR_ : {
                if (it == state->vars.end()) {
                    state->vars[assign_node->var_name] = state->var_counter;
                    state->var_counter += VAR_STEP;
                } 
                break;
            }
            case VarType::_CONST_ : {
                if (it == state->vars.end()) {
                    state->vars[assign_node->var_name] = state->var_counter;
                    state->var_counter += VAR_STEP;
                }
                else {
                    return new Result(ErrType::_ERR_CONST_, assign_node->line_index, "Can't redefine const '" + assign_node->var_name + "'!!");
                }
                break;
            }
            default : {break;}
        }
        Result* res = traverse_tree(assign_node->next, state);
        if (errResult(res)) return res;
    }
    else if (arrElemAssign_node) {
        if (state->arrs.find(arrElemAssign_node->arr_name) == state->arrs.end()) {
            std::string arr = arrElemAssign_node->arr_name.substr(1, arrElemAssign_node->arr_name.length()-2);
            return new Result(ErrType::_ERR_ARR_, arrElemAssign_node->line_index, "Unknown array '" + arr + "'!");
        }
        else {
            Result* index = traverse_tree(arrElemAssign_node->elem_index, state);
            Result* val = traverse_tree(arrElemAssign_node->assign_val, state);
            Result* res = traverse_tree(arrElemAssign_node->next, state);
            
            if (errResult(index)) return index;
            else if (errResult(val)) return val;
            else if (errResult(res)) return res;
        }
    }
    else if (print_node) {
        Result* val = traverse_tree(print_node->print_val, state);
        Result* res = traverse_tree(print_node->next, state);
        
        if (errResult(val)) return val;
        else if (errResult(res)) return res;
    }
    else if (scan_node) {
        if (state->vars.find(scan_node->var_name) == state->vars.end())
            return new Result(ErrType::_ERR_VAR_, scan_node->line_index, "Variable '" + scan_node->var_name + "' not defined!");
        
        Result* res = traverse_tree(scan_node->next, state);
        if (errResult(res)) return res;
    }
    else if (statArrDecl_node) { 
        auto it = state->arrs.find(statArrDecl_node->arr_name);
        if (it == state->arrs.end()) { 
            state->arrs[statArrDecl_node->arr_name] = {
                state->var_counter + statArrDecl_node->arr_size*4, 
                _STAT_
            }; 
        }
        
        int n = statArrDecl_node->arr_size;
        for (int i = n-1; i >= 0; --i) {
            Result* val = traverse_tree(statArrDecl_node->arr_vals[i], state);
            
            if (errResult(val)) return val;
            
            std::string elem_name = statArrDecl_node->arr_name + std::to_string(i);
            
            state->vars[elem_name] = state->var_counter;
            state->var_counter += VAR_STEP;
        }
        
        Result* res = traverse_tree(statArrDecl_node->next, state);
        
        if (errResult(res)) return res;
    }
    else if (dynArrDecl_node) {
        dynArrDecl_node->arrayDecl_loop = state->arrayDecl_loop;
        state->arrayDecl_loop += 1;
        
        auto it = state->arrs.find(dynArrDecl_node->arr_name);
        if (it == state->arrs.end()) { state->arrs[dynArrDecl_node->arr_name] = {state->var_counter, _DYN_};}
        state->var_counter += VAR_STEP;
        
        Result* size = traverse_tree(dynArrDecl_node->arr_size, state);
        Result* val = traverse_tree(dynArrDecl_node->arr_val, state);
        Result* res = traverse_tree(dynArrDecl_node->next, state);
        
        if (errResult(size)) return size;
        else if (errResult(val)) return val;
        else if (errResult(res)) return res;
    }
    else if (if_else_node) {
        int n = if_else_node->conds.size();
        
        if (n == 1) {
            if_else_node->if_num = state->if_counter;
            if_else_node->main_num = state->main_counter;
            if_else_node->cond_num.push_back(state->cond_counter);
            
            state->if_counter += 1;
            state->main_counter += 1;
            state->cond_counter += 1;
            
            Result* cond = traverse_tree(if_else_node->conds[0].first, state);
            Result* stmts = traverse_tree(if_else_node->conds[0].second, state);
            Result* res = traverse_tree(if_else_node->next, state);
            
            if (errResult(cond)) return cond;
            else if (errResult(stmts)) return stmts;
            else if (errResult(res)) return res;
        }
        else {
            if_else_node->if_num = state->if_counter;
            if_else_node->main_num = state->main_counter;
            for (int i = 1; i < n; ++i) {
                if_else_node->cond_num.push_back(state->cond_counter + i - 1);
                Result* cond = traverse_tree(if_else_node->conds[i].first, state);
                
                if (errResult(cond)) return cond;
            }
            if_else_node->cond_num.push_back(state->cond_counter + n - 1);
            state->cond_counter += n;
            state->main_counter += n;
            
            for (int i = 1; i < n; ++i) {
                state->if_counter += 1;
                Result* stmts = traverse_tree(if_else_node->conds[i].second, state);
                
                if (errResult(stmts)) return stmts;
            }
            Result* stmts = traverse_tree(if_else_node->conds[0].second, state);
            Result* res = traverse_tree(if_else_node->next, state);
            
            if (errResult(stmts)) return stmts;
            else if (errResult(res)) return res;
        }
    }
    else if (while_node) {
        while_node->while_num = state->loop_counter;
        while_node->main_num = state->main_counter;
        state->loop_counter += 1;    
        state->main_counter += 1;
        
        Result* cond = traverse_tree(while_node->cond, state);
        Result* stmts = traverse_tree(while_node->stmts, state);
        Result* res = traverse_tree(while_node->next, state);
        
        if (errResult(cond)) return cond;
        else if (errResult(stmts)) return stmts;
        else if (errResult(res)) return res;
    }
    else if (funcDef_node) {
        for (auto it : funcDef_node->func_args) {
            auto* jt = dynamic_cast<VarNode*>(it);
            if (jt) {
                funcDef_node->func_state.vars[jt->var_name] = funcDef_node->func_state.var_counter;
                funcDef_node->func_state.var_counter += VAR_STEP;
            }
        }
        
        state->funcs[funcDef_node->func_name] = funcDef_node;
        Result* func_res = funcDef_node->traverse_func_tree(funcDef_node->func_stmts, *state);
        
        if (errResult(func_res)) return func_res;
        
        Result* res = traverse_tree(funcDef_node->next, state);
        if (errResult(res)) return res;
    }
    else if (funcCall_node) {
        auto name = state->funcs.find(funcCall_node->func_name);
        
        if (name == state->funcs.end()) {
            return new Result(ErrType::_ERR_FUNC_EXIST_, funcCall_node->line_index, "Function '" + funcCall_node->func_name + "' not defined!");
        }
        
        for (auto it : funcCall_node->func_args) {
            Result* tmp = traverse_tree(it, state);
            if (errResult(tmp)) return tmp;
        }
    }
    
    return new Result(ErrType::_OK_, -1, "");
};

void print_asm(ASTNode* ptr, ProgState state) {
    auto* num_node = dynamic_cast<NumNode*>(ptr);
    auto* var_node = dynamic_cast<VarNode*>(ptr);
    auto* arrElem_node = dynamic_cast<ArrayElemNode*>(ptr);
    auto* bin_op_node = dynamic_cast<BinaryNode*>(ptr);
    auto* main_node = dynamic_cast<MainNode*>(ptr);
    auto* assign_node = dynamic_cast<AssignNode*>(ptr);
    auto* print_node = dynamic_cast<PrintNode*>(ptr);
    auto* scan_node = dynamic_cast<ScanNode*>(ptr);
    auto* statArrDecl_node = dynamic_cast<StatArrayDeclNode*>(ptr);
    auto* dynArrDecl_node = dynamic_cast<DynArrayDeclNode*>(ptr);
    auto* arrElemAssign_node = dynamic_cast<ArrayElemAssignNode*>(ptr);
    auto* if_else_node = dynamic_cast<IfElseNode*>(ptr);
    auto* while_node = dynamic_cast<WhileNode*>(ptr);
    auto* funcDef_node = dynamic_cast<FuncDef*>(ptr);
    auto* funcCall_node = dynamic_cast<FuncCall*>(ptr);
    
    if (num_node) {
        std::cout << "  mov rax, " << num_node->num << std::endl;
    }
    else if (var_node) {
        std::cout << "  mov rax, QWORD PTR [rbp-" << 2 * state.vars[var_node->var_name] << "]" << std::endl;
    }
    else if (arrElem_node) {
        auto it = state.arrs.find(arrElem_node->arr_name);
        if (it != state.arrs.end()) {
            ArrayType ty = it->second.second;
            if (ty == ArrayType::_STAT_) {
                std::cout << "  lea rdi, [rbp-" << 2*state.arrs[arrElem_node->arr_name].first-8 << "]" << std::endl;
                print_asm(arrElem_node->elem_index, state);
                std::cout << "  mov rsi, rax" << std::endl;
                std::cout << "  call get" << std::endl;
            }
            else if (ty == ArrayType::_DYN_) {
                std::cout << "  mov rdi, QWORD PTR [rbp-" << 2*state.arrs[arrElem_node->arr_name].first << "]" << std::endl;
                print_asm(arrElem_node->elem_index, state);
                std::cout << "  mov rsi, rax" << std::endl;
                std::cout << "  call get" << std::endl;
            }
        }
    }
    else if (funcCall_node) {
        std::vector<std::string> asm_args = {"rdi", "rsi", "rdx", "rcx"};
        std::vector<int> call_registers;
        std::reverse(funcCall_node->func_args.begin(), funcCall_node->func_args.end());
        int n = (int)funcCall_node->func_args.size();
        for (int i = 0; i < n; ++i) {
            print_asm(funcCall_node->func_args[i], state);
            
            auto* it = dynamic_cast<FuncCall*>(funcCall_node->func_args[i]);
            
            if (it) {
                call_registers.push_back(i);
                std::cout << "  push rax" << std::endl;
            }
            else {
                std::cout << "  mov " << asm_args[i] << ", rax" << std::endl;
            }
        }
        std::reverse(call_registers.begin(), call_registers.end());
        int r = (int)call_registers.size();
        for (int i = 0; i < r; ++i) {
            std::cout << "  pop " << asm_args[call_registers[i]] << std::endl;
        }
        std::cout << "  call " + funcCall_node->func_name << std::endl;  
    }
    else if (bin_op_node) {
        switch (bin_op_node->tag) {
            case _ADD_ : {
                print_asm(bin_op_node->left, state);
                std::cout << "  push rax" << std::endl;
                print_asm(bin_op_node->right, state);
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  add rax, rbx" << std::endl;
                break;
            }
            case _SUB_ : {
                print_asm(bin_op_node->left, state);
                std::cout << "  push rax" << std::endl;
                print_asm(bin_op_node->right, state);
                std::cout << "  push rax" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  pop rax" << std::endl;
                std::cout << "  sub rax, rbx" << std::endl;
                break;
            }
            case _MUL_ : {
                print_asm(bin_op_node->left, state);
                std::cout << "  push rax" << std::endl;
                print_asm(bin_op_node->right, state);
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  imul rax, rbx" << std::endl;
                break;
            }
            case _DIV_ : {
                print_asm(bin_op_node->left, state);
                std::cout << "  push rax" << std::endl;
                print_asm(bin_op_node->right, state);
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  push rax" << std::endl;
                std::cout << "  mov rax, rbx" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  cqo" << std::endl;
                std::cout << "  xor rdx, rdx" << std::endl;
                std::cout << "  idiv rbx" << std::endl;
                break;
            }
            case _MOD_ : {
                print_asm(bin_op_node->left, state);
                std::cout << "  push rax" << std::endl;
                print_asm(bin_op_node->right, state);
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  push rax" << std::endl;
                std::cout << "  mov rax, rbx" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  cqo" << std::endl;
                std::cout << "  xor rdx, rdx" << std::endl;
                std::cout << "  idiv rbx" << std::endl;
                std::cout << "  mov rax, rdx" << std::endl;
                break;
            }
            case _SHL_ : {
                print_asm(bin_op_node->left, state);
                std::cout << "  push rax" << std::endl;
                print_asm(bin_op_node->right, state);
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  push rax" << std::endl;
                std::cout << "  mov rax, rbx" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  mov rdi, rax" << std::endl;
                std::cout << "  mov rsi, rbx" << std::endl;
                std::cout << "  call shlf" << std::endl;
                break;
            }
            case _SHR_ : {
                print_asm(bin_op_node->left, state);
                std::cout << "  push rax" << std::endl;
                print_asm(bin_op_node->right, state);
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  push rax" << std::endl;
                std::cout << "  mov rax, rbx" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  mov rdi, rax" << std::endl;
                std::cout << "  mov rsi, rbx" << std::endl;
                std::cout << "  call shrf" << std::endl;
                break;
            }
            case _AND_ : {
                print_asm(bin_op_node->left, state);
                std::cout << "  push rax" << std::endl;
                print_asm(bin_op_node->right, state);
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  and rax, rbx" << std::endl;
                break;
            }
            case _OR_ : {
                print_asm(bin_op_node->left, state);
                std::cout << "  push rax" << std::endl;
                print_asm(bin_op_node->right, state);
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  or rax, rbx" << std::endl;
                break;
            }
            case _NOT_ : {
                print_asm(bin_op_node->right, state);
                std::cout << "  not rax" << std::endl;
                break;
            }
            case _LESS_ : {
                print_asm(bin_op_node->left, state);
                std::cout << "  push rax" << std::endl;
                print_asm(bin_op_node->right, state);
                std::cout << "  push rax" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  pop rax" << std::endl;
                std::cout << "  mov rdi, rax" << std::endl;
                std::cout << "  mov rsi, rbx" << std::endl;
                std::cout << "  call cmp_less" << std::endl;  
                break;
            }
            case _GREAT_ : {
                print_asm(bin_op_node->left, state);
                std::cout << "  push rax" << std::endl;
                print_asm(bin_op_node->right, state);
                std::cout << "  push rax" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  pop rax" << std::endl;
                std::cout << "  mov rdi, rax" << std::endl;
                std::cout << "  mov rsi, rbx" << std::endl;
                std::cout << "  call cmp_great" << std::endl; 
                break;
            }
            case _EQ_ : {
                print_asm(bin_op_node->left, state);
                std::cout << "  push rax" << std::endl;
                print_asm(bin_op_node->right, state);
                std::cout << "  push rax" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  pop rax" << std::endl;
                std::cout << "  mov rdi, rax" << std::endl;
                std::cout << "  mov rsi, rbx" << std::endl;
                std::cout << "  call cmp_eq" << std::endl; 
                break;
            }
            case _NEQ_ : {
                print_asm(bin_op_node->left, state);
                std::cout << "  push rax" << std::endl;
                print_asm(bin_op_node->right, state);
                std::cout << "  push rax" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  pop rax" << std::endl;
                std::cout << "  mov rdi, rax" << std::endl;
                std::cout << "  mov rsi, rbx" << std::endl;
                std::cout << "  call cmp_neq" << std::endl; 
                break;
            }
            case _LEQ_ : {
                print_asm(bin_op_node->left, state);
                std::cout << "  push rax" << std::endl;
                print_asm(bin_op_node->right, state);
                std::cout << "  push rax" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  pop rax" << std::endl;
                std::cout << "  mov rdi, rax" << std::endl;
                std::cout << "  mov rsi, rbx" << std::endl;
                std::cout << "  call cmp_leq" << std::endl; 
                break;
            }
            case _GEQ_ : {
                print_asm(bin_op_node->left, state);
                std::cout << "  push rax" << std::endl;
                print_asm(bin_op_node->right, state);
                std::cout << "  push rax" << std::endl;
                std::cout << "  pop rbx" << std::endl;
                std::cout << "  pop rax" << std::endl;
                std::cout << "  mov rdi, rax" << std::endl;
                std::cout << "  mov rsi, rbx" << std::endl;
                std::cout << "  call cmp_geq" << std::endl; 
                break;
            }
            case _NEG_ : {
                print_asm(bin_op_node->right, state);
                std::cout << "  mov r8, -1" << std::endl;
                std::cout << "  mul r8" << std::endl;
                break;
            }
            default: break;
        }
    }
    else if (main_node) {
        std::cout << ".intel_syntax noprefix\n" << std::endl;

        std::cout << ".data" << std::endl;
        std::cout << "  print_format: .asciz \"\%ld\\n\"" << std::endl;
        std::cout << "  scan_format: .asciz \"\%ld\"" << std::endl;
        
        std::cout << "\n.text\n" << std::endl;
        std::cout << ".global main" << std::endl;
        
        for (auto it : state.funcs) {
            auto* jt = dynamic_cast<FuncDef*>(it.second);
            if (jt) { jt->print_func_asm(jt->func_stmts); }
        }
        
        std::cout << "main:" << std::endl;
        std::cout << "  push rbp" << std::endl;
        std::cout << "  mov rbp, rsp" << std::endl;
        
        int scans = 0;
        num_of_scans(main_node, &scans);
        scans *= 16;
        int vars = 0;
        for (auto it : state.vars) {
            if (vars < it.second) { vars = it.second; }
        }
        for (auto it : state.arrs) {
            if (vars < it.second.first) { vars = it.second.first; }
        }
        vars *= 2;
        
        if (scans >= vars) {
            std::cout << "  sub rsp, " << scans << std::endl;
        }
        else {
            std::cout << "  sub rsp, " << get_size(vars) << std::endl;
        }

        print_asm(main_node->next, state);
        std::cout << "  leave" << std::endl;
        std::cout << "  ret\n" << std::endl;
    }
    else if (assign_node) {
        print_asm(assign_node->assign_val, state);
        std::cout << "  mov QWORD PTR [rbp-" << 2 * state.vars[assign_node->var_name] << "], rax" << std::endl;
        print_asm(assign_node->next, state);
    }
    else if (arrElemAssign_node) {
        auto it = state.arrs.find(arrElemAssign_node->arr_name);
        if (it != state.arrs.end()) { 
            ArrayType ty = it->second.second;
            if (ty == ArrayType::_STAT_) {
                std::cout << "  lea rdi, [rbp-" << 2*state.arrs[arrElemAssign_node->arr_name].first << "]" << std::endl;
                print_asm(arrElemAssign_node->elem_index, state);
                std::cout << "  mov rsi, rax" << std::endl;
                print_asm(arrElemAssign_node->assign_val, state);
                std::cout << "  mov rdx, rax" << std::endl;
                std::cout << "  call set" << std::endl;
            }
            else if (ty == ArrayType::_DYN_) {
                std::cout << "  mov rdi, QWORD PTR [rbp-" << 2*state.arrs[arrElemAssign_node->arr_name].first << "]" << std::endl;
                print_asm(arrElemAssign_node->elem_index, state);
                std::cout << "  mov rsi, rax" << std::endl;
                print_asm(arrElemAssign_node->assign_val, state);
                std::cout << "  mov rdx, rax" << std::endl;
                std::cout << "  call set" << std::endl;
            }
            else { return; }
        }
        
        print_asm(arrElemAssign_node->next, state);
    }
    else if (print_node) {
        print_asm(print_node->print_val, state);
        std::cout << "  lea rdi, print_format" << std::endl;
        std::cout << "  mov rsi, rax" << std::endl;
        std::cout << "  xor rax, rax" << std::endl;
        std::cout << "  call printf" << std::endl;
        print_asm(print_node->next, state);
    }
    else if (scan_node) {
        std::cout << "  lea rdi, scan_format" << std::endl;
        std::cout << "  lea rsi, [rbp-" << 2 * state.vars[scan_node->var_name] << "]" << std::endl;
        std::cout << "  xor rax, rax" << std::endl;
        std::cout << "  call scanf" << std::endl;
        print_asm(scan_node->next, state);
    }
    else if (statArrDecl_node) {
        for (int i = 0; i < statArrDecl_node->arr_size; ++i) {
            print_asm(statArrDecl_node->arr_vals[i], state);
            
            std::string elem_name = statArrDecl_node->arr_name + std::to_string(i);
            std::cout << "  mov QWORD PTR [rbp-" << 2 * state.vars[elem_name] << "], rax" << std::endl;
        }
        
        print_asm(statArrDecl_node->next, state);
    }
    else if (dynArrDecl_node) {
        int num = num_vars(dynArrDecl_node->arr_size, 0);
        if (num == 0) {
            int res = num_expr_eval(dynArrDecl_node->arr_size, 0);
            std::cout << "  mov rax, " << std::to_string(res) << std::endl;
        }
        else {
            print_asm(dynArrDecl_node->arr_size, state);
        }
        std::cout << "  mov rdi, rax" << std::endl;
        print_asm(dynArrDecl_node->arr_val, state);
        std::cout << "  mov rsi, rax" << std::endl;
        std::cout << "  call dyn_malloc" << std::endl;
        
        std::cout << "  mov QWORD PTR [rbp-" << 2*state.arrs[dynArrDecl_node->arr_name].first << "], rax" << std::endl;
            
        print_asm(dynArrDecl_node->next, state);
    }
    else if (if_else_node) {
        auto* next_if_else = dynamic_cast<IfElseNode*>(if_else_node->next);
        auto* next_while = dynamic_cast<WhileNode*>(if_else_node->next);
        int n = if_else_node->conds.size();
        
        if (n == 1) {
            std::cout << "if" << if_else_node->if_num << ":" << std::endl;
            print_asm(if_else_node->conds[0].first, state);
            std::cout << "  cmp rax, 1" << std::endl;
            std::cout << "  je cond" << if_else_node->cond_num[0] << std::endl;
            
            if (next_if_else) {
                std::cout << "  jmp if" << next_if_else->if_num << std::endl;
            }
            else if (next_while) {
                std::cout << "  jmp loop" << next_while->while_num << std::endl;
            }
            else {
                std::cout << "  jmp main" << if_else_node->main_num << std::endl;
            }
            
            std::cout << "cond" << if_else_node->cond_num[0] << ":" << std::endl;
            print_asm(if_else_node->conds[0].second, state);
                
            if (next_if_else) {
                std::cout << "  jmp if" << next_if_else->if_num << std::endl; 
            }
            else if (next_while) {
                std::cout << "  jmp loop" << next_while->while_num << std::endl;
            }
            else {
                std::cout << "  jmp main" << if_else_node->main_num << std::endl;
            }
            
            if (!(next_if_else || next_while)) {
                std::cout << "main" << if_else_node->main_num << ":" << std::endl;
            }
            
            print_asm(if_else_node->next, state);
        }
        else {
            std::cout << "if" << if_else_node->if_num << ":" << std::endl;
            int i;
            for (i = 1; i < n; ++i) {
                print_asm(if_else_node->conds[i].first, state);
                std::cout << "  cmp rax, 1" << std::endl;
                std::cout << "  je cond" << if_else_node->cond_num[i-1] << std::endl;
            }
            std::cout << "  jmp cond" << if_else_node->cond_num[i-1] << std::endl;
            
            for (i = 1; i < n; ++i) {
                std::cout << "cond" << if_else_node->cond_num[i-1] << ":" << std::endl;
                print_asm(if_else_node->conds[i].second, state);
                
                if (next_if_else) {
                    std::cout << "  jmp if" << next_if_else->if_num << std::endl; 
                }
                else if (next_while) {
                    std::cout << "  jmp loop" << next_while->while_num << std::endl;
                }
                else {
                    std::cout << "  jmp main" << if_else_node->main_num << std::endl;
                }
            }
            std::cout << "cond" << if_else_node->cond_num[i-1] << ":" << std::endl;
            print_asm(if_else_node->conds[0].second, state);
                
            if (next_if_else) {
                std::cout << "  jmp if" << next_if_else->if_num << std::endl; 
            }
            else if (next_while) {
                std::cout << "  jmp loop" << next_while->while_num << std::endl;
            }
            else {
                std::cout << "  jmp main" << if_else_node->main_num << std::endl;
                std::cout << "main" << if_else_node->main_num << ":" << std::endl;
            }
            print_asm(if_else_node->next, state);
        }
        
    }
    else if (while_node) {
        auto* next_while = dynamic_cast<WhileNode*>(while_node->next);
        auto* next_if_else = dynamic_cast<IfElseNode*>(while_node->next);
        
        std::cout << "loop" << while_node->while_num << ":" << std::endl;
        print_asm(while_node->cond, state);
        std::cout << "  cmp rax, 0" << std::endl;
        if (next_while) {
            std::cout << "  je loop" << next_while->while_num << std::endl;
        }
        else if (next_if_else) {
            std::cout << "  je if" << next_if_else->if_num << std::endl;
        }
        else {
            std::cout << "  je main" << while_node->main_num << std::endl;
        }
        print_asm(while_node->stmts, state);
        std::cout << "  jmp loop" << while_node->while_num << std::endl;
        
        if (!(next_while || next_if_else)) {
            std::cout << "main" << while_node->main_num << ":" << std::endl;
        }
        print_asm(while_node->next, state);
    }
    else if (funcDef_node) {
        print_asm(funcDef_node->next, state);
    }
};
