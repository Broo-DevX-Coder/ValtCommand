// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Parser.hpp"

// Nodes
#include "ASTNodes/ValueNode.hpp"


// ==================================================================
// Get the pure value's node
// ==================================================================

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