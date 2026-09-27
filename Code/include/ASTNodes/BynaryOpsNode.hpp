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
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(Scopes::Scope* ParentScope) override; // types and value checking
        ReturnResult<Value> exec(Scopes::Scope* ParentScope) override; // execute and get the result of calculation
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
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(Scopes::Scope* ParentScope) override; // types and value checking
        ReturnResult<Value> exec(Scopes::Scope* ParentScope) override; // execute and get the result of compariton
        ASTNode* clone() override; // Clone the class or get a new copy from them
};