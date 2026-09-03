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

// All Tokens Types by string
std::unordered_map<TokenType,std::string> TokenTypesStr = {
    {TokenType::IDENTIFIER,"IDENTIFIER"},
    {TokenType::TYPE,"TYPE"},
    {TokenType::KEY_WORD,"KEY_WORD"},
    {TokenType::STRING,"STRING"},
    {TokenType::INTEGER,"INTEGER"},
    {TokenType::FLOAT,"FLOAT"},
    {TokenType::BOOLEAN,"BOOLEAN"},
    {TokenType::COLON,"COLON"},
    {TokenType::LESS_THAN,"LESS_THAN"},
    {TokenType::GREATER_THAN,"GREATER_THAN"},
    {TokenType::LEFT_BRACKET,"LEFT_BRACKET"},
    {TokenType::RIGHT_BRACKET,"RIGHT_BRACKET"},
    {TokenType::LEFT_BRACE,"LEFT_BRACE"},
    {TokenType::RIGHT_BRACE,"RIGHT_BRACE"},
    {TokenType::LEFT_PAREN,"LEFT_PAREN"},
    {TokenType::RIGHT_PAREN,"RIGHT_PAREN"},
    {TokenType::SLASH,"SLASH"},
    {TokenType::BACKSLASH,"BACKSLASH"},
    {TokenType::STAR,"STAR"},
    {TokenType::MINUS,"MINUS"},
    {TokenType::PLUS,"PLUS"},
    {TokenType::EQUAL,"EQUAL"},
    {TokenType::CARET,"CARET"},
    {TokenType::END_BLOCK,"END_BLOCK"},
    {TokenType::END_CODE,"END_CODE"},
    {TokenType::UNKNOWN,"UNKNOWN"}
};

// All sepported types
std::vector<std::string> __types__ = {
    "str", // string
    "int", // integer
    "float", // double or float
    "bool", // boolean
    "void" // means null or void in c++
};

// All sepported keywords
std::vector<std::string> __key_words__ = {
    "CALL" // Call function
};

// All sepported symbols
std::unordered_map<char,Token> __symbols__ = {
    {'<',{TokenType::LESS_THAN,"<",0,0}},
    {'>',{TokenType::GREATER_THAN,">",0,0}},
    {':',{TokenType::COLON,":",0,0}},
    {'[',{TokenType::LEFT_BRACKET,"[",0,0}},
    {']',{TokenType::RIGHT_BRACKET,"]",0,0}},
    {'{',{TokenType::LEFT_BRACE,"{",0,0}},
    {'}',{TokenType::RIGHT_BRACE,"}",0,0}},
    {'(',{TokenType::LEFT_PAREN,"(",0,0}},
    {')',{TokenType::RIGHT_PAREN,")",0,0}},
    {'/',{TokenType::SLASH,"/",0,0}},
    {'\\',{TokenType::BACKSLASH,"\\",0,0}},
    {'*',{TokenType::STAR,"*",0,0}},
    {'-',{TokenType::MINUS,"-",0,0}},
    {'+',{TokenType::PLUS,"+",0,0}},
    {'=',{TokenType::EQUAL,"=",0,0}},
    {'^',{TokenType::CARET,"^",0,0}}
};

// All TokenTypes like string
std::unordered_map<TokenType,std::string> TokenTypes_to_StringType = {
    {TokenType::STRING,"str"},
    {TokenType::INTEGER,"int"},
    {TokenType::FLOAT,"float"},
    {TokenType::BOOLEAN,"bool"}
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

// Get Value type (what inside variant)
std::string 
Get_ValueT(
    const Value& value
) {
    if (std::holds_alternative<std::monostate>(value))
        return "void";

    if (std::holds_alternative<uint64_t>(value))
        return "int";

    if (std::holds_alternative<double>(value))
        return "float";

    if (std::holds_alternative<std::string>(value))
        return "string";

    if (std::holds_alternative<bool>(value))
        return "bool";

    return "unknown";
}

// Are two types compatible (like int with float)
bool 
are_types_compatible(
    const std::string& first, 
    const std::string& secound
) {
    if (
        (first == "int" && secound == "float") ||
        (first == "float" && secound == "int")
    ) return true;
    return first == secound;
}