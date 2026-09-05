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
// Main Value Node
// ==================================================================

// Value Node
class ValueNode: public ASTNode
{
    public:
        Token vToken; // The token of value
        std::string type; // The expected type of value
        Value value; // Value that in argumen

        ValueNode(Token token, std::string type); // Constructure
        std::string get_str(int level) override; // Get str of node to print
        ASTNodesTypes NType() override; // Get the type of node
};

// ==================================================================
// Values Nodes
// ==================================================================

// Class String Value Node
class StringValueNode: public ValueNode {
    public:
        StringValueNode(Token token); // Constructure
        ReturnResult<bool> accept(Scopes::Scope* ParentScope) override; // The node verifi it self befor runnig
        ReturnResult<Value> exec(Scopes::Scope* ParentScope) override; // Execute the node and get result
        ASTNodesTypes NType() override; // Get the type of node
};

// Class Value Number Node
class NumberValueNode: public ValueNode {   
    public:
        NumberValueNode(Token token, std::string type="float"); // Constructure
        ReturnResult<bool> accept(Scopes::Scope* ParentScope) override; // The node verifi it self befor runnig
        ReturnResult<Value> exec(Scopes::Scope* ParentScope) override; // Execute the node and get result
        ASTNodesTypes NType() override; // Get the type of node
};

// Class Value Bool Node
class BooleanValueNode: public ValueNode {
    public:
        BooleanValueNode(Token token); // Constructure
        ReturnResult<bool> accept(Scopes::Scope* ParentScope) override; // The node verifi it self befor runnig
        ReturnResult<Value> exec(Scopes::Scope* ParentScope) override; // Execute the node and get result
        ASTNodesTypes NType() override; // Get the type of node
};