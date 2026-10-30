// ==================================================================
// Include necessary headers
// ==================================================================

// == Locals ==
#include "ASTNodes/IfElseNode.hpp"
#include "Errors.hpp"

// == Libs ==
#include <sstream>
#include <string>
#include <memory>
#include <vector>

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
            line,
            Context.ParentScope,
            Context.FilePath
        });
    }
}

// ==================================================================
// If Statement Node Functions
// ==================================================================

// Constructor
IfStatmentsNode::IfStatmentsNode(const std::string& FilePath,
    const Token& END_token,
    Token& CFtoken,
    Node& condition,
    Nodes_list& statments,
    Nodes_list& else_statments
):
    condition_FT(CFtoken),
    condition_node(std::move(condition)),
    Statments(std::move(statments)),
    Else_statments(std::move(else_statments)),
    ASTNode(FilePath, END_token) {}

// ==================================================================
// Get String Representation
// ==================================================================

std::string
IfStatmentsNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "If statment \n";

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";

    ss << "Condition: \n"
       << condition_node->get_str(level + 2);

    for (int i = 0; i < level + 1; i++)
        ss << "|  ";

    ss << "Statments: \n";

    for (auto& stm : Statments)
        ss << stm->get_str(level + 2);

    if (!Else_statments.empty()) {
        for (int i = 0; i < level + 1; i++)
            ss << "|  ";

        ss << "Else Statments: \n";
    }

    for (auto& stm : Else_statments)
        ss << stm->get_str(level + 2);

    return ss.str();
}

// Get the json file of ASTNode
nlohmann::json
IfStatmentsNode::get_json() {
    nlohmann::json result;

    result["type"] = "IfStatement";
    result["condition"] = condition_node->get_json();

    result["statements"] = nlohmann::json::array();

    for (const auto& stm : Statments)
        result["statements"].push_back(stm->get_json());

    result["else_statements"] = nlohmann::json::array();

    for (const auto& stm : Else_statments)
        result["else_statements"].push_back(stm->get_json());

    return result;
}

// ==================================================================
// Get Node Type
// ==================================================================

ASTNodesTypes
IfStatmentsNode::NType() {
    return NT__IfStatmentsNode;
}

// ==================================================================
// Clone Node
// ==================================================================

ASTNode*
IfStatmentsNode::clone() {
    Node new_condition(
        condition_node ? condition_node->clone() : nullptr
    );

    Nodes_list new_stms_list;
    Nodes_list new_else_stms_list;

    for (auto& stm : Statments) {
        Node new_stm(
            stm ? stm->clone() : nullptr
        );

        new_stms_list.push_back(std::move(new_stm));
    }

    for (auto& stm : Else_statments) {
        Node new_stm(
            stm ? stm->clone() : nullptr
        );

        new_else_stms_list.push_back(std::move(new_stm));
    }

    return new IfStatmentsNode(file, END_token, condition_FT,
        new_condition,
        new_stms_list,
        new_else_stms_list
    );
}

// ==================================================================
// Semantic Analysis
// ==================================================================

ReturnResult<bool>
IfStatmentsNode::accept(
    NodesContext Context
) {
    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Starting semantic analysis of IF statement",
        condition_FT.line
    );

    // --------------------------------------------------------------
    // Set return type
    // --------------------------------------------------------------

    return_type = "void";

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Set IF statement return type to 'void'",
        condition_FT.line
    );

    // --------------------------------------------------------------
    // Analyze condition
    // --------------------------------------------------------------

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Analyzing the condition of IF statement",
        condition_FT.line
    );

    auto r = condition_node->accept(Context);

    if (!r.success) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Semantic analysis failed while analyzing the IF condition",
            condition_FT.line
        );

        return r;
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "IF condition analyzed successfully; detected type is '" +
            condition_node->return_type + "'",
        condition_FT.line
    );

    // --------------------------------------------------------------
    // Validate condition type
    // --------------------------------------------------------------

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Checking whether the IF condition has type 'bool'",
        condition_FT.line
    );

    if (condition_node->return_type != "bool") {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "IF condition type validation failed: expected 'bool', received '" +
                condition_node->return_type + "'",
            condition_FT.line
        );

        auto err = Errors::TypeError(
            "bool",
            condition_node->return_type,
            condition_FT.line,
            condition_FT.column,
            condition_FT.value.size()
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
        "IF condition passed type validation",
        condition_FT.line
    );

    // --------------------------------------------------------------
    // Prepare return nodes
    // --------------------------------------------------------------

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Collecting possible return statements from IF and ELSE branches",
        condition_FT.line
    );

    std::vector<ASTNode*> return_nodes;

    // --------------------------------------------------------------
    // Create IF scope
    // --------------------------------------------------------------

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Creating a semantic scope for IF statement branches",
        condition_FT.line
    );

    auto if_scoupe = std::make_unique<Scopes::Scope>(
        Context.ParentScope
    );

    // --------------------------------------------------------------
    // Analyze IF statements
    // --------------------------------------------------------------

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Starting semantic analysis of IF branch with " +
            std::to_string(Statments.size()) + " statement(s)",
        condition_FT.line
    );

    for (auto& smt : Statments) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Analyzing a statement in the IF branch",
            condition_FT.line
        );

        auto statement_result = smt->accept({
            if_scoupe.get(),
            Context.Debugger,
            Context.FilePath
        });

        if (!statement_result.success) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Semantic analysis failed inside the IF branch",
                condition_FT.line
            );

            return statement_result;
        }

        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Statement in the IF branch analyzed successfully",
            condition_FT.line
        );

        if (statement_result.state == ExecState::Return) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Found a possible RETURN statement in the IF branch",
                condition_FT.line
            );

            for (auto rptr : statement_result.return_nodes)
                return_nodes.push_back(rptr);
        }
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Semantic analysis completed for the IF branch",
        condition_FT.line
    );

    // --------------------------------------------------------------
    // Analyze ELSE statements
    // --------------------------------------------------------------

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Starting semantic analysis of ELSE branch with " +
            std::to_string(Else_statments.size()) + " statement(s)",
        condition_FT.line
    );

    for (auto& smt : Else_statments) {
        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Analyzing a statement in the ELSE branch",
            condition_FT.line
        );

        auto statement_result = smt->accept({
            if_scoupe.get(),
            Context.Debugger,
            Context.FilePath
        });

        if (!statement_result.success) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Semantic analysis failed inside the ELSE branch",
                condition_FT.line
            );

            return statement_result;
        }

        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Statement in the ELSE branch analyzed successfully",
            condition_FT.line
        );

        if (statement_result.state == ExecState::Return) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Found a possible RETURN statement in the ELSE branch",
                condition_FT.line
            );

            for (auto rptr : statement_result.return_nodes)
                return_nodes.push_back(rptr);
        }
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Semantic analysis completed for the ELSE branch",
        condition_FT.line
    );

    // --------------------------------------------------------------
    // Finish semantic analysis
    // --------------------------------------------------------------

    const bool has_return_nodes = !return_nodes.empty();

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "IF statement analysis found " +
            std::to_string(return_nodes.size()) +
            " possible return node(s)",
        condition_FT.line
    );

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Semantic analysis completed successfully for IF statement",
        condition_FT.line
    );

    return {
        "",
        true,
        true,
        has_return_nodes ? ExecState::Return : ExecState::Normal,
        std::move(return_nodes)
    };
}

