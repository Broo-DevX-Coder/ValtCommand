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
        if (curent_token_.Type == TokenType::END_CODE)
            code_ended_ = true;
        std::cout << curent_token_.value << std::endl << std::flush;
        
    } else {
        code_ended_ = true;
    }
}

// Verifi the next token type
ReturnResult<bool>
Parser::next_match(
    TokenType tt_,
    std::string comment_
) {
    advence();
    if (curent_token_.Type == tt_ && !code_ended_)
        return {"",true,true};

    return {
        Errors::SyntaxError(
            code_ended_?"(end of code!)":curent_token_.value,
            curent_token_.line,
            curent_token_.column,
            comment_
        ).msg,
        false,false
    };
}

// Verifi the curent token type
ReturnResult<bool>
Parser::match(
    TokenType tt_,
    std::string comment_
) {
    if (curent_token_.Type == tt_ && !code_ended_)
        return {"",true,true};

    return {
        Errors::SyntaxError(
            code_ended_?"(end of code!)":curent_token_.value,
            curent_token_.line,
            curent_token_.column,
            comment_
        ).msg,
        false,false
    };
}

// Get the next node
ReturnResult<Parser::Node>
Parser::get_next_node() {

    // set match result var
    ReturnResult<bool> mt;

    if (curent_token_.Type == TokenType::KEY_WORD) {

        if (curent_token_.value == "CALL") {

            mt = next_match(TokenType::IDENTIFIER, "What is the function name ?? \n");  
            if (!mt.success)
                return {mt.Message,false,nullptr};
            auto func_token = curent_token_;
            advence();

            FunctionCallNode::ArgsT arg_list;

            while (curent_token_.Type != TokenType::NEWLINE && !code_ended_) {

                // get arg name
                mt = match(TokenType::IDENTIFIER, "Forgot naming function's argument \n");
                if (!mt.success)
                    return {mt.Message,false,nullptr};
                auto arg_name = curent_token_.value;            

                // get arg type
                mt = next_match(TokenType::LESS_THAN, "Forgot adding `<` befor type name \n");
                if (!mt.success)
                    return {mt.Message,false,nullptr};
                mt = next_match(TokenType::TYPE, "Forgot setting type for argument \n");
                if (!mt.success)
                    return {mt.Message,false,nullptr};
                auto arg_type = curent_token_.value;
                mt = next_match(TokenType::GREATER_THAN, "Forgot adding `>` aftre type name \n");
                if (!mt.success)
                    return {mt.Message,false,nullptr};

                // get arg value
                mt = next_match(TokenType::COLON, "Forgot adding `:` to set argument value \n");
                if (!mt.success)
                    return {mt.Message,false,nullptr};
                advence();
                {
                    auto tt = curent_token_.Type;
                    if(!(
                        tt == TokenType::STRING ||
                        tt == TokenType::INTEGER ||
                        tt == TokenType::FLOAT ||
                        tt == TokenType::BOOLEAN ||
                        tt == TokenType::KEY_WORD
                    ))
                        return {
                        Errors::SyntaxError(
                            code_ended_?"(end of code!)":curent_token_.value,
                            curent_token_.line,
                            curent_token_.column,
                            "Forgot adding argument value"
                        ).msg,
                        false,nullptr};
                }

                std::unique_ptr<ASTNode> arg_value;
                if (
                    curent_token_.Type == TokenType::STRING ||
                    curent_token_.Type == TokenType::INTEGER ||
                    curent_token_.Type == TokenType::FLOAT ||
                    curent_token_.Type == TokenType::BOOLEAN 
                ) {
                    if (arg_type == "str") {
                        arg_value = std::make_unique<StringValueNode>(curent_token_);
                    } else if (arg_type == "int") {
                        arg_value = std::make_unique<NumberValueNode>(curent_token_,"int");
                    } else if (arg_type == "float") {
                        arg_value = std::make_unique<NumberValueNode>(curent_token_,"float");
                    } else if (arg_type == "boolean") {
                        arg_value = std::make_unique<BooleanValueNode>(curent_token_);
                    } 

                } else if (curent_token_.Type == TokenType::KEY_WORD) {
                    auto g = get_next_node();
                    if (!g.success)
                        return {g.Message,false,nullptr};
                    arg_value = std::move(g.value);
                }

                // Creat argument node
                auto arg = std::make_unique<FunctionCallArgumentNode>(
                    arg_name, arg_type, std::move(arg_value)
                );

                // Move the argument to args list
                arg_list.push_back(std::move(arg));

                advence();
            }
            
            mt = match(TokenType::NEWLINE, "Forgot END at the end of function call \n");
            if (!mt.success)
                return {mt.Message,false,nullptr};
            advence();

            return {
                "",
                true,
                std::make_unique<FunctionCallNode>(
                    func_token.value,
                    arg_list,
                    func_token
                )
            };
            
        }
    } 
    return {
        Errors::SyntaxError(
            code_ended_?"(end of code!)":curent_token_.value,
            curent_token_.line,
            curent_token_.column,
            code_ended_?"The code is empty":fmt::format("This keyword named {}",curent_token_.value)
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
        nt = get_next_node();
        if(!nt.success)
            return {nt.Message,false,nullptr};
        s_list.push_back(std::move(nt.value));
    } while (!code_ended_);

    // make module node and return them
    return {
        "",
        true,
        std::make_unique<ModuleNode>(
            s_list
        )
    };
}