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
// User Function argument node
// ==================================================================

// User Function argument node
class UserFunctionArgumentNode: public ASTNode {
    public:
        Token name_token; // argument's name token
        Token type_token; // argument's data type token
        Token default_value_ftoekn; // The first token of defualt value
        std::unique_ptr<ASTNode> default_value_node; // argument's defualt value node
        Value default_value; // The defualt value of argument that returns from default value noe
        bool is_argument_any; // Is the argument can sepport any type of data

        UserFunctionArgumentNode(const std::string& FilePath, const Token& END_token, Token& nameT, Token& typeT, Token& default_value_ftoekn, std::unique_ptr<ASTNode> default_value_node = nullptr, bool is_argument_any=false); // Constructure
        std::string get_str(int level) override; // Get str of node to print
        nlohmann::json get_json() override; // Get the json file of ASTNode
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(NodesContext Context) override; // The node verify it self befor runnig
        ReturnResult<Value> exec(NodesContext Context) override; // Execute node
        ASTNode* clone() override; // Clone the class or get a new copy from them
};

// ==================================================================
// User Function's return node
// ==================================================================

// User Function's return node
class UserFunctionReturnNode: public ASTNode {
    private:
        Token ReturnT;
        std::unique_ptr<ASTNode> VNode; // Node of value of return

    public:
        UserFunctionReturnNode(const std::string& FilePath, const Token& END_token, Token& t_, std::unique_ptr<ASTNode> value_node); // Constructure
        std::string get_str(int level) override; // Get str of node to print
        nlohmann::json get_json() override; // Get the json file of ASTNode
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(NodesContext Context) override; // The node verify it self befor runnig
        ReturnResult<Value> exec(NodesContext Context) override; // Execute node
        ASTNode* clone() override; // Clone the class or get a new copy from them
};

// ==================================================================
// User Proxy Function node
// ==================================================================

// User Proxy Function node
class UserProxyFunctionNode: public ASTNode {
    public: using NodesListT = std::vector<std::unique_ptr<ASTNode>>;
    private:
        NodesListT statements; // All nodes that function contains them

    public:
        UserProxyFunctionNode(const std::string& FilePath, const Token& END_token, NodesListT statements, const std::string& type); // Constructure
        std::string get_str(int level) override; // Get str of node to print
        nlohmann::json get_json() override; // Get the json file of ASTNode
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(NodesContext Context) override; // The node verify it self befor runnig
        ReturnResult<Value> exec(NodesContext Context) override; // Execute node
        ASTNode* clone() override; // Clone the class or get a new copy from them
};

// ==================================================================
// User Function node
// ==================================================================

// User Function node
class UserFunctionNode: public ASTNode {

    public:
        using NodesListT = std::vector<std::unique_ptr<ASTNode>>;
        using argumentsList = std::vector<std::unique_ptr<UserFunctionArgumentNode>>;
    private:
        Token name_token; // Function's name token
        Token return_type_token; // Function's return type token

        bool is_function_any; // Is the function can get any types of data

        argumentsList arguments_list; // arguments list
        NodesListT statements; // Nodes of function

    public:
        UserFunctionNode(const std::string& FilePath, const Token& END_token, Token& nameT, Token& typeT, argumentsList arguments_list, NodesListT statements, bool is_any=false); // Constructure
        std::string get_str(int level) override; // Get str of node to print
        nlohmann::json get_json() override; // Get the json file of ASTNode
        ASTNodesTypes NType() override; // Get the type of node
        ReturnResult<bool> accept(NodesContext Context) override; // The node verify it self befor runnig
        ReturnResult<Value> exec(NodesContext Context) override; // Execute node
        ASTNode* clone() override; // Clone the class or get a new copy from them
};
