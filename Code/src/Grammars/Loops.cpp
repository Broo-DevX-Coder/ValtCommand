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
    if (!loop_first_token_r.success) return {loop_first_token_r.Message,false,nullptr};

    // Get condition node
    auto condition_FT = curent();
    auto condition_N_r = get_expretion();
    if (!condition_N_r.success) return {condition_N_r.Message,false,nullptr};

    // Jump on `THEN`
    consume_result = consume(TokenType::KEY_WORD); 
    if (!consume_result.success) return {consume_result.Message,false,nullptr};
    if (consume_result.value.value != "THEN") 
        return {Errors::SyntaxError(
            consume_result.value.Type==TokenType::END_CODE?"(end of code!)":consume_result.value.value,
            consume_result.value.line,
            consume_result.value.column,
            "Forgot to add `THEN` after the condition"
        ).msg,false,nullptr};

    // Create statments lists
    WhileLoopsNode::NodesL statments;

    // Get all statments
    while (!check(TokenType::END_BLOCK) && !isAsEnd()) {
        auto r = get_expretion();
        if (!r.success) return {r.Message,false,nullptr};
        statments.push_back(std::move(r.value));
    }

    // Raise error if code ended
    if (isAsEnd())
        return {"SyntaxError: forgot to add END in after of IF Satament",false,nullptr};    

    // Jump on `END`
    consume_result = consume(TokenType::END_BLOCK); 
    if (!consume_result.success) return {consume_result.Message,false,nullptr};

    return {"",true,
        std::make_unique<WhileLoopsNode>(loop_first_token_r.value, condition_N_r.value, statments)
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
    if (!loop_first_token_r.success) return {loop_first_token_r.Message,false,nullptr};

    // Get Initialyzation
    auto initialyzation_FT = curent();
    auto initialyzation_node_r = get_expretion();
    if (!initialyzation_node_r.success) return {initialyzation_node_r.Message,false,nullptr};

    // Jump on `;`
    consume_result = consume(TokenType::SEMICOLON);
    if (!consume_result.success) return {consume_result.Message,false,nullptr};

    // Get COndition
    auto condition_FT = curent();
    auto condition_node_r = get_expretion();
    if (!condition_node_r.success) return {condition_node_r.Message,false,nullptr};

    // Jump on `;`
    consume_result = consume(TokenType::SEMICOLON);
    if (!consume_result.success) return {consume_result.Message,false,nullptr};

    // Get Update
    auto update_FT = curent();
    auto update_node_r = get_expretion();
    if (!update_node_r.success) return {update_node_r.Message,false,nullptr};

    // Jump on `THEN`
    consume_result = consume(TokenType::KEY_WORD); 
    if (!consume_result.success) return {consume_result.Message,false,nullptr};
    if (consume_result.value.value != "THEN") 
        return {Errors::SyntaxError(
            consume_result.value.Type==TokenType::END_CODE?"(end of code!)":consume_result.value.value,
            consume_result.value.line,
            consume_result.value.column,
            "Forgot to add `THEN` after the condition"
        ).msg,false,nullptr};

    // Create statments lists
    ForLoopsNode::NodesL statments;

    // Get all statments
    while (!check(TokenType::END_BLOCK) && !isAsEnd()) {
        auto r = get_expretion();
        if (!r.success) return {r.Message,false,nullptr};
        statments.push_back(std::move(r.value));
    }

    // Raise error if code ended
    if (isAsEnd())
        return {"SyntaxError: forgot to add END in after of IF Satament",false,nullptr};    

    // Jump on `END`
    consume_result = consume(TokenType::END_BLOCK); 
    if (!consume_result.success) return {consume_result.Message,false,nullptr};

    // Create components vars
    ForLoopsNode::Component Init = {initialyzation_FT,std::move(initialyzation_node_r.value)};
    ForLoopsNode::Component Cond = {condition_FT,std::move(condition_node_r.value)};
    ForLoopsNode::Component Updt = {update_FT,std::move(update_node_r.value)};

    return {"",true,
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
    consume(TokenType::KEY_WORD);
    return {"",true,std::make_unique<BreakLoopNode>()};
} 

// ==================================================================
// Parse `CONTINUE`
// ==================================================================

// Get the node of continue a loop `CONTINUE`
ReturnResult<Parser::Node> 
Parser::get_continue_loop_node() {
    consume(TokenType::KEY_WORD);
    return {"",true,std::make_unique<ContinueLoopNode>()};
} 
