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
    private:
        using Node = std::unique_ptr<ASTNode>;
        using PNode = std::unique_ptr<ProgramNode>;
        using TList = std::vector<Token>;

        size_t pos = 0; // Iterator of the curent pos
        Token curent_token_; // The curent token
        TList tokens_list_; // All tokens that will parsed

        bool code_ended_ = false; // Is the parsing operation Done

    public:
        Parser(TList& tokens_list); // Constructure
        void advence(); // Go to the next token
        bool match(TokenType token_t_); // Verifi if the curent token is in type `token_t_` 
        bool next_match(TokenType tt_); // Verifi if the next token is in type `token_t_` 

        Node get_next_node();
        PNode get_program_node(); // Get the clear program node that contain all parsed code
};