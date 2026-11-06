/**
 * @file function_call_node.hpp
 * @brief function call node declarations.
 */

#pragma once

#include "detail/api_export.hpp"

/// Include neccessary headers

/// == Locals ==
#include "detail/ast/user_function_node.hpp"

/// Function Call Argument Node

/// Function Call Argument Node
class FunctionCallArgumentNode: public ASTNode
{
    public:
        std::string name; ///< Name of argument
        std::string type; ///< Type of argument
        Value value; ///< Value that in argumen
        Token TypeToken; ///< toke of type
        std::unique_ptr<ASTNode> VNode; ///< AST Node that givs Argument its content

        FunctionCallArgumentNode(const std::string& FilePath, const Token& END_token, std::string& Aname, Token TypeToken, std::unique_ptr<ASTNode> Value_node); ///< Constructure
        std::string get_str(int level) override; ///< Get str of node to print
        nlohmann::json get_json() override; ///< Get the json file of ASTNode
        ASTNodesTypes NType() override; ///< Get the type of node
        ReturnResult<bool> accept(NodesContext Context) override; ///< The node verifi it self befor runnig
        ReturnResult<Value> exec(NodesContext Context) override; ///< Execute node
        ASTNode* clone() override; ///< Clone the class or get a new copy from them
};


/// Function Call Node

/// Function Call Node
class FunctionCallNode: public ASTNode
{
    public:
        using ArgsT = std::vector<std::unique_ptr<FunctionCallArgumentNode>>;

        std::string name; ///< Name of function
        ArgsT arguments; ///< Function arguments
        Token NameToken; ///< Token of function name in code
        std::unique_ptr<UserProxyFunctionNode> user_func_copy; ///< The proxy of user's function copy

        FunctionCallNode(const std::string& FilePath, const Token& END_token, std::string Fname, ArgsT& Args_list, Token name_token); ///< Contructure
        std::string get_str(int level) override; ///< Get str of node to print
        nlohmann::json get_json() override; ///< Get the json file of ASTNode
        ASTNodesTypes NType() override; ///< Get the type of node
        ReturnResult<bool> accept(NodesContext Context) override; ///< The node verifi it self befor runnig
        ReturnResult<Value> exec(NodesContext Context) override; ///< Execute node
        ASTNode* clone() override; ///< Clone the class or get a new copy from them
};
