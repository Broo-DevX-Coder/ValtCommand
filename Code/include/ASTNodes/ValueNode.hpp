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

        ValueNode(const std::string& FilePath, const Token& END_token, Token token, std::string type); // Constructure
        std::string get_str(int level) override; // Get str of node to print
        nlohmann::json get_json() override; // Get the json file of ASTNode
        ASTNodesTypes NType() override; // Get the type of node
};

// ==================================================================
// Values Nodes
// ==================================================================

// Class String Value Node
class StringValueNode: public ValueNode {
    public:
        StringValueNode(const std::string& FilePath, const Token& END_token, Token token); // Constructure
        ReturnResult<bool> accept(NodesContext Context) override; // The node verifi it self befor runnig
        ReturnResult<Value> exec(NodesContext Context) override; // Execute the node and get result
        ASTNodesTypes NType() override; // Get the type of node
        ASTNode* clone() override; // Clone the class or get a new copy from them
};

// Class Value Number Node
class NumberValueNode: public ValueNode {
    public:
        NumberValueNode(const std::string& FilePath, const Token& END_token, Token token, std::string type="float"); // Constructure
        ReturnResult<bool> accept(NodesContext Context) override; // The node verifi it self befor runnig
        ReturnResult<Value> exec(NodesContext Context) override; // Execute the node and get result
        ASTNodesTypes NType() override; // Get the type of node
        ASTNode* clone() override; // Clone the class or get a new copy from them
};

// Class Value Bool Node
class BooleanValueNode: public ValueNode {
    public:
        BooleanValueNode(const std::string& FilePath, const Token& END_token, Token token); // Constructure
        ReturnResult<bool> accept(NodesContext Context) override; // The node verifi it self befor runnig
        ReturnResult<Value> exec(NodesContext Context) override; // Execute the node and get result
        ASTNodesTypes NType() override; // Get the type of node
        ASTNode* clone() override; // Clone the class or get a new copy from them
};
