// ==================================================================
// Include necessary headers
// ==================================================================

// == Libs ==
#include <sstream>
#include <cstdint>
#include <algorithm>
#include <string>

// == Locals ==
#include "ASTNodes/FunctionCallNode.hpp"
#include "Errors.hpp"

// ==================================================================
// Runtime Tracker Helpers
// ==================================================================

static void emit_event(
    NodesContext& Context,
    DevTools::EventsT type,
    const std::string& message,
    std::size_t line
) {
    if (Context.Debugger) {
        Context.Debugger->emit({
            type,
            message,
            line
        });
    }
}

// ==================================================================
// Function Call Argument Node
// ==================================================================

// Constructor
FunctionCallArgumentNode::FunctionCallArgumentNode(
    std::string& Aname,
    Token TToken,
    std::unique_ptr<ASTNode> Value_node
):
    name(Aname),
    type(TToken.value),
    TypeToken(TToken),
    VNode(std::move(Value_node))
{
    return_type = type;
}

// Get string representation
std::string
FunctionCallArgumentNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "Argument " << name << ":\n";

    ss << VNode->get_str(level + 1);

    return ss.str();
}

// Get the json file of ASTNode
nlohmann::json 
FunctionCallArgumentNode::get_json() {
    nlohmann::json result;

    result["type"] = "FunctionCallArgument";
    result["name"] = name;
    result["value"] = VNode->get_json();

    return result;
}

// Get node type
ASTNodesTypes
FunctionCallArgumentNode::NType() {
    return NT__FunctionCallArgumentNode;
}

// ==================================================================
// Analyze Function Call Argument
// ==================================================================

ReturnResult<bool>
FunctionCallArgumentNode::accept(
    NodesContext Context
) {
    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Starting semantic analysis of argument '" + name +
            "' with expected type '" + type + "'",
        TypeToken.line
    );

    // --------------------------------------------------------------
    // Validate the argument value node
    // --------------------------------------------------------------

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing the value expression of argument '" + name + "'",
        TypeToken.line
    );

    auto vnode_ar = VNode->accept(Context);

    if (!vnode_ar.success) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Semantic analysis failed for the value expression of argument '" +
                name + "'",
            TypeToken.line
        );

        return vnode_ar;
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Value expression of argument '" + name +
            "' analyzed successfully; actual type is '" +
            VNode->return_type + "'",
        TypeToken.line
    );

    // --------------------------------------------------------------
    // Check type compatibility
    // --------------------------------------------------------------

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Checking type compatibility for argument '" + name +
            "': expected '" + type +
            "', received '" + VNode->return_type + "'",
        TypeToken.line
    );

    if (!are_types_compatible(VNode->return_type, type)) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Type validation failed for argument '" + name +
                "': expected '" + type +
                "', received '" + VNode->return_type + "'",
            TypeToken.line
        );

        auto err = Errors::TypeError(
            type,
            VNode->return_type,
            TypeToken.line,
            TypeToken.column,
            TypeToken.value.size()
        );

        ReturnResult<bool> re = {
            err.msg,
            false,
            false
        };

        re.error_info = err.info;

        return re;
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Argument '" + name + "' passed type validation",
        TypeToken.line
    );

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Semantic analysis completed successfully for argument '" +
            name + "'",
        TypeToken.line
    );

    return {"", true, true};
}

// ==================================================================
// Execute Function Call Argument
// ==================================================================

