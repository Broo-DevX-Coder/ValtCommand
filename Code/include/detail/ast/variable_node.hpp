/**
 * @file variable_node.hpp
 * @brief variable node declarations.
 */

#pragma once

#include "detail/api_export.hpp"

/// Include neccessary headers

/// == Locals ==
#include "detail/ast/ast_node.hpp"

/// Set variable nodes

/// Set varuable node: the node that create the variable and execute its value, after that it put the var in symbols table in parent scope
namespace vltc {

class SetVariableNode: public ASTNode {
    private:
        Token NameToken; ///< Token of Name of variable
        Token TypeToken; ///< Token Type of variable
        std::string name; ///< Name of variable
        std::string type; ///< Type of variable
        std::unique_ptr<ASTNode> VNode; ///< value node that givs the real value after exec
        bool is_const; ///< Is the variable const

    public:
        SetVariableNode(const std::string& FilePath, const Token& END_token, Token name_token, Token type_token, std::unique_ptr<ASTNode> value_node, bool is_const = false); ///< Constructure
        std::string get_str(int level) override; ///< Get the str to print
        nlohmann::json get_json() override; ///< Get the json file of ASTNode
        ASTNodesTypes NType() override; ///< Get the type of node
        ReturnResult<bool> accept(NodesContext Context) override; ///< type and value checking
        ReturnResult<Value> exec(NodesContext Context) override; ///< execute and set the var in symbols table
        ASTNode* clone() override; ///< Clone the class or get a new copy from them
};

/// Set varuable node: the node that reset the variable's value and execute it, after that it reset the value of var in symbols table in parent scope
class ResetVariableNode: public ASTNode {
    private:
        Token NameToken; ///< Token of Name of variable
        std::string name; ///< Name of variable
        std::string type; ///< Type of variable founded in scoupe
        std::unique_ptr<ASTNode> VNode; ///< value node that givs the real value after exec

    public:
        ResetVariableNode(const std::string& FilePath, const Token& END_token, Token name_token, std::unique_ptr<ASTNode> value_node); ///< Constructure
        std::string get_str(int level) override; ///< Get the str to print
        nlohmann::json get_json() override; ///< Get the json file of ASTNode
        ASTNodesTypes NType() override; ///< Get the type of node
        ReturnResult<bool> accept(NodesContext Context) override; ///< type and value checking
        ReturnResult<Value> exec(NodesContext Context) override; ///< execute and set the var's value in symbols table
        ASTNode* clone() override; ///< Clone the class or get a new copy from them
};

/// Get variable node

/// Get variable value: the node that get the value of var from symbols table
class GetVariableNode: public ASTNode {
        Token NameToken; ///< Token of Name of variable
        std::string name; ///< Name of variable
    public:
        GetVariableNode(const std::string& FilePath, const Token& END_token, Token name_token); ///< Constructure
        std::string get_str(int level) override; ///< Get the str to print
        nlohmann::json get_json() override; ///< Get the json file of ASTNode
        ASTNodesTypes NType() override; ///< Get the type of node
        ReturnResult<bool> accept(NodesContext Context) override; ///< type and value checking
        ReturnResult<Value> exec(NodesContext Context) override; ///< execute and get the value from symbols table
        ASTNode* clone() override; ///< Clone the class or get a new copy from them
};

} // namespace vltc
