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

// Execute node
ReturnResult<Value> 
FunctionCallArgumentNode::exec() {
    auto vnode_r = VNode->exec();
    if (!vnode_r.success)
        return {vnode_r.Message,false,std::monostate{}};
    
    value = vnode_r.value;
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
ReturnResult<Value> 
FunctionCallNode::exec() {
    if (!Runtime::registries::functions.contains(name)) 
        return {
            Errors::NameError(
                vToken.value,
                vToken.line,
                vToken.column
            ).msg,
            false,
            std::monostate()
        };
    
    std::unordered_map<std::string, Value> args_list;
    for (auto& ar: arguments){
        auto arg_exec_r = ar->exec();
        if (!arg_exec_r.success) 
            return {arg_exec_r.Message,false,std::monostate()};

        args_list[ar->name] = ar->value;
    }
    return Runtime::registries::functions.at(name)(args_list);
}