// ==================================================================
// Include neccessary headers
// ==================================================================

// == Libs ==
#include <fmt/format.h>

// == Local ==
#include "Errors.hpp"

// ==================================================================
// Namespace
// ==================================================================

// ============== TypeError class constructure ==============
Errors::SyntaxError::SyntaxError(
    std::string token, 
    int line, 
    int column, 
    std::string comment
) {
    msg = fmt::format(
        "SyntaxError: expected '{}' at line:{} ,column:{}",
        token, line, column
    );
    if (!comment.empty())
        msg+=fmt::format("\n{}",comment);
}

// ============== TypeError class constructure ==============
Errors::TypeError::TypeError(
    std::string expected, 
    std::string got, 
    int line, 
    int column, 
    std::string comment
) {
    msg = fmt::format(
        "TypeError: expected '{}', got '{}', at line:{} ,column:{}",
        expected, got, line, column
    );
    if (!comment.empty())
        msg+=fmt::format("\n{}",comment);
}

// ============== TypeError class constructure ==============
Errors::NameError::NameError(
    std::string name, 
    int line, 
    int column, 
    std::string comment
) {
    msg = fmt::format(
        "NameError: '{}' is not defined at line:{} ,column:{}",
        name, line, column
    );
    if (!comment.empty())
        msg+=fmt::format("\n{}",comment);
}