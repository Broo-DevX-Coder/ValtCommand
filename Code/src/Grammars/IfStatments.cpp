// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Parser.hpp"
#include "Errors.hpp"

// Nodes
#include "ASTNodes/IfElseNode.hpp"

// ==================================================================
// Parse `IF`
// ==================================================================

// Get the node of `IF` keyword (if statment)
ReturnResult<Parser::Node>
Parser::get_if_statment_node(const std::string& FilePath) {
    ReturnResult<Token> consume_result; // The global cunsum return resut object
    Token END_token;

    // Jump on `IF`
    consume_result = consume(TokenType::KEY_WORD);
    if (!consume_result.success) {
        ReturnResult<Node> re =  {consume_result.Message,false,nullptr};
        re.error_info = consume_result.error_info;
        return re;
    }

    // Get condition node
    auto condition_FT = curent();
    auto condition_N_r = get_expretion(FilePath);
    if (!condition_N_r.success) return condition_N_r;

    // Jump on `THEN`
    consume_result = consume(TokenType::KEY_WORD);
    if (!consume_result.success) {
        ReturnResult<Node> re =  {consume_result.Message,false,nullptr};
        re.error_info = consume_result.error_info;
        return re;
    }
    if (consume_result.value.value != "THEN") {
        auto err = Errors::SyntaxError(
            consume_result.value.Type==TokenType::END_CODE?"(end of code!)":consume_result.value.value,
            consume_result.value.line,
            consume_result.value.column,
            consume_result.value.value.size(),
            "Forgot to add `THEN` after the condition"
        );
        ReturnResult<Node> re = {err.msg,false,nullptr};
        re.error_info = err.info;
        return re;
    }

    // Create statments lists
    IfStatmentsNode::Nodes_list statments;
    IfStatmentsNode::Nodes_list else_statments;

    // Get all statments
    while (!check(TokenType::END_BLOCK) && !isAsEnd() && curent().value != "ELSE") {
        auto r = get_expretion(FilePath);
        if (!r.success) return r;
        statments.push_back(std::move(r.value));
    }

    // Else statments
    if (curent().value == "ELSE") {

        // Jump on `ELSE`
        consume(TokenType::KEY_WORD);

        // Jump on `THEN`
        consume_result = consume(TokenType::KEY_WORD);
        if (!consume_result.success) {
            ReturnResult<Node> re =  {consume_result.Message,false,nullptr};
            re.error_info = consume_result.error_info;
            return re;
        }
        if (consume_result.value.value != "THEN") {
            auto err = Errors::SyntaxError(
                consume_result.value.Type==TokenType::END_CODE?"(end of code!)":consume_result.value.value,
                consume_result.value.line,
                consume_result.value.column,
                consume_result.value.value.size(),
                "Forgot to add `THEN` after the condition"
            );
            ReturnResult<Node> re = {err.msg,false,nullptr};
            re.error_info = err.info;
            return re;
        }

        while (!check(TokenType::END_BLOCK) && !isAsEnd()) {
            auto r = get_expretion(FilePath);
            if (!r.success) return r;
            else_statments.push_back(std::move(r.value));
        }

        // Raise error if code ended
        if (isAsEnd())
            return {"SyntaxError: forgot to add END in after of ELSE Satament",false,nullptr};

        // Jump on `END`
        consume_result = consume(TokenType::END_BLOCK);
        if (!consume_result.success) {
            ReturnResult<Node> re =  {consume_result.Message,false,nullptr};
            re.error_info = consume_result.error_info;
            return re;
        }
        END_token = consume_result.value;

    // If there is not any else statment
    } else {
        // Raise error if code ended
        if (isAsEnd())
            return {"SyntaxError: forgot to add END in after of IF Satament",false,nullptr};

        // Jump on `END`
        consume_result = consume(TokenType::END_BLOCK);
        if (!consume_result.success) {
            ReturnResult<Node> re =  {consume_result.Message,false,nullptr};
            re.error_info = consume_result.error_info;
            return re;
        }
        END_token = consume_result.value;
    }

    // Creat the IF statment node
    return {
        "",true,
        std::make_unique<IfStatmentsNode>(FilePath, END_token, condition_FT,condition_N_r.value,statments,else_statments)
    };
};