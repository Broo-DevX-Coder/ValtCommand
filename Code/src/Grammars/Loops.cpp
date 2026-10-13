// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Parser.hpp"
#include "Errors.hpp"

// Nodes
#include "ASTNodes/LoopsNodes.hpp"

// ==================================================================
// Parse `WHILE`
// ==================================================================

// Get the node of While loop `WHILE`
ReturnResult<Parser::Node>
Parser::get_while_loop_node() {
    ReturnResult<Token> consume_result; // The global cunsum return resut object

    // Jump on `WHILE`
    auto loop_first_token_r = consume(TokenType::KEY_WORD);
    if (!loop_first_token_r.success) {
        ReturnResult<Node> re = {
            loop_first_token_r.Message,
            false,
            nullptr
        };
        re.error_info = loop_first_token_r.error_info;
        return re;
    }

    // Get condition node
    auto condition_FT = curent();
    auto condition_N_r = get_expretion();
    if (!condition_N_r.success) {
        ReturnResult<Node> re = {
            condition_N_r.Message,
            false,
            nullptr
        };
        re.error_info = condition_N_r.error_info;
        return re;
    }

    // Jump on `THEN`
    consume_result = consume(TokenType::KEY_WORD);
    if (!consume_result.success) {
        ReturnResult<Node> re = {
            consume_result.Message,
            false,
            nullptr
        };
        re.error_info = consume_result.error_info;
        return re;
    }

    if (consume_result.value.value != "THEN") {
        auto err = Errors::SyntaxError(
            consume_result.value.Type == TokenType::END_CODE
                ? "(end of code!)"
                : consume_result.value.value,
            consume_result.value.line,
            consume_result.value.column,
            consume_result.value.value.size(),
            "Forgot to add `THEN` after the condition"
        );

        ReturnResult<Node> re = {
            err.msg,
            false,
            nullptr
        };
        re.error_info = err.info;
        return re;
    }

    // Create statements list
    WhileLoopsNode::NodesL statments;

    // Get all statements
    while (!check(TokenType::END_BLOCK) && !isAsEnd()) {
        auto r = get_expretion();

        if (!r.success) {
            ReturnResult<Node> re = {
                r.Message,
                false,
                nullptr
            };
            re.error_info = r.error_info;
            return re;
        }

        statments.push_back(std::move(r.value));
    }

    // Raise error if code ended
    if (isAsEnd()) {
        return {
            "SyntaxError: forgot to add END in after of IF Satament",
            false,
            nullptr
        };
    }

    // Jump on `END`
    consume_result = consume(TokenType::END_BLOCK);
    if (!consume_result.success) {
        ReturnResult<Node> re = {
            consume_result.Message,
            false,
            nullptr
        };
        re.error_info = consume_result.error_info;
        return re;
    }

    return {
        "",
        true,
        std::make_unique<WhileLoopsNode>(
            loop_first_token_r.value,
            condition_N_r.value,
            statments
        )
    };
}

// ==================================================================
// Parse `FOR`
// ==================================================================

