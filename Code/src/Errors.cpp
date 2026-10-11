// ==================================================================
// Include neccessary headers
// ==================================================================

// == Libs ==
#include <fmt/format.h>
#include <sstream>

// == Local ==
#include "Errors.hpp"
#include "utils.hpp"

// ==================================================================
// Errors formatter
// ==================================================================

// Errors formatter
std::string 
Errors::formatter(
    Errors::ErrorInfo info,
    const std::string& code
) {
    std::stringstream ss;

    // Put the type and the message of error
    ss << std::endl << info.type << ": " << info.message << "\n------------------------------------------\n";
    
    std::string line1;
    std::string line2;

    {
        auto lines = Utils::split_lines(code);
        line1 = (info.line-1 < lines.size() && info.line > 0) ? lines[info.line-1] : std::string((char)0x5f, 1);
        line2 = (info.line < lines.size() && info.line >= 0) ? lines[info.line] : std::string((char)0x5f, 1);
    }

    if (line1 != std::string((char)0x5f, 1)) {
        ss << info.line-1 << "-" << "|  " << line1 << '\n';
    } 
    if (line2 != std::string((char)0x5f, 1)) {
        ss << info.line << "-" << "|  " << line2 << "\n";

        for (size_t i=0; i < std::to_string(info.line).length(); i++) ss << "-";
        ss << "-|  ";

        for (size_t i=1; i<info.column; i++) 
            ss << ' ';
        for (size_t i=0; i<info.length; i++)
            ss << '^';

        if (!info.comment.empty()) {
            ss << "\n------------------------------------------\n" << info.comment << '\n';
        }
    }

    return ss.str();
}

// ==================================================================
// Errors types classes
// ==================================================================

// ============== SyntaxError class constructure ==============
Errors::SyntaxError::SyntaxError(
    std::string token, 
    size_t line, 
    size_t column, 
    size_t lenth,
    std::string comment
) {
    info = {
        "SyntaxError",
        fmt::format("unexpected token '{}'", token),
        comment, 
        line,
        column,
        lenth
    };

    msg = fmt::format(
        "{}: {} at line:{} ,column:{}",
        info.type, info.message, info.line, info.column
    );

    if (!info.comment.empty())
        msg+=fmt::format("\n{}",info.comment);
}

// ============== TypeError class constructure ==============
Errors::TypeError::TypeError(
    std::string expected, 
    std::string got, 
    size_t line, 
    size_t column, 
    size_t lenth,
    std::string comment
) {

    info = {
        "TypeError",
        fmt::format("expected '{}', got '{}'", expected, got),
        comment,
        line,
        column,
        lenth
    };

    msg = fmt::format(
        "{}: {} at line:{} ,column:{}",
        info.type, info.message, info.line, info.column
    );

    if (!info.comment.empty())
        msg+=fmt::format("\n{}",info.comment);
}

// ============== NameError class constructure ==============
Errors::NameError::NameError(
    std::string name, 
    size_t line, 
    size_t column, 
    size_t lenth,
    std::string comment
) {    
    info = {
        "NameError",
        fmt::format("'{}' is not defined", name),
        comment, 
        line,
        column,
        lenth
    };

    msg = fmt::format(
        "{}: {} at line:{} ,column:{}",
        info.type, info.message, info.line, info.column
    );

    if (!info.comment.empty())
        msg+=fmt::format("\n{}",info.comment);
}


// ============== ArgumentError class constructure ==============
Errors::ArgumentError::ArgumentError(
    std::string f_name,
    size_t l_, 
    size_t c_, 
    size_t len,
    std::string com_
):
function_name(f_name) {
    info.line = l_;
    info.column = c_;
    info.length = len;
    info.comment = com_;
    info.type = "ArgumentError";
}

// When the user places more arguments than expacted
std::string 
Errors::ArgumentError::too_many_arguments(
    size_t expected, 
    size_t placed
) {
    info.message = fmt::format("Too many arguments to function '{}'. Expected {}, but got {}", function_name, expected, placed);

    msg = fmt::format(
        "{}: {} at line:{} ,column:{}",
        info.type, info.message, info.line, info.column
    );

    if (!info.comment.empty())
        msg+=fmt::format("\n{}",info.comment);

    return msg;
}

// When user places unknown argument
std::string 
Errors::ArgumentError::unexpected_argument(
    std::string arg_name
) {
    info.message = fmt::format("Unexpected argument '{}' in function '{}' arguments.", arg_name, function_name);

    msg = fmt::format(
        "{}: {} at line:{} ,column:{}",
        info.type, info.message, info.line, info.column
    );

    if (!info.comment.empty())
        msg+=fmt::format("\n{}",info.comment);
        
    return msg;
}

std::string 
Errors::ArgumentError::unexpected_argument_type(
    std::string arg_name, 
    std::string expected_type, 
    std::string placed_type
) {
    info.message = fmt::format("Unexpected argument '{}' in function '{}' arguments. Type expected:{} but got:{}", arg_name, function_name, expected_type, placed_type);

    msg = fmt::format(
        "{}: {} at line:{} ,column:{}",
        info.type, info.message, info.line, info.column
    );

    if (!info.comment.empty())
        msg+=fmt::format("\n{}",info.comment);
        
    return msg;
}

