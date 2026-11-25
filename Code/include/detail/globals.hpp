/**
 * @file globals.hpp
 * @brief Internal lexer, formatter, and value helper declarations.
 */
#pragma once

#include "valtcommand/globals.hpp"

namespace vltc {

namespace Errors {
VALTC_API std::string formatter(ErrorInfo info, const std::string& code);
}

extern VALTC_API std::unordered_map<TokenType, std::string> TokenTypesStr;
extern VALTC_API std::unordered_map<TokenType, std::string> TokenTypes_to_StringType;
extern VALTC_API std::vector<std::string> __types__;
extern VALTC_API std::vector<std::string> __key_words__;
extern VALTC_API std::unordered_map<char, Token> __symbols__;
extern VALTC_API std::unordered_map<std::string, Token> __complex_2_symbols__;
extern VALTC_API std::unordered_map<char, char> __backslashed_symbols__;

VALTC_API bool is_token_type_(std::string token);
VALTC_API bool is_token_key_word_(std::string token);
VALTC_API std::string Get_ValueT(const Value& value);
VALTC_API bool are_types_compatible(const std::string& first, const std::string& secound);
VALTC_API ReturnResult<Value> reconsiliation_int_float(
    const std::string& type,
    Value& input,
    const Token& source_token
);
VALTC_API ReturnResult<long double> turn_value_to_float(Value& input);

} // namespace vltc