ReturnResult<Value>
FunctionCallArgumentNode::exec(
    NodesContext Context
) {
    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Evaluating argument '" + name + "'",
        TypeToken.line
    );

    // --------------------------------------------------------------
    // Evaluate argument expression
    // --------------------------------------------------------------

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Executing the value expression of argument '" + name + "'",
        TypeToken.line
    );

    auto vnode_r = VNode->exec(Context);

    if (!vnode_r.success) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Execution failed while evaluating argument '" + name + "'",
            TypeToken.line
        );

        return vnode_r;
    }

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Value expression of argument '" + name +
            "' evaluated successfully",
        TypeToken.line
    );

    // --------------------------------------------------------------
    // Reconcile the argument value with its declared type
    // --------------------------------------------------------------

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Reconciling the value of argument '" + name +
            "' with expected type '" + type + "'",
        TypeToken.line
    );

    value = reconsiliation_int_float(type, vnode_r.value);

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Value reconciliation completed for argument '" + name + "'",
        TypeToken.line
    );

    // Release the evaluated AST node, following the existing behavior.
    VNode = nullptr;

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Argument '" + name + "' evaluated successfully",
        TypeToken.line
    );

    return {"", true, std::monostate{}};
}

// ==================================================================
// Clone Function Call Argument
// ==================================================================

ASTNode*
FunctionCallArgumentNode::clone() {
    std::unique_ptr<ASTNode> new_value_node(VNode->clone());

    return new FunctionCallArgumentNode(
        name,
        TypeToken,
        std::move(new_value_node)
    );
}

// ==================================================================
// Function Call Node
// ==================================================================

// Constructor
FunctionCallNode::FunctionCallNode(
    std::string Fname,
    ArgsT& Args_list,
    Token n_token
):
    name(Fname),
    arguments(std::move(Args_list)),
    NameToken(n_token)
{}

// Get string representation
std::string
FunctionCallNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "FunctionCall " << name;

    if (!arguments.empty())
        ss << ":";

    ss << "\n";

    for (auto& arg : arguments)
        ss << arg->get_str(level + 1);

    return ss.str();
}

// Get the json file of ASTNode
nlohmann::json 
FunctionCallNode::get_json() {
    nlohmann::json result;

    result["type"] = "FunctionCall";
    result["name"] = name;
    result["arguments"] = nlohmann::json::array();

    for (const auto& arg : arguments)
        result["arguments"].push_back(arg->get_json());

    return result;
}

// Get node type
ASTNodesTypes
FunctionCallNode::NType() {
    return NT__FunctionCallNode;
}

// ==================================================================
// Analyze Function Call
// ==================================================================

