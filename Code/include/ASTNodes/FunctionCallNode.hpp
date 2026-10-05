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
#include "ASTNodes/UserFunctionNode.hpp"

// ==================================================================
// Function Call Argument Node
// ==================================================================

// Function Call Argument Node
class FunctionCallArgumentNode: public ASTNode
{
    public:
        std::string name; // Name of argument
        std::string type; // Type of argument
        Value value; // Value that in argumen
        Token TypeToken; // toke of type
        std::unique_ptr<ASTNode> VNode; // AST Node that givs Argument its content

        FunctionCallArgumentNode(std::string& Aname, Token TypeToken, std::unique_ptr<ASTNode> Value_node); // Constructure
        std::string get_str(int level) override; // Get str of node to print
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(Scopes::Scope* ParentScope) override; // The node verifi it self befor runnig
        ReturnResult<Value> exec(Scopes::Scope* ParentScope) override; // Execute node
        ASTNode* clone() override; // Clone the class or get a new copy from them
};


// ==================================================================
// Function Call Node
// ==================================================================

// Function Call Node
class FunctionCallNode: public ASTNode
{
    public:
        using ArgsT = std::vector<std::unique_ptr<FunctionCallArgumentNode>>;

        std::string name; // Name of function
        ArgsT arguments; // Function arguments
        Token NameToken; // Token of function name in code
        std::unique_ptr<UserProxyFunctionNode> user_func_copy; // The proxy of user's function copy

        FunctionCallNode(std::string Fname, ArgsT& Args_list, Token name_token); // Contructure
        std::string get_str(int level) override; // Get str of node to print
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(Scopes::Scope* ParentScope) override; // The node verifi it self befor runnig
        ReturnResult<Value> exec(Scopes::Scope* ParentScope) override; // Execute node
        ASTNode* clone() override; // Clone the class or get a new copy from them
};