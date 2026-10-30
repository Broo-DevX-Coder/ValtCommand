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
Parser::get_set_variable_node(const std::string& FilePath,
    bool is_const
) {
    ReturnResult<Token> consume_result; // The global cunsum return resut object

    // Jump on `SET`
    consume_result = consume(TokenType::KEY_WORD);
    if (!consume_result.success) {
        ReturnResult<Node> re = {consume_result.Message, false, nullptr};
        re.error_info = consume_result.error_info;
        return re;
    }

    // Get variable name
    auto var_name_t = consume(TokenType::IDENTIFIER);
    if (!var_name_t.success) {
        ReturnResult<Node> re = {var_name_t.Message, false, nullptr};
        re.error_info = var_name_t.error_info;
        return re;
    }

    if (check(TokenType::LESS_THAN)) {

        // Jump on `<`
        consume_result = consume(TokenType::LESS_THAN);
        if (!consume_result.success) {
            ReturnResult<Node> re = {consume_result.Message, false, nullptr};
            re.error_info = consume_result.error_info;
            return re;
        }

        // Get expected type of var
        auto var_type_t = consume(TokenType::TYPE);
        if (!var_type_t.success) {
            ReturnResult<Node> re = {var_type_t.Message, false, nullptr};
            re.error_info = var_type_t.error_info;
            return re;
        }

        // Jump on `>`
        consume_result = consume(TokenType::GREATER_THAN);
        if (!consume_result.success) {
            ReturnResult<Node> re = {consume_result.Message, false, nullptr};
            re.error_info = consume_result.error_info;
            return re;
        }

        // Jump on `=`
        consume_result = consume(TokenType::EQUAL);
        if (!consume_result.success) {
            ReturnResult<Node> re = {consume_result.Message, false, nullptr};
            re.error_info = consume_result.error_info;
            return re;
        }

        // Get the ASTNode of variable value
        auto var_value = get_expretion(FilePath);
        if (!var_value.success) {
            ReturnResult<Node> re = {var_value.Message, false, nullptr};
            re.error_info = var_value.error_info;
            return re;
        }

        const Token END_token = tokens_list_[pos - 1];
        return {
            "", true,
            std::make_unique<SetVariableNode>(FilePath, END_token, var_name_t.value,
                var_type_t.value,
                std::move(var_value.value),
                is_const
            )
        };

    } else if (!is_const) {

        // Jump on `=`
        consume_result = consume(TokenType::EQUAL);
        if (!consume_result.success) {
            ReturnResult<Node> re = {consume_result.Message, false, nullptr};
            re.error_info = consume_result.error_info;
            return re;
        }

        // Get the ASTNode of variable value
        auto var_value = get_expretion(FilePath);
        if (!var_value.success) {
            ReturnResult<Node> re = {var_value.Message, false, nullptr};
            re.error_info = var_value.error_info;
            return re;
        }

        const Token END_token = tokens_list_[pos - 1];
        return {
            "", true,
            std::make_unique<ResetVariableNode>(FilePath, END_token, var_name_t.value,
                std::move(var_value.value)
            )
        };
    }

    auto err = Errors::SyntaxError(
        var_name_t.value.value,
        var_name_t.value.line,
        var_name_t.value.column,
        var_name_t.value.value.size(),
        "Constant declarations require an explicit type"
    );

    ReturnResult<Node> re = {err.msg, false, nullptr};
    re.error_info = err.info;
    return re;
}

// ==================================================================
// Get the node that get the variable value from symbols table
// ==================================================================

ReturnResult<Parser::Node>
Parser::get_get_variable_node(const std::string& FilePath) {
    ReturnResult<Token> consume_result; // The global cunsum return resut object

    // Jump on `GET`
    consume_result = consume(TokenType::KEY_WORD);
    if (!consume_result.success) {
        ReturnResult<Node> re = {consume_result.Message, false, nullptr};
        re.error_info = consume_result.error_info;
        return re;
    }

    // Get variable name
    auto var_name_t = consume(TokenType::IDENTIFIER);
    if (!var_name_t.success) {
        ReturnResult<Node> re = {var_name_t.Message, false, nullptr};
        re.error_info = var_name_t.error_info;
        return re;
    }

    const Token END_token = var_name_t.value;
    return {
        "", true,
        std::make_unique<GetVariableNode>(FilePath, END_token, var_name_t.value)
    };
}