ReturnResult<bool>
FunctionCallNode::accept(
    NodesContext Context
) {
    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Starting semantic analysis of function call '" + name +
            "' with " + std::to_string(arguments.size()) + " argument(s)",
        NameToken.line
    );

    // --------------------------------------------------------------
    // Search for the function
    // --------------------------------------------------------------

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Searching for function '" + name + "' in the current scope",
        NameToken.line
    );

    auto search_result = Context.ParentScope->search_function(NameToken);

    if (!search_result.success) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Function lookup failed for '" + name + "'",
            NameToken.line
        );

        ReturnResult<bool> re = {
            search_result.Message,
            false,
            false
        };

        re.error_info = search_result.error_info;

        return re;
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Function '" + name + "' found successfully",
        NameToken.line
    );

    return_type = search_result.value->return_type;

    auto func = search_result.value;

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Function '" + name + "' has return type '" +
            return_type + "'",
        NameToken.line
    );

    // --------------------------------------------------------------
    // Create argument error information
    // --------------------------------------------------------------

    auto error_obj = Errors::ArgumentError(
        name,
        NameToken.line,
        NameToken.column,
        NameToken.value.size()
    );

    // --------------------------------------------------------------
    // Validate arguments for functions without arbitrary arguments
    // --------------------------------------------------------------

    if (!func->is_sepport_any_methods_) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Validating the arguments provided to function '" + name + "'",
            NameToken.line
        );

        // Check the number of arguments
        if (arguments.size() > func->methods.size()) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Argument validation failed for function '" + name +
                    "': too many arguments were provided",
                NameToken.line
            );

            ReturnResult<bool> re = {
                error_obj.too_many_arguments(
                    func->methods.size(),
                    arguments.size()
                ),
                false,
                false
            };

            re.error_info = error_obj.info;

            return re;
        }

        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Argument count validation passed for function '" + name + "'",
            NameToken.line
        );

        // ----------------------------------------------------------
        // Check whether provided argument names are valid
        // ----------------------------------------------------------

        for (auto& ar : arguments) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Checking whether argument '" + ar->name +
                    "' is accepted by function '" + name + "'",
                ar->TypeToken.line
            );

            if (!func->methods.contains(ar->name)) {
                emit_event(
                    Context,
                    DevTools::EventsT::Analysis,
                    "Argument validation failed: unexpected argument '" +
                        ar->name + "' passed to function '" + name + "'",
                    ar->TypeToken.line
                );

                ReturnResult<bool> re = {
                    error_obj.unexpected_argument(ar->name),
                    false,
                    false
                };

                re.error_info = error_obj.info;

                return re;
            }

            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Argument name '" + ar->name +
                    "' is accepted by function '" + name + "'",
                ar->TypeToken.line
            );
        }

        // ----------------------------------------------------------
        // Check required arguments
        // ----------------------------------------------------------

        for (auto& [mt_n, mt] : func->methods) {
            if (mt.is_required) {
                emit_event(
                    Context,
                    DevTools::EventsT::Analysis,
                    "Checking required argument '" + mt_n +
                        "' for function '" + name + "'",
                    NameToken.line
                );

                auto it = std::find_if(
                    arguments.begin(),
                    arguments.end(),
                    [mt_n](
                        const std::unique_ptr<FunctionCallArgumentNode>& arg
                    ) {
                        return arg->name == mt_n;
                    }
                );

                if (it == arguments.end()) {
                    emit_event(
                        Context,
                        DevTools::EventsT::Analysis,
                        "Required argument '" + mt_n +
                            "' is missing from function call '" + name + "'",
                        NameToken.line
                    );

                    ReturnResult<bool> re = {
                        error_obj.unplaced_arg(mt_n),
                        false,
                        false
                    };

                    re.error_info = error_obj.info;

                    return re;
                }

                emit_event(
                    Context,
                    DevTools::EventsT::Analysis,
                    "Required argument '" + mt_n +
                        "' was provided to function '" + name + "'",
                    (*it)->TypeToken.line
                );
            }
        }

        // ----------------------------------------------------------
        // Check argument types
        // ----------------------------------------------------------

        for (auto& ar : arguments) {
            auto method = func->methods[ar->name];

            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Checking type of argument '" + ar->name +
                    "' for function '" + name +
                    "': expected '" + method.type +
                    "', received '" + ar->type + "'",
                ar->TypeToken.line
            );

            if (!method.is_any &&
                !are_types_compatible(method.type, ar->type)) {

                emit_event(
                    Context,
                    DevTools::EventsT::Analysis,
                    "Argument type validation failed for '" + ar->name +
                        "' in function '" + name + "'",
                    ar->TypeToken.line
                );

                ReturnResult<bool> re = {
                    error_obj.unexpected_argument_type(
                        ar->name,
                        method.type,
                        ar->type
                    ),
                    false,
                    false
                };

                re.error_info = error_obj.info;

                return re;
            }

            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Argument type validation passed for '" + ar->name +
                    "' in function '" + name + "'",
                ar->TypeToken.line
            );
        }

        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "All argument validations passed for function '" + name + "'",
            NameToken.line
        );
    } else {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Function '" + name +
                "' supports arbitrary arguments; fixed argument validation skipped",
            NameToken.line
        );
    }

    // --------------------------------------------------------------
    // Analyze each argument expression
    // --------------------------------------------------------------

    for (auto& ar : arguments) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Starting semantic analysis of argument '" + ar->name +
                "' in function call '" + name + "'",
            ar->TypeToken.line
        );

        auto arg_accept_r = ar->accept(Context);

        if (!arg_accept_r.success) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Semantic analysis failed for argument '" + ar->name +
                    "' in function call '" + name + "'",
                ar->TypeToken.line
            );

            return arg_accept_r;
        }

        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Semantic analysis completed for argument '" + ar->name +
                "' in function call '" + name + "'",
            ar->TypeToken.line
        );
    }

    // --------------------------------------------------------------
    // Analyze user-defined function
    // --------------------------------------------------------------

    if (search_result.value->type ==
        Scopes::SymbolTableTypes::FunctionsTypes::Inside) {

        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Function '" + name +
                "' is user-defined; preparing its semantic analysis",
            NameToken.line
        );

        user_func_copy = std::unique_ptr<UserProxyFunctionNode>(
            dynamic_cast<UserProxyFunctionNode*>(
                func->user_function->clone()
            )
        );

        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Created a copy of the user-defined function '" + name + "'",
            NameToken.line
        );

        // ----------------------------------------------------------
        // Create function scope
        // ----------------------------------------------------------

        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Creating a semantic scope for user-defined function '" +
                name + "'",
            NameToken.line
        );

        auto func_scoupe = std::make_unique<Scopes::Scope>(
            Context.ParentScope
        );

        // ----------------------------------------------------------
        // Register provided arguments in function scope
        // ----------------------------------------------------------

        std::unordered_map<std::string, std::string> args_list;

        for (auto& arg : arguments) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Registering argument '" + arg->name +
                    "' with type '" + arg->type +
                    "' in the semantic scope of function '" + name + "'",
                arg->TypeToken.line
            );

            func_scoupe->add_var(
                arg->name,
                arg->type,
                std::monostate{},
                true
            );

            args_list[arg->name] = arg->type;
        }

        // ----------------------------------------------------------
        // Register default arguments
        // ----------------------------------------------------------

        for (auto& [method_name, method] : func->methods) {
            if (!args_list.contains(method_name)) {
                emit_event(
                    Context,
                    DevTools::EventsT::Analysis,
                    "Registering default argument '" + method_name +
                        "' with type '" + method.type +
                        "' in the semantic scope of function '" + name + "'",
                    NameToken.line
                );

                func_scoupe->add_var(
                    method_name,
                    method.type,
                    method.default_value,
                    true
                );
            }
        }

        // ----------------------------------------------------------
        // Analyze the user-defined function body
        // ----------------------------------------------------------

        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Starting semantic analysis of the body of function '" +
                name + "'",
            NameToken.line
        );

        auto a_r = user_func_copy->accept({
            func_scoupe.get(),
            Context.Debugger
        });

        if (!a_r.success) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Semantic analysis failed for the body of function '" +
                    name + "'",
                NameToken.line
            );

            return a_r;
        }

        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Semantic analysis completed successfully for the body of function '" +
                name + "'",
            NameToken.line
        );
    } else {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Function '" + name +
                "' is external; its implementation will be invoked at runtime",
            NameToken.line
        );
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Semantic analysis completed successfully for function call '" +
            name + "'",
        NameToken.line
    );

    return {"", true, true};
}

