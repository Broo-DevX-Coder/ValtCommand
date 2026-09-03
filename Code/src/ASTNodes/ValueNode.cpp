// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "ASTNodes/ValueNode.hpp"
#include "Errors.hpp"

// == Libs ==
#include <sstream>

// ==================================================================
// Main Value Node Functions
// ==================================================================

// Constructure
ValueNode::ValueNode(
    Token t_,
    std::string tt_
): 
    vToken(t_),
    type(tt_)
{
    return_type = type;
}

// Get str of node to print
std::string
ValueNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "Value:" << "\n";

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "| ";
    ss << "Type: " << type << "\n";

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "| ";
    ss << "Value: " << vToken.value << "\n";

    return ss.str();
}

// Get node Type
ASTNodesTypes 
ValueNode::NType() {
    return NT__ValueNode;
}

// ==================================================================
// Values Nodes functions
// ==================================================================

// ================== Class String Value Node ==================

// Constructure
StringValueNode::StringValueNode(
    Token t_
): ValueNode(t_,"str") {}

// The node verifi it self befor runnig
ReturnResult<bool> 
StringValueNode::accept(
    Scopes::Scope* ParentScope
) {
    if (vToken.Type != TokenType::STRING)
        return {
            Errors::TypeError(
                type,
                TokenTypes_to_StringType[vToken.Type],
                vToken.line,
                vToken.column
            ).msg,false,false};
    
    return {"",true,true};
}

// Execute the node and get result
ReturnResult<Value> 
StringValueNode::exec(
    Scopes::Scope* ParentScope
){
    value = vToken.value;
    return {"",true,std::move(value)};
}

// Get node Type
ASTNodesTypes 
StringValueNode::NType() {
    return NT__StringValueNode;
}

// ================== Class Number Value Node ==================

// Constructure
NumberValueNode::NumberValueNode(
    Token t_,
    std::string tt_
): ValueNode(t_,tt_) {}

// The node verifi it self befor runnig
ReturnResult<bool> 
NumberValueNode::accept(
    Scopes::Scope* ParentScope
) {
    if (vToken.Type != TokenType::INTEGER && vToken.Type != TokenType::FLOAT)
        return {
            Errors::TypeError(
                type,
                TokenTypes_to_StringType[vToken.Type],
                vToken.line,
                vToken.column
            ).msg,false,false};
    
    return {"",true,true};
}

// Execute the node and get result
ReturnResult<Value> 
NumberValueNode::exec(
    Scopes::Scope* ParentScope
){
    auto v = std::stoul(vToken.value);
    value = type == "int"? (uint64_t)v : (double)v;
    
    return {"",true,std::move(value)};
}

// Get node Type
ASTNodesTypes 
NumberValueNode::NType() {
    return NT__NumberValueNode;
}

// ================== Class Boolean Value Node ==================

// Constructure
BooleanValueNode::BooleanValueNode(
    Token t_
): ValueNode(t_,"bool") {}

// The node verifi it self befor runnig
ReturnResult<bool> 
BooleanValueNode::accept(
    Scopes::Scope* ParentScope
) {
    if (vToken.Type != TokenType::BOOLEAN)
        return {
            Errors::TypeError(
                type,
                TokenTypes_to_StringType[vToken.Type],
                vToken.line,
                vToken.column
            ).msg,false,false};

    return {"",true,true};
}

// Execute the node and get result
ReturnResult<Value> 
BooleanValueNode::exec(
    Scopes::Scope* ParentScope
){
    value = vToken.value == "True";
    return {"",true,std::move(value)};
}

// Get node Type
ASTNodesTypes 
BooleanValueNode::NType() {
    return NT__BooleanValueNode;
}