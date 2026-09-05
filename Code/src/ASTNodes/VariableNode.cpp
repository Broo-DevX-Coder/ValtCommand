// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "ASTNodes/VariableNode.hpp"
#include "Errors.hpp"

// == Libs ==
#include <sstream>

// ==================================================================
// Set variable node class functions
// ==================================================================

// Constructure
SetVariableNode::SetVariableNode(
    Token name_token,
    Token type_token, 
    std::unique_ptr<ASTNode> value_node_,
    bool is_const_
):
    NameToken(name_token),
    TypeToken(type_token),
    name(name_token.value),
    type(type_token.value),
    is_const(is_const_),
    VNode(std::move(value_node_)) {}


// Get the str to print
std::string SetVariableNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    if (is_const) ss << "Const " << name << ":\n";
    else ss << "Variable " << name << ":\n";

    for (int i=0;i<level+1;i++)
        ss << "|  ";
    ss << "type: " << type << "\n";

    for (int i=0;i<level+1;i++)
        ss << "|  ";
    ss << "value: " << "\n";

    ss <<  VNode->get_str(level+2);

    return ss.str();
}

// Get the type of node
ASTNodesTypes 
SetVariableNode::NType() {
    return NT__SetVariablNode;
}

// type and value checking
ReturnResult<bool> 
SetVariableNode::accept(
    Scopes::Scope* ParentScope
) {
    return_type = "void";

    auto parent_scope_id = ParentScope->get_id();
    auto search_r = ParentScope->search_var(NameToken);

    Errors::VariableError err(
        name,
        NameToken.line,
        NameToken.column
    );

    if (search_r.success) {
        if (parent_scope_id == search_r.value->scope_id)
            return {err.reset_var_in_same_scope(),false,false};
    }

    auto vnode_ar = VNode->accept(ParentScope);
    if (!vnode_ar.success) return {vnode_ar.Message,false,false};

    if (!are_types_compatible(VNode->return_type,type)){
        return {
            Errors::TypeError(
                type,VNode->return_type,
                TypeToken.line,
                TypeToken.column
            ).msg,false,false
        };
    }

    Value v = "";
    ParentScope->add_var(name,type,v,is_const);

    return {"",true,true};
}

// execute and set the var in symbols table
ReturnResult<Value> 
SetVariableNode::exec(
    Scopes::Scope* ParentScope
) {
    auto vnode_ar = VNode->exec(ParentScope);
    if (!vnode_ar.success) return {vnode_ar.Message,false,false};

    auto value = vnode_ar.value;
    ParentScope->add_var(name,type,value,is_const);

    return {"",true,std::monostate{}};
}


// ==================================================================
// Reset variable node class functions
// ==================================================================

// Constructure
ResetVariableNode::ResetVariableNode(
    Token name_token,
    std::unique_ptr<ASTNode> value_node_
):
    NameToken(name_token),
    name(name_token.value),
    VNode(std::move(value_node_)) {}


// Get the str to print
std::string 
ResetVariableNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "Reset Variable " << name << ":\n";

    for (int i=0;i<level+1;i++)
        ss << "|  ";
    ss << "value: " << "\n";

    ss <<  VNode->get_str(level+2);

    return ss.str();
}

// Get the type of node
ASTNodesTypes 
ResetVariableNode::NType() {
    return NT__ResetVariablNode;
}

// type and value checking
ReturnResult<bool> 
ResetVariableNode::accept(
    Scopes::Scope* ParentScope
) {
    return_type = "void";

    auto parent_scope_id = ParentScope->get_id();
    auto search_r = ParentScope->search_var(NameToken);

    Errors::VariableError err(
        name,
        NameToken.line,
        NameToken.column
    );

    // Variable not found
    if (!search_r.success) {
        return {search_r.Message,false,false};
    }

    auto var_ptr = search_r.value;

    // Variable is const
    if (var_ptr->is_const) {
        return {err.reset_const_var(),false,false};
    }

    // variable is in auther scope
    if (parent_scope_id != search_r.value->scope_id){
        return {err.reset_var_from_auther_scope(),false,false};
    }

    type = var_ptr->type;

    auto vnode_ar = VNode->accept(ParentScope);
    if (!vnode_ar.success) return {vnode_ar.Message,false,false};

    // value type is incompatible with variable type
    if (VNode->return_type != type){
        return {
            err.reset_var_value_by_other_type(type,VNode->return_type),false,false
        };
    }

    return {"",true,true};
}

// Excute node
ReturnResult<Value> 
ResetVariableNode::exec(
    Scopes::Scope* ParentScope
) {
    auto vnode_ar = VNode->exec(ParentScope);
    if (!vnode_ar.success) return {vnode_ar.Message,false,false};

    auto value = vnode_ar.value;
    auto search_r = ParentScope->search_var(NameToken);
    search_r.value->value = value;

    return {"",true,std::monostate{}};
}


// ==================================================================
// Reset variable node class functions
// ==================================================================

// Constructure
GetVariableNode::GetVariableNode(
    Token name_token
):
    NameToken(name_token),
    name(name_token.value) {}


// Get the str to print
std::string 
GetVariableNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "Get Variable " << name << "\n";

    return ss.str();
}

// Get the type of node
ASTNodesTypes 
GetVariableNode::NType() {
    return NT__GetVariableNode;
}

// type and value checking
ReturnResult<bool> 
GetVariableNode::accept(
    Scopes::Scope* ParentScope
) {
    auto search_r = ParentScope->search_var(NameToken);

    // Variable not found
    if (!search_r.success) {
        return {search_r.Message,false,false};
    }

    return_type = search_r.value->type;
    return {"",true,true};
}

// Execute node
ReturnResult<Value> 
GetVariableNode::exec(
    Scopes::Scope* ParentScope
) {
    auto search_r = ParentScope->search_var(NameToken);
    return {"",true,search_r.value->value};
}