// ==================================================================
// Execute Function Call
// ==================================================================

ReturnResult<Value>
FunctionCallNode::exec(
    NodesContext Context
) {
    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Starting execution of function call '" + name + "'",
        NameToken.line
    );

    ExternalFunInType::ArgsType args_list;

    // --------------------------------------------------------------
    // Search for the function
    // --------------------------------------------------------------

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Searching for function '" + name + "' before execution",
        NameToken.line
    );

    auto search_result = Context.ParentScope->search_function(NameToken);

    if (!search_result.success) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Function lookup failed during execution of '" + name + "'",
            NameToken.line
        );

        ReturnResult<Value> re = {
            search_result.Message,
            false,
            std::monostate{}
        };

        re.error_info = search_result.error_info;

        return re;
    }

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Function '" + name + "' found successfully for execution",
        NameToken.line
    );

    return_type = search_result.value->return_type;

    auto func = search_result.value;

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Preparing to execute function '" + name +
            "' with return type '" + return_type + "'",
        NameToken.line
    );

    // --------------------------------------------------------------
    // Evaluate arguments
    // --------------------------------------------------------------

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Evaluating " + std::to_string(arguments.size()) +
            " argument(s) for function '" + name + "'",
        NameToken.line
    );

    for (auto& ar : arguments) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Executing argument '" + ar->name +
                "' for function '" + name + "'",
            ar->TypeToken.line
        );

        auto arg_exec_r = ar->exec(Context);

        if (!arg_exec_r.success) {
            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Execution failed while evaluating argument '" +
                    ar->name + "' for function '" + name + "'",
                ar->TypeToken.line
            );

            return arg_exec_r;
        }

        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Argument '" + ar->name +
                "' evaluated successfully for function '" + name + "'",
            ar->TypeToken.line
        );

        args_list[ar->name] = ar->value;

        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Stored the evaluated value of argument '" + ar->name +
                "' for function '" + name + "'",
            ar->TypeToken.line
        );

        // Release the argument node, following the existing behavior.
        ar = nullptr;
    }

    arguments.clear();

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Finished evaluating arguments for function '" + name + "'",
        NameToken.line
    );

    // --------------------------------------------------------------
    // Execute external function
    // --------------------------------------------------------------

    if (func->type ==
        Scopes::SymbolTableTypes::FunctionsTypes::Extenal) {

        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Function '" + name +
                "' is external; invoking its implementation",
            NameToken.line
        );

        auto result = func->external_func({args_list,Context.ParentScope});

        if (!result.success) {
            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "External function '" + name + "' returned an error",
                NameToken.line
            );

            return result;
        }

        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "External function '" + name +
                "' executed successfully",
            NameToken.line
        );

        return result;
    }

    // --------------------------------------------------------------
    // Execute user-defined function
    // --------------------------------------------------------------

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Function '" + name +
            "' is user-defined; preparing its execution scope",
        NameToken.line
    );

    // Create function scope
    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Creating a runtime scope for user-defined function '" +
            name + "'",
        NameToken.line
    );

    auto func_scoupe = std::make_unique<Scopes::Scope>(
        Context.ParentScope
    );

    // --------------------------------------------------------------
    // Register provided arguments in function scope
    // --------------------------------------------------------------

    for (auto& [arg_name, arg_value] : args_list) {
        auto arg_type = Get_ValueT(arg_value);

        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Registering argument '" + arg_name +
                "' with runtime type '" + arg_type +
                "' in function scope '" + name + "'",
            NameToken.line
        );

        func_scoupe->add_var(
            arg_name,
            arg_type,
            arg_value,
            true
        );
    }

    // --------------------------------------------------------------
    // Register default arguments
    // --------------------------------------------------------------

    for (auto& [method_name, method] : func->methods) {
        if (!args_list.contains(method_name)) {
            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Registering default value for argument '" + method_name +
                    "' in function '" + name + "'",
                NameToken.line
            );

            func_scoupe->add_var(
                method_name,
                method.type,
                method.default_value,
                true
            );
        }
    }

    // --------------------------------------------------------------
    // Execute the user-defined function body
    // --------------------------------------------------------------

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Starting execution of the body of user-defined function '" +
            name + "'",
        NameToken.line
    );

    auto result = user_func_copy->exec({
        func_scoupe.get(),
        Context.Debugger
    });

    if (!result.success) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Execution failed inside user-defined function '" + name + "'",
            NameToken.line
        );

        return result;
    }

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "User-defined function '" + name +
            "' executed successfully",
        NameToken.line
    );

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Function call '" + name + "' completed successfully",
        NameToken.line
    );

    return result;
}

// ==================================================================
// Clone Function Call Node
// ==================================================================

ASTNode*
FunctionCallNode::clone() {
    ArgsT new_args_list;

    for (auto& a : arguments) {
        std::unique_ptr<FunctionCallArgumentNode> arg(
            dynamic_cast<FunctionCallArgumentNode*>(a->clone())
        );

        new_args_list.push_back(std::move(arg));
    }

    return new FunctionCallNode(
        name,
        new_args_list,
        NameToken
    );
}