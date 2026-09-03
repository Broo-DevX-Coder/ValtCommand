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
#include "globals.hpp"

// Nodes
#include "ASTNodes/FunctionCallNode.hpp"

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
        
        void advence(); // Go to the next token
        const Token& curent(); // Get the curent token
        const Token& peek(size_t offset=1); // Return the next <offset> token
        bool check(TokenType type);  // Check the type of curent token
        ReturnResult<Token> consume(TokenType type); // Return curent token, check its type, and advence
        bool isAsEnd(); // is the code ended
        

        // Grammars 
        ReturnResult<Node> get_node();
        ReturnResult<PNode> get_module_node(); // Get the clear program node that contain all parsed code

        ReturnResult<Node> get_functioncall_node(); // Get the function call node when found CALL keyword
        ReturnResult<Node> get_value_node(); // Get the pure value node
};