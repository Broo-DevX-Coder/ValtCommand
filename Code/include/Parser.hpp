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
#include "globals.hpp"

// ==================================================================
// Parser
// ==================================================================

class API Parser {
    public:
        using Node = std::unique_ptr<ASTNode>;
        using PNode = std::unique_ptr<ModuleNode>;
        using TList = std::vector<Token>;

    private:
        size_t pos = 0; // Iterator of the curent pos
        Token curent_token_; // The curent token
        TList tokens_list_; // All tokens that will parsed

        bool is_code_ended_ = false; // Is the parsing operation Done

    public:

        Parser(TList& tokens_list); // Constructure
        ReturnResult<PNode> get_module_node(); // Get the clear program node that contain all parsed code
        ReturnResult<Node> get_expretion(); // Get The linear operations node

    private:

        // Basic functions
        void advence(); // Go to the next token
        const Token& curent(); // Get the curent token
        const Token& peek(size_t offset=1); // Return the next <offset> token
        bool check(TokenType type);  // Check the type of curent token
        ReturnResult<Token> consume(TokenType type); // Return curent token, check its type, and advence
        bool isAsEnd(); // is the code ended

        // Grammars
        ReturnResult<Node> get_term(); // Get the complex operation node
        ReturnResult<Node> get_primary(); // Get primary nodes like Values and functions ...

        ReturnResult<Node> get_functioncall_node(); // Get the function call node when found CALL keyword
        ReturnResult<Node> get_value_node(); // Get the pure value node
        ReturnResult<Node> get_set_variable_node(bool is_const); // Get the set or reset variable node when found SET keyword
        ReturnResult<Node> get_get_variable_node(); // Get the node that get the variable value from symbols table
        ReturnResult<Node> get_user_function_node(); // Get the node of user's function that put the function in symbol table
        ReturnResult<Node> get_return_noode(); // Get the node of returning value in function
        ReturnResult<Node> get_if_statment_node();// Get the node of `IF` keyword (if statment)
        ReturnResult<Node> get_while_loop_node();// Get the node of While loop `WHILE`
        ReturnResult<Node> get_break_loop_node();// Get the node of break a loop `BREAK`
        ReturnResult<Node> get_continue_loop_node();// Get the node of continue a loop `CONTINUE`  
};