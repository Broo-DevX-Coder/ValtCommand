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
Parser::get_if_statment_node() {
    ReturnResult<Token> consume_result; // The global cunsum return resut object

    // Jump on `IF`
    consume_result = consume(TokenType::KEY_WORD); 
    if (!consume_result.success) return {consume_result.Message,false,nullptr};

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
            consume_result.value.column    
        ).msg,false,nullptr};

    // Create statments list 
    IfStatmentsNode::Nodes_list statments;

    // Get all statments
    while (!check(TokenType::END_BLOCK)) {
        auto r = get_expretion();
        if (!r.success) return {r.Message,false,nullptr};
        statments.push_back(std::move(r.value));
    }

    // Jump on `END`
    consume_result = consume(TokenType::END_BLOCK); 
    if (!consume_result.success) return {consume_result.Message,false,nullptr};

    // Creat the IF statment node
    return {
        "",true,
        std::make_unique<IfStatmentsNode>(condition_FT,condition_N_r.value,statments)
    };
};