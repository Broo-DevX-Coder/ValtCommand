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
        value = type == "int"? (int)v : (double)v;

    } else if (type == "bool") {
        value = v_=="True";

    }
}

// get str to print
std::string
FunctionCallArgumentNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i;i<level*3;i++)
        ss << " ";
    ss << "\n";
    ss << "Argument " << name << ":" << "\n";

    for (int i;i<(level*3)+2;i++) 
        ss << " ";
    ss << "\n";
    ss << "Type: " << type << "\n";

    for (int i;i<(level*3)+2;i++) 
        ss << " ";
    ss << "\n";
    ss << "Value: " << value_str << "\n";

    return ss.str();
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
    arguments(a_) 
{}

// get str to print
std::string
FunctionCallNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i;i<level*3;i++)
        ss << " ";
    ss << "\n";
    ss << "FunctionCall " << name << ":" << "\n";

    for (auto& arg: arguments)
        ss << arg.get_str(level+1);

    return ss.str();
}