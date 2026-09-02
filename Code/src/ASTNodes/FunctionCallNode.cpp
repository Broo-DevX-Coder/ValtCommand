// ==================================================================
// Include neccessary headers
// ==================================================================

// == Libs ==
#include <sstream>
#include <cstdint>

// == Locals ==
#include "ASTNodes/FunctionCallNode.hpp"
#include "Errors.hpp"

// ==================================================================
// Function Call Argument Node
// ==================================================================

// Constructure
FunctionCallArgumentNode::FunctionCallArgumentNode(
    std::string Aname, 
    std::string Atype, 
    std::unique_ptr<ASTNode> Value_node
): 
    name(Aname),
    type(Atype),
    VNode(std::move(Value_node)) {}

// get str to print
std::string
FunctionCallArgumentNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "Argument " << name << ":" << "\n";

    ss <<  VNode->get_str(level+1);

    return ss.str();
}

// Get the node type
ASTNodesTypes 
FunctionCallArgumentNode::NType() {
    return NT__FunctionCallArgumentNode;
}

// Verifi the node 
ReturnResult<bool>
FunctionCallArgumentNode::accept(
    Scopes::Scope* ParentScope
) {
    auto vnode_ar = VNode->accept(ParentScope);
    if (!vnode_ar.success)
        return {vnode_ar.Message,false,false};
    return {"",true,true};
}

// Execute node
ReturnResult<Value> 
FunctionCallArgumentNode::exec(
    Scopes::Scope* ParentScope
) {
    auto vnode_r = VNode->exec(ParentScope);
    if (!vnode_r.success)
        return {vnode_r.Message,false,std::monostate{}};
    
    value = vnode_r.value;
    VNode = nullptr;

    return {"",true,std::monostate{}};
}

// ==================================================================
// Function Call Node
// ==================================================================

// Constructure
FunctionCallNode::FunctionCallNode(
    std::string Fname, 
    ArgsT& Args_list, 
    Token token
): 
    name(Fname),
    arguments(std::move(Args_list)),
    vToken(token)
{}

// get str to print
std::string
FunctionCallNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "FunctionCall " << name << ":" << "\n";

    for (auto& arg: arguments)
        ss << arg->get_str(level+1);

    return ss.str();
}

// Get the node type
ASTNodesTypes 
FunctionCallNode::NType() {
    return NT__FunctionCallNode;
}

// Execute node
ReturnResult<bool> 
FunctionCallNode::accept(
    
    Scopes::Scope* ParentScope
) {
    auto search_result = ParentScope->search_function(vToken);
    if (!search_result.success)
        return {
            search_result.Message,
            false,false
        };
    
    for (auto& ar: arguments){
        auto arg_accept_r = ar->accept(ParentScope);
        if (!arg_accept_r.success) 
            return {arg_accept_r.Message,false,false};
    }

    return {"",true,true};
}


// Execute node
ReturnResult<Value> 
FunctionCallNode::exec(
    Scopes::Scope* ParentScope
) {
    auto search_result = ParentScope->search_function(vToken);
    
    std::unordered_map<std::string, Value> args_list;
    for (auto& ar: arguments){
        auto arg_exec_r = ar->exec(ParentScope);
        if (!arg_exec_r.success) 
            return {arg_exec_r.Message,false,std::monostate()};

        args_list[ar->name] = ar->value;
        ar = nullptr;
    }
    arguments.clear();

    if (search_result.value->type == Scopes::SymbolTableTypes::FunctionsTypes::Extenal)
        return search_result.value->external_func(args_list);
    return search_result.value->external_func(args_list);
}