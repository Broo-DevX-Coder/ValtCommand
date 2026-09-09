// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "Parser.hpp"
#include "Errors.hpp"

// Nodes



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


// Get clear module node ==================================================
ReturnResult<Parser::PNode>
Parser::get_module_node() {

    ModuleNode::StatmentsT s_list;
    ReturnResult<Parser::Node> nt;

    do {
        nt = get_expretion();
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