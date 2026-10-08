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
    {TokenType::EXCLAMATION,"EXCLAMATION"},
    {TokenType::EQUAL_EQUAL,"EQUAL_EQUAL"},
    {TokenType::NOT_EQUAL,"NOT_EQUAL"},
    {TokenType::LESS_EQUAL,"LESS_EQUAL"},
    {TokenType::GREATER_EQUAL,"GREATER_EQUAL"},
    {TokenType::LOGICAL_AND,"LOGICAL_AND"},
    {TokenType::LOGICAL_OR,"LOGICAL_OR"},
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
    "CALL", // Call function
    "SET", // Set of reset variable
    "SET_CONST", // Set const variable
    "GET", // Get a variable value 
    "FUNCTION", // Set user function
    "RETURN", // Return a value from function to outside
    "IF", // Start of if statment
    "ELSE", // Start of else statment
    "THEN", // Start of executing block after condition
    "WHILE", // Start of While loop
    "BREAK", // Break from a loop
    "CONTINUE" // Continue a loop
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
    {'^',{TokenType::CARET,"^",0,0}},
    {'!',{TokenType::EXCLAMATION,"!",0,0}}
};

// Comparitions operations
std::unordered_map<std::string, Token> __complex_2_symbols__ = {
    {"==", {TokenType::EQUAL_EQUAL, "==", 0, 0}},
    {"!=", {TokenType::NOT_EQUAL, "!=", 0, 0}},
    {"<=", {TokenType::LESS_EQUAL, "<=", 0, 0}},
    {">=", {TokenType::GREATER_EQUAL, ">=", 0, 0}},
    {"&&", {TokenType::LOGICAL_AND, "&&", 0, 0}},
    {"||", {TokenType::LOGICAL_OR, "||", 0, 0}}
};

// All TokenTypes like string
std::unordered_map<TokenType,std::string> TokenTypes_to_StringType = {
    {TokenType::STRING,"str"},
    {TokenType::INTEGER,"int"},
    {TokenType::FLOAT,"float"},
    {TokenType::BOOLEAN,"bool"}
};

// All symbols like `\n` or `\t`
std::unordered_map<char,char> 
__backslashed_symbols__ = {
    {'n', '\n'},
    {'t', '\t'}
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

    if (std::holds_alternative<int64_t>(value))
        return "int";

    if (std::holds_alternative<long double>(value))
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

// Reconsiliation between intiger and float
Value 
reconsiliation_int_float(
    const std::string& type, 
    Value& input
) {
    Value value;

    if (type=="int") {
        int64_t* v = std::get_if<int64_t>(&input);
    
        if (v!=nullptr) {
            value = *v;
        } else {
            long double* v = std::get_if<long double>(&input);
            value = static_cast<int64_t>(*v);
        }
    
    }else if (type == "float") {
        long double* v = std::get_if<long double>(&input);
    
        if (v!=nullptr) {
            value = *v;
        } else {
            int64_t* v = std::get_if<int64_t>(&input);
            value = static_cast<long double>(*v);
        }
    
    } else value=input;

    return value;
}

// Turn a Value type to float
ReturnResult<long double>
turn_value_to_float(
    Value& input
) {
    long double value;

    long double* v = std::get_if<long double>(&input);
    if (v!=nullptr) {
        value = *v;
    } else {
        int64_t* v = std::get_if<int64_t>(&input);

        if (v != nullptr)
            value = static_cast<long double>(*v);
        else 
            return {"The input is not a int64_t or long double",false,SIZE_MAX};
    }
    

    return {"",true,value};
}