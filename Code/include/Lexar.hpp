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

// == Libs ==
#include <vector>

// ==================================================================
// Class
// ==================================================================

// This is the maain lixar class
class API Lexar {
    private:
        size_t pos = 0; // The curent position in code
        size_t curent_line = 0; // The line that the cursor is on.
        size_t curent_column = 0; // The number letter in current line that the cursor is on.
        std::string code; // The code's program
        char curent_c; // The curent byte in code
        bool code_ended = false; // Is the code done or no, used to stop all whileloops and return END_CODE type
    public:
        Lexar(std::string& code); // Constructure
        void advence(); // Move to the next byte in code; pos++
        void escape_spaces(); // Jump on all spaces and escape chaines
        Token get_the_next_token(); // Get the next token from code
        std::vector<Token> get_all_tokens(); // Get all tokens in code
};