// When user don't places all required arguments
std::string 
Errors::ArgumentError::unplaced_arg(
    std::string arg_name
) {
    info.message = fmt::format("Missing required argument '{}' with function '{}' arguments", arg_name, function_name);

    msg = fmt::format(
        "{}: {} at line:{} ,column:{}",
        info.type, info.message, info.line, info.column
    );

    if (!info.comment.empty())
        msg+=fmt::format("\n{}",info.comment);
        
    return msg;
}

// ============== VariableError class constructure ==============

Errors::VariableError::VariableError(
    std::string v_name,
    size_t l_, 
    size_t c_, 
    size_t len,
    std::string com_
):
var_name(v_name) {
    info.line = l_;
    info.column = c_;
    info.length = len;
    info.comment = com_;
}

// When recreate variable in same scope
std::string 
Errors::VariableError::reset_var_in_same_scope() {
    info.type = "VariableError";
    info.message = fmt::format("Variable '{}' already defined in this scope",var_name);

    msg = fmt::format(
        "{}: {} at line:{} ,column:{}",
        info.type, info.message, info.line, info.column
    );

    if (!info.comment.empty())
        msg+=fmt::format("\n{}",info.comment);
        
    return msg;
}



// When reset variable value but new value is incompatible type with variable type
std::string 
Errors::VariableError::reset_var_value_by_other_type(
    std::string var_type, 
    std::string got_type
) {
    info.type = "TypeError";
    info.message = fmt::format("Cannot assign value of type '{}' to variable '{}' of type '{}'",got_type,var_name,var_type);

    msg = fmt::format(
        "{}: {} at line:{} ,column:{}",
        info.type, info.message, info.line, info.column
    );

    if (!info.comment.empty())
        msg+=fmt::format("\n{}",info.comment);
        
    return msg;
}

// When user reset a const variable
std::string 
Errors::VariableError::reset_const_var() {
    info.type = "ImmutableError";
    info.message = fmt::format("ImmutableError: Cannot modify constant variable '{}'",var_name);

    msg = fmt::format(
        "{}: {} at line:{} ,column:{}",
        info.type, info.message, info.line, info.column
    );

    if (!info.comment.empty())
        msg+=fmt::format("\n{}",info.comment);
        
    return msg;
}

// ============== UserFunctionError class constructure ==============

// Constructure
Errors::UserFunctionError::UserFunctionError(
    std::string f_name,
    size_t l_,
    size_t c_,
    size_t len,
    std::string com_
):
    function_name(f_name) {
        info.line = l_;
        info.column = c_;
        info.length = len;
        info.comment = com_;
    }


// When user returns uncompatible type data with function return type
std::string
Errors::UserFunctionError::return_type_error(
    std::string expected_type,
    std::string got_type
) {
    info.type = "ReturnTypeError";
    info.message = fmt::format("function '{}' expected return type '{}', got '{}'",function_name,expected_type,got_type);

    msg = fmt::format(
        "{}: {} at line:{} ,column:{}",
        info.type, info.message, info.line, info.column
    );

    if (!info.comment.empty())
        msg+=fmt::format("\n{}",info.comment);
        
    return msg;
}

// Not all paths return a value
std::string
Errors::UserFunctionError::missing_return_error() {
    info.type = "MissingReturnError";
    info.message = fmt::format("Not all code paths return a value in function '{}'",function_name);

    msg = fmt::format(
        "{}: {} at line:{} ,column:{}",
        info.type, info.message, info.line, info.column
    );

    if (!info.comment.empty())
        msg+=fmt::format("\n{}",info.comment);
        
    return msg;
}

// When user redifine a function by same name in same scope
std::string
Errors::UserFunctionError::function_already_defined() {
    info.type = "FunctionError";
    info.message = fmt::format("function '{}' already defined in this scope",function_name);

    msg = fmt::format(
        "{}: {} at line:{} ,column:{}",
        info.type, info.message, info.line, info.column
    );

    if (!info.comment.empty())
        msg+=fmt::format("\n{}",info.comment);
        
    return msg;
}


// ============== RuntimeError class constructure ==============

Errors::RuntimeError::RuntimeError(
    size_t l_,
    size_t c_,
    size_t len,
    std::string com_
) {
    info.line = l_;
    info.column = c_;
    info.length = len;
    info.comment = com_;
}


// When trying to divide by zero
std::string
Errors::RuntimeError::division_by_zero() {
    info.type = "RuntimeError";
    info.message = fmt::format("division by zero");

    msg = fmt::format(
        "{}: {} at line:{} ,column:{}",
        info.type, info.message, info.line, info.column
    );

    if (!info.comment.empty())
        msg+=fmt::format("\n{}",info.comment);
        
    return msg;
}