#include "detail/lexer.hpp"

namespace vltc {
namespace {
bool is_digit(char c) { return c >= '0' && c <= '9'; }
bool is_alpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
bool is_space(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}
}

Lexar::Lexar(const std::string& c_): code(c_) {
    if (!code.empty()) curent_c = code.front();
    else code_ended = true;
}

void Lexar::advence() {
    if (code_ended) return;
    if (curent_c == '\n') {
        ++curent_line;
        curent_column = 0;
    } else {
        ++curent_column;
    }
    ++pos;
    if (pos < code.size()) curent_c = code[pos];
    else {
        curent_c = '\0';
        code_ended = true;
    }
}

char Lexar::peek(size_t offset) const {
    return pos < code.size() && offset < code.size() - pos ? code[pos + offset] : '\0';
}

void Lexar::escape_spaces() {
    while (!code_ended && is_space(curent_c)) advence();
}

void Lexar::escape_comments() {
    escape_spaces();
    while (!code_ended && curent_c == '/') {
        const char next = peek();
        if (next == '/') {
            while (!code_ended && curent_c != '\n') advence();
        } else if (next == '*') {
            const size_t start_line = curent_line;
            const size_t start_column = curent_column;
            advence(); advence();
            bool closed = false;
            while (!code_ended) {
                if (curent_c == '*' && peek() == '/') {
                    advence(); advence();
                    closed = true;
                    break;
                }
                advence();
            }
            if (!closed) {
                unterminated_block_comment = true;
                curent_line = start_line;
                curent_column = start_column;
                return;
            }
        } else break;
        escape_spaces();
    }
}

Token Lexar::get_the_next_token() {
    escape_comments();
    if (unterminated_block_comment) {
        unterminated_block_comment = false;
        return {TokenType::UNTERMINATED_COMMENT, "unterminated block comment", curent_line, curent_column, curent_column};
    }
    if (code_ended) return {TokenType::END_CODE, "", curent_line, curent_column, curent_column};

    const size_t start_line = curent_line;
    const size_t start_column = curent_column;
    if (peek() != '\0') {
        const std::string pair{curent_c, peek()};
        if (__complex_2_symbols__.contains(pair)) {
            Token token = __complex_2_symbols__.at(pair);
            token.line = start_line; token.column = start_column; token.last_column = start_column + 1;
            advence(); advence();
            return token;
        }
    }
    if (__symbols__.contains(curent_c)) {
        Token token = __symbols__.at(curent_c);
        token.line = start_line; token.column = start_column; token.last_column = start_column;
        advence();
        return token;
    }
    if (is_digit(curent_c)) {
        std::string value;
        bool dot = false;
        size_t last_column = start_column;
        while (!code_ended && (is_digit(curent_c) || (!dot && curent_c == '.'))) {
            if (curent_c == '.') dot = true;
            value += curent_c; last_column = curent_column; advence();
        }
        return {dot ? TokenType::FLOAT : TokenType::INTEGER, value, start_line, start_column, last_column};
    }
    if (is_alpha(curent_c) || curent_c == '_') {
        std::string value;
        size_t last_column = start_column;
        while (!code_ended && (is_digit(curent_c) || is_alpha(curent_c) || curent_c == '_')) {
            value += curent_c; last_column = curent_column; advence();
        }
        TokenType type = TokenType::IDENTIFIER;
        if (is_token_type_(value)) type = TokenType::TYPE;
        if (value == "END") type = TokenType::END_BLOCK;
        if (is_token_key_word_(value)) type = TokenType::KEY_WORD;
        if (value == "True" || value == "False") type = TokenType::BOOLEAN;
        return {type, value, start_line, start_column, last_column};
    }
    if (curent_c == '"' || curent_c == '\'') {
        const char quote = curent_c;
        std::string value;
        advence();
        while (!code_ended && curent_c != quote && curent_c != '\n') {
            if (curent_c == '\\') {
                advence();
                if (code_ended || curent_c == '\n') break;
                const auto escaped = __backslashed_symbols__.find(curent_c);
                if (escaped != __backslashed_symbols__.end()) value += escaped->second;
                else { value += '\\'; value += curent_c; }
            } else value += curent_c;
            advence();
        }
        if (curent_c != quote)
            return {TokenType::UNTERMINATED_STRING, "unterminated string", start_line, start_column, start_column};
        const size_t end_column = curent_column;
        advence();
        return {TokenType::STRING, value, start_line, start_column, end_column};
    }
    const char unknown = curent_c;
    std::string unknown_value(1, unknown);
    const auto first_byte = static_cast<unsigned char>(unknown);
    size_t sequence_length = first_byte >= 0xF0 ? 4 : first_byte >= 0xE0 ? 3 : first_byte >= 0xC2 ? 2 : 1;
    advence();
    for (size_t i = 1; i < sequence_length && !code_ended; ++i) {
        const auto next_byte = static_cast<unsigned char>(curent_c);
        if ((next_byte & 0xC0) != 0x80) break;
        unknown_value += curent_c;
        advence();
    }
    return {TokenType::UNKNOWN, unknown_value, start_line, start_column, start_column + unknown_value.size() - 1};
}

std::vector<Token> Lexar::get_all_tokens() {
    std::vector<Token> tokens;
    Token token = get_the_next_token();
    tokens.push_back(token);
    while (token.Type != TokenType::END_CODE) {
        token = get_the_next_token();
        tokens.push_back(token);
    }
    return tokens;
}

} // namespace vltc
