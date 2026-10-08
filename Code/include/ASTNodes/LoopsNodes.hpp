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
// Class of while loops
// ==================================================================

// While loops node
class WhileLoopsNode: public ASTNode {
    public:
        using Node = std::unique_ptr<ASTNode>;
        using NodesL = std::vector<Node>;

    private:
        Node condition_node; // Condition of whiile loop
        NodesL Statments; // All statments that will execte and repete if condition is true
        Token FirstToken; // The first token of loop definition

    public:
        WhileLoopsNode(Token& FirstToken, Node& condition, NodesL& nodes_list); // Constructure
        std::string get_str(int level) override; // Get the str to print
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(Scopes::Scope* ParentScope) override; // types and value checking
        ReturnResult<Value> exec(Scopes::Scope* ParentScope) override; // execute the loop
        ASTNode* clone() override; // Clone the class or get a new copy from them
};

// ==================================================================
// Classes of break and continue node
// ==================================================================

// Break loop
class BreakLoopNode: public ASTNode {
    public:
        std::string get_str(int level) override; // Get the str to print
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(Scopes::Scope* ParentScope) override; // types and value checking
        ReturnResult<Value> exec(Scopes::Scope* ParentScope) override; // execute the loop
        ASTNode* clone() override; // Clone the class or get a new copy from them
};

// Continue loop
class ContinueLoopNode: public ASTNode {
    public:
        std::string get_str(int level) override; // Get the str to print
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(Scopes::Scope* ParentScope) override; // types and value checking
        ReturnResult<Value> exec(Scopes::Scope* ParentScope) override; // execute the loop
        ASTNode* clone() override; // Clone the class then get a new copy from them
};