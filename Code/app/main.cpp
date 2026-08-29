// ==================================================================
// Include neccessary headers
// ==================================================================

// == Libs ==
#include <iostream>
#include <cctype>
#include <variant>
#include <string>
#include <vector>
#include <unordered_map>

// == Locals ==


// ==================================================================
// Enums
// ==================================================================
enum TokenType {
    IDENTIFIER,
    TYPE,
    STRING,
    INTEGER,
    FLOAT,
    BOOLEAN,
    COLON,
    LESS_THAN,
    GREATER_THAN,
    NEWLINE,
    END_CODE,
    UNKNOWN
};

// ==================================================================
// Structs
// ==================================================================
struct Token {
    TokenType Type;
    std::string value;
    void print() {
        std::cout << "{" << Type << " - " << value << "}" << std::endl << std::flush;
    }
};

// ==================================================================
// Lexar class
// ==================================================================
class Lexar {
    private:
        int pos = 0;
        std::string code;
        char curent_c;
        bool code_ended = false;
    public:
        Lexar(std::string& code);
        void advence();
        void escape_spaces();
        Token get_the_next_token();
        std::vector<Token> get_all_tokens();
};

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
    } else 
        code_ended = true;
}

// Wolk on spaces
void 
Lexar::escape_spaces() {
    
    if (code_ended == true) 
        return;
    
    while ((std::isspace(curent_c) || curent_c == '\n' || curent_c == '\t') && !code_ended) {
        advence();
    }
}

// get the next token function
Token
Lexar::get_the_next_token() {
    escape_spaces();

    // If there is not any char
    if (code_ended == true) {
        return {END_CODE, ""};

    // If char is < or >
    } else if (curent_c == '<' || curent_c == '>') {
        Token t = {curent_c=='<'? LESS_THAN:GREATER_THAN, std::string(1,curent_c)};
        advence();
        return t;

    // If char is :
    } else if (curent_c == ':') {
        Token t = {COLON ,std::string(1,curent_c)};
        advence();
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
            return {FLOAT, tk};
        else 
            return {INTEGER, tk};

    // If curent char is letter
    } else if (std::isalpha(curent_c) || curent_c == '_') {
        std::string tk;

        do {
            tk += curent_c;
            advence(); 
        } while ((std::isdigit(curent_c) || std::isalpha(curent_c) || curent_c == '_') && !code_ended);

        TokenType tk_t = IDENTIFIER;

        if (
            tk == "str" ||
            tk == "int" ||
            tk == "float" ||
            tk == "bool"
        )
            tk_t = TYPE;

        if (
            tk == "True" ||
            tk == "False"
        ) 
            tk_t = BOOLEAN;
        
        return {tk_t, tk};

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

        return {STRING, tk};
    } else {
        Token t = {UNKNOWN, std::string(1,curent_c)};
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

    while (tk.Type != END_CODE) {
        tk = get_the_next_token();
        r.push_back(tk);
    }

    return r;

}

// ==================================================================
// Entry point function
// ==================================================================
int main () {
    std::string code = R"CODE(
    CALL print 
        value<str>:"Hellow n****"
    CALL sum
        _1st<int>:1.5
        _2nd<int>: 5
    )CODE";

    std::cout << "Start Lexar" << std::endl << std::flush;

    Lexar l(code);
    auto all_t = l.get_all_tokens();
    for (auto& tk: all_t) {
        tk.print();
    }

    std::cout << "end Lexar" << std::endl << std::flush;

    return 0;
}