// ==================================================================
// Runtime Execution
// ==================================================================

ReturnResult<Value>
IfStatmentsNode::exec(
    NodesContext Context
) {
    ASTNodeExecCompletionGuard exec_end_guard(*this, Context);
    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Starting execution of IF statement",
        condition_FT.line
    );

    return_type = "void";

    // --------------------------------------------------------------
    // Evaluate condition
    // --------------------------------------------------------------

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Evaluating the condition of IF statement",
        condition_FT.line
    );

    auto condition_result = condition_node->exec(Context);

    if (!condition_result.success) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Execution failed while evaluating the IF condition",
            condition_FT.line
        );

        return condition_result;
    }

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "IF condition evaluated successfully",
        condition_FT.line
    );

    // --------------------------------------------------------------
    // Release condition node
    // --------------------------------------------------------------

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Releasing the evaluated IF condition node",
        condition_FT.line
    );

    condition_node = nullptr;

    // --------------------------------------------------------------
    // Create IF scope
    // --------------------------------------------------------------

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Creating a runtime scope for IF statement",
        condition_FT.line
    );

    auto if_scoupe = std::make_unique<Scopes::Scope>(
        Context.ParentScope
    );

    // --------------------------------------------------------------
    // Execute IF branch
    // --------------------------------------------------------------

    if (condition_result.value == Value(true)) {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "IF condition evaluated to true; executing IF branch",
            condition_FT.line
        );

        for (auto& stm : Statments) {
            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Executing a statement in the IF branch",
                condition_FT.line
            );

            auto r = stm->exec({
                if_scoupe.get(),
                Context.Debugger,
            Context.FilePath
            });

            if (!r.success) {
                emit_event(
                    Context,
                    DevTools::EventsT::Runtime,
                    "Execution failed inside the IF branch",
                    condition_FT.line
                );

                return r;
            }

            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Statement in the IF branch executed successfully",
                condition_FT.line
            );

            if (r.state != ExecState::Normal) {
                emit_event(
                    Context,
                    DevTools::EventsT::Runtime,
                    "Statement in the IF branch changed execution state; propagating control flow",
                    condition_FT.line
                );

                return r;
            }

            // Release executed statement, preserving existing behavior.
            stm = nullptr;

            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Released the executed statement from the IF branch",
                condition_FT.line
            );
        }

        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "IF branch executed successfully",
            condition_FT.line
        );

    // --------------------------------------------------------------
    // Execute ELSE branch
    // --------------------------------------------------------------

    } else {
        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "IF condition evaluated to false; executing ELSE branch",
            condition_FT.line
        );

        for (auto& stm : Else_statments) {
            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Executing a statement in the ELSE branch",
                condition_FT.line
            );

            auto r = stm->exec({
                if_scoupe.get(),
                Context.Debugger,
            Context.FilePath
            });

            if (!r.success) {
                emit_event(
                    Context,
                    DevTools::EventsT::Runtime,
                    "Execution failed inside the ELSE branch",
                    condition_FT.line
                );

                return r;
            }

            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Statement in the ELSE branch executed successfully",
                condition_FT.line
            );

            if (r.state != ExecState::Normal) {
                emit_event(
                    Context,
                    DevTools::EventsT::Runtime,
                    "Statement in the ELSE branch changed execution state; propagating control flow",
                    condition_FT.line
                );

                return r;
            }

            // Release executed statement, preserving existing behavior.
            stm = nullptr;

            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Released the executed statement from the ELSE branch",
                condition_FT.line
            );
        }

        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "ELSE branch executed successfully",
            condition_FT.line
        );
    }

    // --------------------------------------------------------------
    // Finish execution
    // --------------------------------------------------------------

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "IF statement completed successfully",
        condition_FT.line
    );

    return {
        "",
        true,
        std::monostate()
    };
}
