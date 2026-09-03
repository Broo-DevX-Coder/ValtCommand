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

// == Libs ==
#include <vector>
#include <unordered_map>

// == Locals ==
#include "ASTNodes/ASTNode.hpp"

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
        Scopes::SymbolTableTypes::Function* func; // The pointer of function symbol in scop's table

        FunctionCallNode(std::string Fname, ArgsT& Args_list, Token name_token); // Contructure
        std::string get_str(int level) override; // Get str of node to print
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(Scopes::Scope* ParentScope) override; // The node verifi it self befor runnig
        ReturnResult<Value> exec(Scopes::Scope* ParentScope) override; // Execute node
};