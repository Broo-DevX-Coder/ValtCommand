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
    StatmentsT& s_
): statements(std::move(s_)) {
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
        if (statement)
            ss << statement->get_str(level + 1);
    }

    return ss.str();
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
        0
    );

    ReturnResult<Value> r;

    std::size_t statement_index = 0;

    for (auto& stmt : statements) {
        ++statement_index;

        if (!stmt) {
            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Skipping null statement #" +
                    std::to_string(statement_index),
                0
            );

            continue;
        }

        emit_event(
            Context,
            DevTools::EventsT::Runtime,
            "Executing program statement #" +
                std::to_string(statement_index),
            0
        );

        r = stmt->exec(Context);

        if (!r.success) {
            emit_event(
                Context,
                DevTools::EventsT::Runtime,
                "Program statement #" +
                    std::to_string(statement_index) +
                    " execution failed",
                0
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
                0
            );

            return r;
        }

        // Preserve the original ownership behavior.
        stmt = nullptr;
    }

    emit_event(
        Context,
        DevTools::EventsT::Runtime,
        "Program execution completed successfully",
        0
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
        0
    );

    ReturnResult<bool> r;

    std::size_t statement_index = 0;

    for (auto& stmt : statements) {
        ++statement_index;

        if (!stmt) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Skipping null statement #" +
                    std::to_string(statement_index),
                0
            );

            continue;
        }

        emit_event(
            Context,
            DevTools::EventsT::Analysis,
            "Analyzing program statement #" +
                std::to_string(statement_index),
            0
        );

        r = stmt->accept(Context);

        if (!r.success) {
            emit_event(
                Context,
                DevTools::EventsT::Analysis,
                "Program statement #" +
                    std::to_string(statement_index) +
                    " semantic analysis failed",
                0
            );

            return r;
        }
    }

    emit_event(
        Context,
        DevTools::EventsT::Analysis,
        "Program semantic analysis completed successfully",
        0
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
        if (!s) {
            new_statments.push_back(nullptr);
            continue;
        }

        std::unique_ptr<ASTNode> statment_c(s->clone());
        new_statments.push_back(std::move(statment_c));
    }

    return new ModuleNode(new_statments);
}