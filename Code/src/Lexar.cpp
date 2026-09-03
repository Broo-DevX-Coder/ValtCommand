// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Lexar.hpp"

// ==================================================================
// Lexar functions
// ==================================================================

// Constructure
Lexar::Lexar(
    std::string& c_
): code(c_) {
    if (!c_.empty()) {
        curent_c = code[pos];
    } else 
        code_ended = true;
}

// advence
void
Lexar::advence() {
    pos++;
    if (pos < code.size()) {
        curent_c = code[pos];
        if (curent_c != '\n') 
            curent_column++;
    } else 
        code_ended = true;
}

// Wolk on spaces
void 
Lexar::escape_spaces() {
    
    if (code_ended == true) 
        return;
    
    while ((std::isspace(curent_c) || curent_c == '\n' || curent_c == '\t') && !code_ended) {
        if (curent_c == '\n') {
            curent_line++;
            curent_column = 0;
        }
        advence();
    }
}

// get the next token function
Token
Lexar::get_the_next_token() {
    escape_spaces();

    // If there is not any char
    if (code_ended == true) {
        return {TokenType::END_CODE, "", curent_line, curent_column};

    // If char is < or >
    } else if (__symbols__.contains(curent_c)) {
        auto t = __symbols__[curent_c];
        advence();
        t.line = curent_line;
        t.column = curent_column;
        return t;

    // If the curent char is digit
    } else if (std::isdigit(curent_c)) {
        bool dot = false;
        std::string tk;

        tk += curent_c;
        advence();

        while ((std::isdigit(curent_c) || (!dot && curent_c == '.')) && !code_ended) {
            if (curent_c == '.') 
                dot = true;
            tk += curent_c;
            advence();
        }

        if (dot)
            return {TokenType::FLOAT, tk, curent_line, curent_column};
        else 
            return {TokenType::INTEGER, tk, curent_line, curent_column};

    // If curent char is letter
    } else if (std::isalpha(curent_c) || curent_c == '_') {
        std::string tk;

        do {
            tk += curent_c;
            advence(); 
        } while ((std::isdigit(curent_c) || std::isalpha(curent_c) || curent_c == '_') && !code_ended);

        TokenType tk_t = TokenType::IDENTIFIER;

        if (is_token_type_(tk))
            tk_t = TokenType::TYPE;

        if (tk == "END") 
            tk_t = TokenType::END_BLOCK;

        if (is_token_key_word_(tk))
            tk_t = TokenType::KEY_WORD;

        if (
            tk == "True" ||
            tk == "False"
        ) 
            tk_t = TokenType::BOOLEAN;
        
        return {tk_t, tk, curent_line, curent_column};

    // If curent type is string
    } else if (curent_c == '"' || curent_c == '\'') {
        std::string tk;
        char c = curent_c;

        advence();

        while ((curent_c != c) && !code_ended) {
            tk += curent_c;
            advence(); 
        }
        advence();

        return {TokenType::STRING, tk, curent_line, curent_column};
    } else {
        Token t = {TokenType::UNKNOWN, std::string(1,curent_c), curent_line, curent_column};
        advence();
        return t;
    }
}

// Get all tokens
std::vector<Token>
Lexar::get_all_tokens() {
    std::vector<Token> r;
    auto tk = get_the_next_token();
    r.push_back(tk);

    while (tk.Type != TokenType::END_CODE) {
        tk = get_the_next_token();
        r.push_back(tk);
    }

    return r;
}
