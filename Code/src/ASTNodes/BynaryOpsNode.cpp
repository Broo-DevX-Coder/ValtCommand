// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "ASTNodes/BynaryOpsNode.hpp"
#include "Errors.hpp"

// == Libs ==
#include <sstream>

// ==================================================================
// Base (linear) operatins node classes
// ==================================================================

// Constructure
BinOpsNode::BinOpsNode(
    OperationPartsList& parts_
): 
    Parts(std::move(parts_)) {}

// Get str to print
std::string 
BinOpsNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "Linear operation " << ":\n";

    for (auto& part: Parts) {
        for (int i=0;i<level+1;i++)
            ss << "|  ";

        if (part.op == TokenType::PLUS) ss << "plus+" << ":\n";
        else if (part.op == TokenType::MINUS) ss << "minus-" << ":\n";
        else if (part.op == TokenType::STAR) ss << "multiple*" << ":\n";
        else if (part.op == TokenType::SLASH) ss << "divide/" << ":\n";
        else ss << "First: " << "\n";



        ss << part.node->get_str(level+2);
    }

    return ss.str();
}

// get type of node
ASTNodesTypes 
BinOpsNode::NType() {
    return NT__BinOpsNode;
}

// Typess checking
ReturnResult<bool> 
BinOpsNode::accept(
    Scopes::Scope* ParentScope
) {
    return_type = "float";
    for (auto& part: Parts) {
        auto accept_result = part.node->accept(ParentScope);
        if (!accept_result.success) {
            return {accept_result.Message,false,false};
        }
        if (part.node->return_type != "int" && part.node->return_type != "float") {
            return {Errors::TypeError(
                "int | float",
                part.node->return_type,
                part.token.line,
                part.token.column,
                "This operation does not support type else then int or float"
            ).msg,false,false};
        }
    }
    return {"",true,true};
}

// Do operation on a node
void
BinOpsNode::push_to_result(
    double& result, 
    TokenType op, 
    double input
) {
    if (op == TokenType::PLUS) {
        result += input;
    } else if (op == TokenType::MINUS) {
        result -= input;
    } else if (op == TokenType::STAR) {
        result *= input;
    } else if (op == TokenType::SLASH) {
        result /= input;
    }
}

// execute operation node
ReturnResult<Value> 
BinOpsNode::exec(
    Scopes::Scope* ParentScope
) { 
    double result = 0;

    for (size_t i=0;i<Parts.size();i++) {
        auto& part = Parts[i];

        auto exec_result = part.node->exec(ParentScope);
        if (!exec_result.success) return {exec_result.Message,false,std::monostate{}};

        size_t* val = std::get_if<size_t>(&exec_result.value);

        if (val != nullptr) {
            if (i==0) {
                result = static_cast<double>(*val);
            } else {
                push_to_result(result,part.op,static_cast<double>(*val));
            }
        } else {
            double* val = std::get_if<double>(&exec_result.value);
            if (i==0) {
                result = static_cast<double>(*val);
            } else {
                push_to_result(result,part.op,static_cast<double>(*val));
            }
        }
    }

    return {"",true,result};
}