// Get the node of for loop `FOR`
ReturnResult<Parser::Node>
Parser::get_for_loop_node() {
    ReturnResult<Token> consume_result; // The global cunsum return resut object

    // Jump on `FOR`
    auto loop_first_token_r = consume(TokenType::KEY_WORD);
    if (!loop_first_token_r.success) {
        ReturnResult<Node> re = {
            loop_first_token_r.Message,
            false,
            nullptr
        };
        re.error_info = loop_first_token_r.error_info;
        return re;
    }

    // Get Initialization
    auto initialyzation_FT = curent();
    auto initialyzation_node_r = get_expretion();

    if (!initialyzation_node_r.success) {
        ReturnResult<Node> re = {
            initialyzation_node_r.Message,
            false,
            nullptr
        };
        re.error_info = initialyzation_node_r.error_info;
        return re;
    }

    // Jump on `;`
    consume_result = consume(TokenType::SEMICOLON);
    if (!consume_result.success) {
        ReturnResult<Node> re = {
            consume_result.Message,
            false,
            nullptr
        };
        re.error_info = consume_result.error_info;
        return re;
    }

    // Get condition
    auto condition_FT = curent();
    auto condition_node_r = get_expretion();

    if (!condition_node_r.success) {
        ReturnResult<Node> re = {
            condition_node_r.Message,
            false,
            nullptr
        };
        re.error_info = condition_node_r.error_info;
        return re;
    }

    // Jump on `;`
    consume_result = consume(TokenType::SEMICOLON);
    if (!consume_result.success) {
        ReturnResult<Node> re = {
            consume_result.Message,
            false,
            nullptr
        };
        re.error_info = consume_result.error_info;
        return re;
    }

    // Get update
    auto update_FT = curent();
    auto update_node_r = get_expretion();

    if (!update_node_r.success) {
        ReturnResult<Node> re = {
            update_node_r.Message,
            false,
            nullptr
        };
        re.error_info = update_node_r.error_info;
        return re;
    }

    // Jump on `THEN`
    consume_result = consume(TokenType::KEY_WORD);
    if (!consume_result.success) {
        ReturnResult<Node> re = {
            consume_result.Message,
            false,
            nullptr
        };
        re.error_info = consume_result.error_info;
        return re;
    }

    if (consume_result.value.value != "THEN") {
        auto err = Errors::SyntaxError(
            consume_result.value.Type == TokenType::END_CODE
                ? "(end of code!)"
                : consume_result.value.value,
            consume_result.value.line,
            consume_result.value.column,
            consume_result.value.value.size(),
            "Forgot to add `THEN` after the condition"
        );

        ReturnResult<Node> re = {
            err.msg,
            false,
            nullptr
        };
        re.error_info = err.info;
        return re;
    }

    // Create statements list
    ForLoopsNode::NodesL statments;

    // Get all statements
    while (!check(TokenType::END_BLOCK) && !isAsEnd()) {
        auto r = get_expretion();

        if (!r.success) {
            ReturnResult<Node> re = {
                r.Message,
                false,
                nullptr
            };
            re.error_info = r.error_info;
            return re;
        }

        statments.push_back(std::move(r.value));
    }

    // Raise error if code ended
    if (isAsEnd()) {
        return {
            "SyntaxError: forgot to add END in after of IF Satament",
            false,
            nullptr
        };
    }

    // Jump on `END`
    consume_result = consume(TokenType::END_BLOCK);
    if (!consume_result.success) {
        ReturnResult<Node> re = {
            consume_result.Message,
            false,
            nullptr
        };
        re.error_info = consume_result.error_info;
        return re;
    }

    // Create component variables
    ForLoopsNode::Component Init = {
        initialyzation_FT,
        std::move(initialyzation_node_r.value)
    };

    ForLoopsNode::Component Cond = {
        condition_FT,
        std::move(condition_node_r.value)
    };

    ForLoopsNode::Component Updt = {
        update_FT,
        std::move(update_node_r.value)
    };

    return {
        "",
        true,
        std::make_unique<ForLoopsNode>(
            Init,
            Cond,
            Updt,
            statments
        )
    };
}

// ==================================================================
// Parse `BREAK`
// ==================================================================

// Get the node of break a loop `BREAK`
ReturnResult<Parser::Node>
Parser::get_break_loop_node() {
    auto consume_result = consume(TokenType::KEY_WORD);

    if (!consume_result.success) {
        ReturnResult<Node> re = {
            consume_result.Message,
            false,
            nullptr
        };
        re.error_info = consume_result.error_info;
        return re;
    }

    return {
        "",
        true,
        std::make_unique<BreakLoopNode>(consume_result.value)
    };
}

// ==================================================================
// Parse `CONTINUE`
// ==================================================================

// Get the node of continue a loop `CONTINUE`
ReturnResult<Parser::Node>
Parser::get_continue_loop_node() {
    auto consume_result = consume(TokenType::KEY_WORD);

    if (!consume_result.success) {
        ReturnResult<Node> re = {
            consume_result.Message,
            false,
            nullptr
        };
        re.error_info = consume_result.error_info;
        return re;
    }

    return {
        "",
        true,
        std::make_unique<ContinueLoopNode>(consume_result.value)
    };
}