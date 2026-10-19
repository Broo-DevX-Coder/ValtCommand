#pragma once

// ==================================================================
// Marcos
// ==================================================================
#ifdef BUILDING_COMPILER_DLL
    #if defined(_WIN32)
        #define API __declspec(dllexport)
    #else
        #define API __attribute__((visibility("default")))
    #endif
#else
    #if defined(_WIN32)
        #define API __declspec(dllimport)
    #else
        #define API 
    #endif
#endif

// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "ASTNodes/ASTNode.hpp"

// ==================================================================
// Structs, enums and types
// ==================================================================

// All types of comparions types
enum class ComparitonOpsTypes {
    EQUAL,
    NOT_EQUAL,
    LESS_THEN,
    GREATER_THEN,
    LESS_THEN_OR_EQUAL,
    GREATER_THEN_OR_EQUAL
};

// Operation part's type
struct OperationPart {
    TokenType op;
    Token token;
    std::unique_ptr<ASTNode> node;
};

using OperationPartsList = std::vector<OperationPart>;

// ==================================================================
// Vars
// ==================================================================
extern std::unordered_map<TokenType,ComparitonOpsTypes> ComparitonSymbols_ToOps; // Map for eatch comparition operation's symbol with its type

// ==================================================================
// Operations node
// ==================================================================

// Plus (+) and mines (-) and multiple (*) and divide (/) operatins node
class BinOpsNode: public ASTNode {
    private:
        OperationPartsList Parts; // Parts of calculation

    public: 
        BinOpsNode(OperationPartsList& parts); // Constructure
        std::string get_str(int level) override; // Get the str to print
        nlohmann::json get_json() override; // Get the json file of ASTNode
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(NodesContext Context) override; // types and value checking
        ReturnResult<Value> exec(NodesContext Context) override; // execute and get the result of calculation
        ASTNode* clone() override; // Clone the class or get a new copy from them
        void push_to_result(long double& result, TokenType op, long double input); // Do binary operatiion on a node
};

// Equal (==), not equal (!=) , less then (<), grater then (>), less then or equal (<=) and grater then or equal (>=)
class CompOpsNode: public ASTNode {
    private:
        OperationPart first_part; // The first part of comparition
        OperationPart second_part; // The secound part of comparition
        ComparitonOpsTypes type; // Type of operation

    public:
        CompOpsNode(OperationPart& first_token, OperationPart& secound_token, ComparitonOpsTypes type); // Constructure
        std::string get_str(int level) override; // Get the str to print
        nlohmann::json get_json() override; // Get the json file of ASTNode
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(NodesContext Context) override; // types and value checking
        ReturnResult<Value> exec(NodesContext Context) override; // execute and get the result of compariton
        ASTNode* clone() override; // Clone the class or get a new copy from them
};

// AND (||) and OR (||) Logical operation
class LogicOpsNode: public ASTNode {
    private:
        OperationPartsList Parts; // Parts of calculation

    public:
        LogicOpsNode(OperationPartsList& parts); // Constructure
        std::string get_str(int level) override; // Get the str to print
        nlohmann::json get_json() override; // Get the json file of ASTNode
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(NodesContext Context) override; // types and value checking
        ReturnResult<Value> exec(NodesContext Context) override; // execute and get the result of compariton
        ASTNode* clone() override; // Clone the class or get a new copy from them
};

// ==================================================================
// Power Operation Node
// ==================================================================
class API PowerNode : public ASTNode {
private:
    // Token representing the power operator (**)
    Token token;

    // AST nodes representing the base and exponent
    OperationPart left;
    OperationPart right;

public:
    PowerNode(const Token& token,OperationPart& left,OperationPart& right);  // Constructor: Initialize the power operation node
    std::string get_str(int level) override; // Get the string representation of the node for debugging    
    nlohmann::json get_json() override; // Get the json file of ASTNode
    ASTNodesTypes NType() override; // Get the AST node type    
    ReturnResult<bool> accept(NodesContext Context) override; // Perform semantic analysis and validate both operands    
    ReturnResult<Value> exec(NodesContext Context) override; // Execute the power operation and return the result
    ASTNode* clone() override; // Create a deep copy of the node
};