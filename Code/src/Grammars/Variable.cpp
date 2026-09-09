// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Parser.hpp"
#include "Errors.hpp"

// Nodes
#include "ASTNodes/VariableNode.hpp"

// ==================================================================
// Get Set Variable Node
// ==================================================================

ReturnResult<Parser::Node>
Parser::get_set_variable_node(
    bool is_const
) {
    ReturnResult<Token> consume_result; // The global cunsum return resut object

    // Jump on `SET`
    consume_result = consume(TokenType::KEY_WORD); 
    if (!consume_result.success) return {consume_result.Message,false,nullptr};

    // Get variable name
    auto var_name_t = consume(TokenType::IDENTIFIER);
    if (!var_name_t.success) return {var_name_t.Message,false,nullptr};

    if (check(TokenType::LESS_THAN)) {
        
        // Jump on `<`
        consume_result = consume(TokenType::LESS_THAN); 
        if (!consume_result.success) return {consume_result.Message,false,nullptr};

        // Get expected type of var
        auto var_type_t = consume(TokenType::TYPE);
        if (!var_type_t.success) return {var_type_t.Message,false,nullptr};

        // Jump on `>`
        consume_result = consume(TokenType::GREATER_THAN); 
        if (!consume_result.success) return {consume_result.Message,false,nullptr};

        // Jump on `=`
        consume_result = consume(TokenType::EQUAL); 
        if (!consume_result.success) return {consume_result.Message,false,nullptr};

        // Get ths ASTNode of variable value
        auto var_value = get_expretion();
        if (!var_value.success) return {var_value.Message,false,nullptr};

        return {
            "",true,
            std::make_unique<SetVariableNode>(
                var_name_t.value,
                var_type_t.value,
                std::move(var_value.value),
                is_const
            )
        };

    } else if (!is_const) {

        // Jump on `=`
        consume_result = consume(TokenType::EQUAL); 
        if (!consume_result.success) return {consume_result.Message,false,nullptr};

        // Get ths ASTNode of variable value
        auto var_value = get_expretion();
        if (!var_value.success) return {var_value.Message,false,nullptr};

        return {
            "",true,
            std::make_unique<ResetVariableNode>(
                var_name_t.value,
                std::move(var_value.value)
            )
        };
    }

    return {Errors::SyntaxError(
        var_name_t.value.value,
        var_name_t.value.line,
        var_name_t.value.column,
        "Constant declarations require an explicit type"
    ).msg,false,nullptr};

}

// ==================================================================
// Get the node that get the variable value from symbols table
// ==================================================================

ReturnResult<Parser::Node> 
Parser::get_get_variable_node() {
    ReturnResult<Token> consume_result; // The global cunsum return resut object

    // Jump on `GET`
    consume_result = consume(TokenType::KEY_WORD); 
    if (!consume_result.success) return {consume_result.Message,false,nullptr};

    // Get variable name
    auto var_name_t = consume(TokenType::IDENTIFIER);
    if (!var_name_t.success) return {var_name_t.Message,false,nullptr};

    return {
        "",true,
        std::make_unique<GetVariableNode>(var_name_t.value),
    };
}