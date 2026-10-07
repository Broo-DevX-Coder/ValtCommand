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
// Class
// ==================================================================

// If satments's node class
class IfStatmentsNode: public ASTNode{
    public:
        using Node = std::unique_ptr<ASTNode>;
        using Nodes_list = std::vector<std::unique_ptr<ASTNode>>;

    private:
        Node condition_node; // Condition node that returns true or false
        Nodes_list Statments; // All statmnts that will runed when condition is true
        Nodes_list Else_statments; // All statmnts that will runed when condition is false

        Token condition_FT; // The first token of condition

    public:
        IfStatmentsNode(Token& CFtoken ,Node& condition, Nodes_list& statments, Nodes_list& else_statments); // Constructure
        std::string get_str(int level) override; // Get the str to print
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(Scopes::Scope* ParentScope) override; // types and value checking
        ReturnResult<Value> exec(Scopes::Scope* ParentScope) override; // execute and get the result of calculation
        ASTNode* clone() override; // Clone the class or get a new copy from them
};