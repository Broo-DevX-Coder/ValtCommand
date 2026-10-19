
#pragma once

// ==================================================================
// Include necessary headers
// ==================================================================

// == Locals ==
#include "ASTNodes/ASTNode.hpp"

// ==================================================================
// Sign operation types
// ==================================================================

enum class SignOpsTypes {
    POSITIVE,
    NEGATIVE
};

// ==================================================================
// Unary Sign Operation Node
// ==================================================================

class API SignOpNode : public ASTNode {
private:
    // The type of unary sign operation (positive or negative)
    SignOpsTypes type;

    // Token representing the sign operator (+ or -)
    Token token;

    // The first token of Operand node
    Token Operand_FT;

    // The AST node representing the operand
    std::unique_ptr<ASTNode> node;

public:
    // Constructor: Initialize the sign operation node
    SignOpNode(
        SignOpsTypes type,
        const Token& token,
        const Token& operand_ft,
        std::unique_ptr<ASTNode>& node
    );

    // Get the string representation of the node for debugging
    std::string get_str(int level) override;

    // Get the json file of ASTNode
    nlohmann::json get_json() override; 

    // Get the AST node type
    ASTNodesTypes NType() override;

    // Perform semantic analysis and validate the operand
    ReturnResult<bool> accept(NodesContext Context) override;

    // Execute the unary sign operation and return the result
    ReturnResult<Value> exec(NodesContext Context) override;

    // Create a deep copy of the node
    ASTNode* clone() override;
};


// ==================================================================
// Logical NOT Node
// ==================================================================
class API LogicalNotNode : public ASTNode {
private:
    // Token representing the logical NOT operator (!)
    Token token;

    // The first token of Operand node
    Token Operand_FT;

    // AST node representing the operand
    std::unique_ptr<ASTNode> node;

public:
    // Constructor: Initialize the logical NOT node
    LogicalNotNode(
        const Token& token,
        const Token& operand_ft,
        std::unique_ptr<ASTNode>& node
    );

    // Get the string representation of the node for debugging
    std::string get_str(int level) override;

    // Get the json file of ASTNode
    nlohmann::json get_json() override;

    // Get the AST node type
    ASTNodesTypes NType() override;

    // Perform semantic analysis and validate the operand
    ReturnResult<bool> accept(NodesContext Context) override;

    // Execute the logical NOT operation
    ReturnResult<Value> exec(NodesContext Context) override;

    // Create a deep copy of the node
    ASTNode* clone() override;
};