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

        bool code_ended_ = false; // Is the parsing operation Done

    public:

        Parser(TList& tokens_list); // Constructure
        void advence(); // Go to the next token
        ReturnResult<bool> match(TokenType token_t_, std::string Emsg=""); // Verifi if the curent token is in type `token_t_` 
        ReturnResult<bool> next_match(TokenType tt_, std::string Emsg=""); // Verifi if the next token is in type `token_t_` 

        ReturnResult<Node> get_next_node();
        ReturnResult<PNode> get_module_node(); // Get the clear program node that contain all parsed code
};