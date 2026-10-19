// ==================================================================
// Include necessary headers
// ==================================================================

// == Libs ==
#include <sstream>
#include <string>

// == Locals ==
#include "ASTNodes/ASTNode.hpp"

// ==================================================================
// Runtime Tracker Helper
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
// Module node functions
// ==================================================================

// Constructor
ModuleNode::ModuleNode(
    StatmentsT& s_,
    Token& First_token
): statements(std::move(s_)),
   FToken(First_token) {
    return_type = "void";
}

// Get string to print
std::string
ModuleNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i = 0; i < level; i++)
        ss << "|  ";

    ss << "Program:\n";

    for (auto& statement : statements) {
        if (statement.node)
            ss << statement.node->get_str(level + 1);
    }

    return ss.str();
}

// Get the json file of ASTNode
nlohmann::json
ModuleNode::get_json() {
    nlohmann::json result;

    result["type"] = "Program";
    result["statements"] = nlohmann::json::array();

    for (const auto& statement : statements) {
        if (statement.node)
            result["statements"].push_back(statement.node->get_json());
    }

    return result;
}

// Get the node type
ASTNodesTypes
ModuleNode::NType() {
    return NT__ModuleNode;
}

// ==================================================================
// Module node: runtime execution
// ==================================================================

ReturnResult<Value>
ModuleNode::exec(
    NodesContext Context
) {
    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Starting program execution",
        FToken.line
    );

    ReturnResult<Value> r;

    std::size_t statement_index = 0;

    for (auto& stmt : statements) {
        ++statement_index;

        if (!stmt.node) {
            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Skipping null statement #" +
                    std::to_string(statement_index),
                FToken.line
            );

            continue;
        }

        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Executing program statement #" +
                std::to_string(statement_index),
            FToken.line
        );

        r = stmt.node->exec(Context);

        if (!r.success) {
            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Program statement #" +
                    std::to_string(statement_index) +
                    " execution failed",
                FToken.line
            );

            return r;
        }

        if (
            r.state == ExecState::Return ||
            r.state == ExecState::Break ||
            r.state == ExecState::Continue
        ) {
            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Control-flow state propagated from program statement #" +
                    std::to_string(statement_index),
                FToken.line
            );

            return r;
        }

        // Preserve the original ownership behavior.
        stmt.node = nullptr;
    }

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Program execution completed successfully",
        FToken.line
    );

    return {"", true, std::monostate{}};
}

// ==================================================================
// Module node: semantic analysis
// ==================================================================

ReturnResult<bool>
ModuleNode::accept(
    NodesContext Context
) {
    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Starting program semantic analysis",
        FToken.line
    );

    ReturnResult<bool> r;

    std::size_t statement_index = 0;

    for (auto& stmt : statements) {
        ++statement_index;

        if (!stmt.node) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Skipping null statement #" +
                    std::to_string(statement_index),
                FToken.line
            );

            continue;
        }

        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Analyzing program statement #" +
                std::to_string(statement_index),
            FToken.line
        );

        r = stmt.node->accept(Context);

        if (!r.success) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Program statement #" +
                    std::to_string(statement_index) +
                    " semantic analysis failed",
                FToken.line
            );

            return r;
        }
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Program semantic analysis completed successfully",
        FToken.line
    );

    return {"", true, true};
}

// ==================================================================
// Clone module node
// ==================================================================

ASTNode*
ModuleNode::clone() {
    StatmentsT new_statments;

    for (auto& s : statements) {
        if (!s.node) {
            new_statments.push_back({nullptr});
            continue;
        }

        std::unique_ptr<ASTNode> statment_c(s.node->clone());
        new_statments.push_back({std::move(statment_c),s.first_token});
    }

    return new ModuleNode(new_statments,FToken);
}