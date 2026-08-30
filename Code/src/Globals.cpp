// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "globals.hpp"

// == Libs ==
#include <algorithm>

// ==================================================================
// Vars
// ==================================================================

// All sepported types
std::vector<std::string> __types__ = {
    "str", // string
    "int", // integer
    "float", // double or float
    "bool" // boolean
};

// All sepported keywords
std::vector<std::string> __key_words__ = {
    "END", // End of block (Functioon call, var ...)
    "CALL" // Call function
};

// All sepported symbols
std::unordered_map<char,Token> __symbols__ = {
    {'<',{LESS_THAN,"<"}},
    {'>',{GREATER_THAN,">"}},
    {':',{COLON,":"}}
};

// ==================================================================
// Functions
// ==================================================================

// Is the token type
bool is_token_type_(
    std::string token
) {
    auto it = std::find(
        __types__.begin(),
        __types__.end(),
        token
    );

    if (it != __types__.end())
        return true;
    return false;
}


// is the token key word
bool is_token_key_word_(
    std::string token
) {
    auto it = std::find(
        __key_words__.begin(),
        __key_words__.end(),
        token
    );

    if (it != __key_words__.end())
        return true;
    return false;
}