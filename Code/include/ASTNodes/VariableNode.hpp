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
// Set variable nodes
// ==================================================================

// Set varuable node: the node that create the variable and execute its value, after that it put the var in symbols table in parent scope
class SetVariableNode: public ASTNode {
    private:
        Token NameToken; // Token of Name of variable
        Token TypeToken; // Token Type of variable
        std::string name; // Name of variable
        std::string type; // Type of variable
        std::unique_ptr<ASTNode> VNode; // value node that givs the real value after exec
        bool is_const; // Is the variable const

    public: 
        SetVariableNode(Token name_token, Token type_token, std::unique_ptr<ASTNode> value_node, bool is_const = false); // Constructure
        std::string get_str(int level) override; // Get the str to print
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(Scopes::Scope* ParentScope) override; // type and value checking
        ReturnResult<Value> exec(Scopes::Scope* ParentScope) override; // execute and set the var in symbols table
};

// Set varuable node: the node that reset the variable's value and execute it, after that it reset the value of var in symbols table in parent scope
class ResetVariableNode: public ASTNode {
    private:
        Token NameToken; // Token of Name of variable
        std::string name; // Name of variable
        std::string type; // Type of variable founded in scoupe
        std::unique_ptr<ASTNode> VNode; // value node that givs the real value after exec

    public: 
        ResetVariableNode(Token name_token, std::unique_ptr<ASTNode> value_node); // Constructure
        std::string get_str(int level) override; // Get the str to print
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(Scopes::Scope* ParentScope) override; // type and value checking
        ReturnResult<Value> exec(Scopes::Scope* ParentScope) override; // execute and set the var's value in symbols table
};

// Get variable value: the node that get the value of var from symbols table
class GetVariableNode: public ASTNode {
        Token NameToken; // Token of Name of variable
        std::string name; // Name of variable
    public: 
        GetVariableNode(Token name_token); // Constructure
        std::string get_str(int level) override; // Get the str to print
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(Scopes::Scope* ParentScope) override; // type and value checking
        ReturnResult<Value> exec(Scopes::Scope* ParentScope) override; // execute and get the value from symbols table
};