// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Parser.hpp"

// ==================================================================
// Parses's functions
// ==================================================================

// Constructure
Parser::Parser(
    TList& tl_
): tokens_list_(std::move(tl_)) {

    if (tokens_list_.empty()) {
        code_ended_ = true;
        return;
    }
    curent_token_ = tokens_list_[0];
}

// Go to the next token
void
Parser::advence() {
    pos++;
    if (pos < tokens_list_.size()) {
        curent_token_ = tokens_list_[pos];
        if (curent_token_.Type == END_CODE)
            code_ended_ = true;
    } else {
        code_ended_ = true;
    }
}

// Verifi the next token type
bool
Parser::next_match(
    TokenType tt_
) {
    advence();
    if (curent_token_.Type == tt_ && !code_ended_)
        return true;

    throw std::runtime_error(
        fmt::format("Unexpected token {}", curent_token_.value)
    );
}

// Verifi the curent token type
bool
Parser::match(
    TokenType tt_
) {
    if (curent_token_.Type == tt_ && !code_ended_)
        return true;

    throw std::runtime_error(
        fmt::format("Unexpected token {}", curent_token_.value)
    );
}

// Get the next node
Parser::Node
Parser::get_next_node() {
    if (curent_token_.Type == KEY_WORD) {

        if (curent_token_.value == "CALL") {

            next_match(IDENTIFIER);
            auto func_name = curent_token_.value;
            advence();

            FunctionCallNode::ArgsT arg_list;

            while (curent_token_.Type != NEWLINE && !code_ended_) {

                // get arg name
                match(IDENTIFIER);
                auto arg_name = curent_token_.value;            

                // get arg type
                next_match(LESS_THAN);
                next_match(TYPE);
                auto arg_type = curent_token_.value;
                next_match(GREATER_THAN);

                // get arg value
                next_match(COLON);
                advence();
                auto arg_value = curent_token_.value;

                // Make the argument node
                auto arg = std::make_unique<FunctionCallArgumentNode>(
                    arg_name, arg_type, arg_value
                );

                // Move the argument to args list
                arg_list.push_back(std::move(arg));

                advence();
            }
            
            advence();

            return std::make_unique<FunctionCallNode>(
                func_name,
                arg_list
            );
        }
    } else {
        throw std::runtime_error(
            fmt::format("Unexpected token {}", curent_token_.value)
        );
    }
}

// Get clear program node
Parser::PNode
Parser::get_program_node() {

    ProgramNode::StatmentsT s_list;

    while (!code_ended_) {
        s_list.push_back(get_next_node());
    }

    // make program node and return them
    return  std::make_unique<ProgramNode>(
        s_list
    );

}