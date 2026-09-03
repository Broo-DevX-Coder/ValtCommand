// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Parser.hpp"
#include "ASTNodes/ValueNode.hpp"
#include "Errors.hpp"

#include <iostream>

// ==================================================================
// Parses's functions
// ==================================================================

// Constructure
Parser::Parser(
    TList& tl_
): tokens_list_(std::move(tl_)) {

    if (tokens_list_.empty()) {
        is_code_ended_ = true;
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
        if (curent_token_.Type == TokenType::END_CODE)
            is_code_ended_ = true;
        //std::cout << curent_token_.value << std::endl << std::flush;
        
    } else {
        is_code_ended_ = true;
        curent_token_ = tokens_list_[tokens_list_.size()-1];
        curent_token_.value = "(end of code!)";
    }
}

// Get the curent token
const Token& 
Parser::curent() {
    return curent_token_;
}

// Get the next <offset> token
const Token& 
Parser::peek(
    size_t offset
) {
    if (pos+offset < tokens_list_.size())
        return tokens_list_[pos+offset];
    return tokens_list_[tokens_list_.size()-1];
}

// Check the curent token
bool 
Parser::check(TokenType type) {
    return curent().Type == type;
}

// Check the curent token and move to next
ReturnResult<Token> 
Parser::consume(TokenType type) {
    if (curent().Type != type)
        return {
            Errors::SyntaxError(
                isAsEnd()?"(end of code!)":curent_token_.value,
                curent_token_.line,
                curent_token_.column
            ).msg,
            false,
            curent()
        };

    auto t = curent();
    advence();
    
    return {"",true,t};
}

// Is the code as end
bool 
Parser::isAsEnd() {
    return is_code_ended_;
}

// Grammar ==============================================

// Get the pure value's node
ReturnResult<Parser::Node> 
Parser::get_value_node() {
    if (curent().Type == TokenType::STRING) {
        auto value_t = consume(TokenType::STRING);
        return {"",true,std::make_unique<StringValueNode>(value_t.value)};

    // If the curent type is intiger value
    } else if (curent().Type == TokenType::INTEGER) {
        auto value_t = consume(TokenType::INTEGER);
        return {"",true,std::make_unique<NumberValueNode>(value_t.value,"int")};

    // If the curent type is float value
    } else if (curent().Type == TokenType::FLOAT) {
        auto value_t = consume(TokenType::FLOAT);
        return {"",true,std::make_unique<NumberValueNode>(value_t.value,"float")};
    } 

    // If the curent type is boolean value
    auto value_t = consume(TokenType::BOOLEAN);
    return {"",true,std::make_unique<BooleanValueNode>(value_t.value)};

}

// When found CALL
ReturnResult<Parser::Node>
Parser::get_functioncall_node() {
    ReturnResult<Token> consume_result; // The global cunsum return resut object

    // Jump on `CALL`
    consume_result = consume(TokenType::KEY_WORD); 
    if (!consume_result.success) return {consume_result.Message,false,nullptr};

    // Get function name
    auto func_name_t = consume(TokenType::IDENTIFIER);
    if (!func_name_t.success) return {func_name_t.Message,false,nullptr};

    FunctionCallNode::ArgsT arg_list;
    while (!check(TokenType::END_BLOCK) && !isAsEnd()) {
        
        // Get argument name
        auto arg_name_t = consume(TokenType::IDENTIFIER);
        if (!arg_name_t.success) return {arg_name_t.Message,false,nullptr};

        // Jump on `<`
        consume_result = consume(TokenType::LESS_THAN); 
        if (!consume_result.success) return {consume_result.Message,false,nullptr};

        // Get expected type of arg
        auto arg_type_t = consume(TokenType::TYPE);
        if (!arg_type_t.success) return {arg_type_t.Message,false,nullptr};

        // Jump on `>`
        consume_result = consume(TokenType::GREATER_THAN); 
        if (!consume_result.success) return {consume_result.Message,false,nullptr};

        // Jump on `:`
        consume_result = consume(TokenType::COLON); 
        if (!consume_result.success) return {consume_result.Message,false,nullptr};

        // Get arg value node
        auto arg_value = get_node();
        if (!arg_value.success) return {arg_value.Message,false,nullptr};

        // Add the argument to args list
        arg_list.push_back(std::make_unique<FunctionCallArgumentNode>(
            arg_name_t.value.value,
            arg_type_t.value,
            std::move(arg_value.value)
        ));
    }

    // Jump on `CALL`
    consume_result = consume(TokenType::END_BLOCK); 
    if (!consume_result.success) {
        return {consume_result.Message,false,nullptr};
    }

    // Create FunctionCallNode and get the value
    return {"",true,std::make_unique<FunctionCallNode>(
        func_name_t.value.value,
        arg_list,
        func_name_t.value
    )};

}

// Global function of nodes
ReturnResult<Parser::Node>
Parser::get_node() {

    // set match result var
    ReturnResult<bool> mt;

    // If the curent token is key word  (like CALL and SET)
    if (curent().Type == TokenType::KEY_WORD) {
        if (curent().value == "CALL") {
            return get_functioncall_node();
        }

    // If the curent type is pure value
    } else if (
        curent().Type == TokenType::STRING ||
        curent().Type == TokenType::INTEGER ||
        curent().Type == TokenType::FLOAT ||
        curent().Type == TokenType::BOOLEAN
    ) {
        return get_value_node();

    }

    return {
        Errors::SyntaxError(
            curent().value,
            curent().line,
            curent().column
        ).msg,
        false,
        nullptr
    };
}

// Get clear module node
ReturnResult<Parser::PNode>
Parser::get_module_node() {

    ModuleNode::StatmentsT s_list;
    ReturnResult<Parser::Node> nt;

    do {
        nt = get_node();
        if(!nt.success){
            return {nt.Message,false,nullptr};
        }
        s_list.push_back(std::move(nt.value));
    } while (!isAsEnd());

    // make module node and return them
    return {
        "",
        true,
        std::make_unique<ModuleNode>(
            s_list
        )
    };
}