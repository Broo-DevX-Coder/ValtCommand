// ==================================================================
// Include neccessary headers
// ==================================================================

// == Libs ==
#include <sstream>
#include <cstdint>

// == Locals ==
#include "ASTNodes/FunctionCallNode.hpp"

// ==================================================================
// Function Call Argument Node
// ==================================================================

// Constructure
FunctionCallArgumentNode::FunctionCallArgumentNode(
    std::string n_,
    std::string t_,
    std::string v_
): 
    name(n_),
    type(t_),
    value_str(v_)
{
    if (type == "str") {
        value = v_;

    } else if (type == "int" || type == "float") {
        auto v = std::stoul(v_);
        value = type == "int"? (uint64_t)v : (double)v;

    } else if (type == "bool") {
        value = v_=="True";

    } 

    if (v_.empty()){
        value = std::monostate();
    }
}

// get str to print
std::string
FunctionCallArgumentNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "Argument " << name << ":" << "\n";

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "| ";
    ss << "Type: " << type << "\n";

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "| ";
    ss << "Value: " << value_str << "\n";

    return ss.str();
}

// Get the node type
ASTNodesTypes 
FunctionCallArgumentNode::NType() {
    return T__FunctionCallArgumentNode;
}

// Execute node
void 
FunctionCallArgumentNode::exec() {
    return;
}

// ==================================================================
// Function Call Node
// ==================================================================

// Constructure
FunctionCallNode::FunctionCallNode(
    std::string n_, 
    ArgsT& a_
): 
    name(n_),
    arguments(std::move(a_)) 
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
    return T__FunctionCallNode;
}

// Execute node
void 
FunctionCallNode::exec() {
    if (!Runtime::registries::functions.contains(name)) 
        throw std::runtime_error(
            fmt::format("No function named {}",name)
        );
    
    std::unordered_map<std::string, Value> args_list;
    for (auto& ar: arguments)
        args_list[ar->name] = ar->value;

    Runtime::registries::functions.at(name)(args_list);
}