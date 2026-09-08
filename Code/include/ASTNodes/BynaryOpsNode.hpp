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
// Include neccessary headers
// ==================================================================
struct OperationPart {
    TokenType op;
    Token token;
    std::unique_ptr<ASTNode> node;
};

using OperationPartsList = std::vector<OperationPart>;

// ==================================================================
// Operatins node
// ==================================================================

// Plus (+) and mines (-) and multiple (*) and divide (/) operatins node
class BinOpsNode: public ASTNode {
    private:
        OperationPartsList Parts; // Parts of calculation

    public: 
        BinOpsNode(OperationPartsList& parts); // Constructure
        std::string get_str(int level) override; // Get the str to print
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(Scopes::Scope* ParentScope) override; // type and value checking
        ReturnResult<Value> exec(Scopes::Scope* ParentScope) override; // execute and get the result of calculation
        void push_to_result(double& result, TokenType op, double input); // Do binary operatiion on a node
};