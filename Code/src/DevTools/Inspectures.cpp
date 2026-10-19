// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "DevTools/Inspectures.hpp"

// == Libs ==
#include <sstream>

// ==================================================================
// Helper: Variant's value to nlohmann::json obj
// ==================================================================

nlohmann::json 
value_to_json(
    const Value& value
) {
    if (std::holds_alternative<std::monostate>(value))
        return nullptr;

    if (std::holds_alternative<int64_t>(value))
        return std::get<int64_t>(value);

    if (std::holds_alternative<long double>(value))
        return std::get<long double>(value);

    if (std::holds_alternative<std::string>(value))
        return std::get<std::string>(value);

    if (std::holds_alternative<bool>(value))
        return std::get<bool>(value);

    return nullptr;
}

// ==================================================================
// Helper: operator to push variant value to stringstream
// ==================================================================

std::stringstream& 
operator<<(
    std::stringstream& ss,
    Value& value
) {
    if (std::holds_alternative<std::monostate>(value))
        ss << "null";

    if (std::holds_alternative<int64_t>(value))
        ss << *(std::get_if<int64_t>(&value));

    if (std::holds_alternative<long double>(value))
        ss << *(std::get_if<long double>(&value));

    if (std::holds_alternative<std::string>(value))
        ss << *(std::get_if<std::string>(&value));

    if (std::holds_alternative<bool>(value))
        ss << *(std::get_if<bool>(&value));

    return ss;
}

// ==================================================================
// Helper: Variant's value to std::string
// ==================================================================

std::string
value_to_str(
    const Value& value
) {
    std::stringstream ss;
    if (std::holds_alternative<std::monostate>(value))
        ss << "null";

    if (std::holds_alternative<int64_t>(value))
        ss << *(std::get_if<int64_t>(&value));

    if (std::holds_alternative<long double>(value))
        ss << *(std::get_if<long double>(&value));

    if (std::holds_alternative<std::string>(value))
        ss << *(std::get_if<std::string>(&value));

    if (std::holds_alternative<bool>(value))
        ss << *(std::get_if<bool>(&value));

    return ss.str();
}

// ==================================================================
// Functions
// ==================================================================

// Get the basic information of the curent scope
DevTools::InspecturesReturn
DevTools::ScopeInspect(
    Scopes::Scope* scope
) {
    // Iniitialyze results objects
    std::stringstream ss;
    nlohmann::json JSON;

    // Iniitialyze string
    ss << "================= Scope inspect result ====================\n"; 

    // Scope ID
    ss << "Scope ID: " << scope->get_id() << '\n';
    JSON["scope_id"] = scope->get_id();

    // Add parent ID if it is exists
    {
        auto parent = scope->get_parent();
        if (parent) {
            ss << "Parent ID: " << parent->get_id() << '\n';
            JSON["parent_id"] = parent->get_id();
        }
    }

    // Depth of scope
    ss << "Scope depth: " << scope->get_depth() << '\n';
    JSON["depth"] = scope->get_depth();

    // Number of visible vars
    size_t vars_num = scope->get_variables_table().size();
    ss << "Visible Variables: " << vars_num << '\n';
    JSON["visible_vars"] = vars_num;

    // Number of visible functions
    size_t funcs_num = scope->get_functions_table().size();
    ss << "Visible Functions: " << funcs_num << '\n';
    JSON["visible_funcs"] = funcs_num;

    // End string
    ss << "===========================================================\n";

    // Return result
    return {ss.str(),JSON};
}

// Get visible variables table in entered scope
DevTools::InspecturesReturn
DevTools::VariablesInspect(
    Scopes::Scope* scope
) {
    // Initialize result objects
    std::stringstream ss;
    nlohmann::json JSON = nlohmann::json::object();

    // Check scope
    if (scope == nullptr) {
        return {"Error: Cannot inspect a null scope.", JSON};
    }

    // Get variables table
    auto vars_table = scope->get_variables_table();

    // Initialize field sizes
    constexpr int field_max = 20;

    int name_field      = 4;  // Name
    int scope_id_field  = 8;  // Scope ID
    int data_type_field = 9;  // Data Type
    int constant_field  = 8;  // Constant
    int value_field     = 5;  // Value

    // Add all variables to JSON and calculate field sizes
    for (auto& var : vars_table) {

        // Add variable to JSON
        JSON[var.name]["scope_id"] = var.var_ptr->scope_id;
        JSON[var.name]["constant"] = var.var_ptr->is_const;
        JSON[var.name]["data_type"] = var.var_ptr->type;
        JSON[var.name]["value"] = value_to_json(var.var_ptr->value);

        // Calculate Name width
        if (var.name.size() > static_cast<size_t>(name_field)) {
            name_field = static_cast<int>(var.name.size());
        }

        // Calculate Scope ID width
        std::stringstream scope_id_ss;
        scope_id_ss << var.var_ptr->scope_id;

        if (scope_id_ss.str().size() > static_cast<size_t>(scope_id_field)) {
            scope_id_field = static_cast<int>(scope_id_ss.str().size());
        }

        // Calculate Data Type width
        if (var.var_ptr->type.size() > static_cast<size_t>(data_type_field)) {
            data_type_field = static_cast<int>(var.var_ptr->type.size());
        }

        // Calculate Constant width
        const std::string constant_str =
            var.var_ptr->is_const ? "True" : "False";

        if (constant_str.size() > static_cast<size_t>(constant_field)) {
            constant_field = static_cast<int>(constant_str.size());
        }

        // Calculate Value width
        const std::string value_str = value_to_str(var.var_ptr->value);

        if (value_str.size() > static_cast<size_t>(value_field)) {
            value_field = static_cast<int>(value_str.size());
        }
    }

    // Limit field sizes
    name_field      = (name_field > field_max)      ? field_max : name_field;
    scope_id_field  = (scope_id_field > field_max)  ? field_max : scope_id_field;
    data_type_field = (data_type_field > field_max) ? field_max : data_type_field;
    constant_field  = (constant_field > field_max)  ? field_max : constant_field;
    value_field     = (value_field > field_max)     ? field_max : value_field;

    // Table column widths
    const int widths[] = {
        name_field,
        scope_id_field,
        data_type_field,
        constant_field,
        value_field
    };

    // Format a cell: truncate if necessary, then pad with spaces
    auto format_cell = [](const std::string& value, int width) -> std::string {
        if (value.size() > static_cast<size_t>(width)) {
            if (width <= 3) {
                return value.substr(0, static_cast<size_t>(width));
            }

            return value.substr(0, static_cast<size_t>(width - 3)) + "...";
        }

        return value + std::string(
            static_cast<size_t>(width) - value.size(),
            ' '
        );
    };

    // Draw a horizontal border
    auto draw_border = [&]() {
        ss << '+';

        for (int width : widths) {
            for (int i = 0; i < width + 2; ++i) {
                ss << '-';
            }

            ss << '+';
        }

        ss << '\n';
    };

    // Draw a table row
    auto draw_row = [&](const std::string fields[]) {
        ss << '|';

        for (int i = 0; i < 5; ++i) {
            ss << ' '
               << format_cell(fields[i], widths[i])
               << ' '
               << '|';
        }

        ss << '\n';
    };

    // Draw the table
    draw_border();

    // Add column titles
    const std::string headers[] = {
        "Name",
        "Scope ID",
        "Data Type",
        "Constant",
        "Value"
    };

    draw_row(headers);

    // Add header separator
    draw_border();

    // Add variables
    for (auto& var : vars_table) {

        // Scope ID
        std::stringstream scope_id_ss;
        scope_id_ss << var.var_ptr->scope_id;

        // Constant
        const std::string constant_str =
            var.var_ptr->is_const ? "True" : "False";

        // Value
        const std::string value_str =
            value_to_str(var.var_ptr->value);

        // Prepare row
        const std::string fields[] = {
            var.name,
            scope_id_ss.str(),
            var.var_ptr->type,
            constant_str,
            value_str
        };

        draw_row(fields);
    }

    // Add final border
    draw_border();

    // Return table and JSON
    return {ss.str(), JSON};
}



// Get visible functions table in entered scope
DevTools::InspecturesReturn
DevTools::FunctionsInspect(
    Scopes::Scope* scope
) {
    // Initialize result objects
    std::stringstream ss;
    nlohmann::json JSON = nlohmann::json::object();

    // Check scope
    if (scope == nullptr) {
        return {"Error: Cannot inspect a null scope.", JSON};
    }

    // Get functions table
    auto funcs_table = scope->get_functions_table();

    // Initialize field sizes
    constexpr int field_max = 20;

    int name_field          = 4;
    int scope_id_field      = 8;
    int return_type_field   = 11;
    int function_type_field = 13;
    int any_methods_field   = 11;
    int methods_field       = 7;

    // Store formatted methods for table output
    std::unordered_map<std::string, std::string> methods_strings;

    // Add all functions to JSON and calculate field sizes
    for (auto& func : funcs_table) {

        auto* func_ptr = func.func_ptr;

        if (func_ptr == nullptr) {
            continue;
        }

        // Get function type
        const std::string function_type =
            func_ptr->type ==
                Scopes::SymbolTableTypes::FunctionsTypes::Extenal
                ? "External"
                : "Inside";

        // Get whether function accepts any methods
        const std::string any_methods =
            func_ptr->is_sepport_any_methods_ ? "True" : "False";

        // Add function to JSON
        JSON[func.name]["scope_id"] = func_ptr->scope_id;
        JSON[func.name]["return_type"] = func_ptr->return_type;
        JSON[func.name]["function_type"] = function_type;
        JSON[func.name]["any_methods"] =
            func_ptr->is_sepport_any_methods_;

        JSON[func.name]["methods"] = nlohmann::json::object();

        // Build methods string
        std::stringstream methods_ss;

        bool first_method = true;

        for (auto& method_entry : func_ptr->methods) {

            const std::string& method_name = method_entry.first;
            const auto& method = method_entry.second;

            // Add method information to JSON
            JSON[func.name]["methods"][method_name]["type"] =
                method.is_any ? "any" : method.type;

            JSON[func.name]["methods"][method_name]["required"] =
                method.is_required;

            JSON[func.name]["methods"][method_name]["is_any"] =
                method.is_any;

            JSON[func.name]["methods"][method_name]["default_value"] =
                value_to_json(method.default_value);

            // Format method for the ASCII table
            if (!first_method) {
                methods_ss << ", ";
            }

            first_method = false;

            methods_ss << method_name
                       << '<'
                       << (method.is_any ? "any" : method.type)
                       << '>';

            if (!method.is_required) {
                methods_ss << " = "
                           << value_to_str(method.default_value);
            }
        }

        const std::string methods_str = methods_ss.str();

        methods_strings[func.name] = methods_str;

        // Calculate Name width
        if (func.name.size() > static_cast<size_t>(name_field)) {
            name_field = static_cast<int>(func.name.size());
        }

        // Calculate Scope ID width
        std::stringstream scope_id_ss;
        scope_id_ss << func_ptr->scope_id;

        if (scope_id_ss.str().size() >
            static_cast<size_t>(scope_id_field)) {
            scope_id_field =
                static_cast<int>(scope_id_ss.str().size());
        }

        // Calculate Return Type width
        if (func_ptr->return_type.size() >
            static_cast<size_t>(return_type_field)) {
            return_type_field =
                static_cast<int>(func_ptr->return_type.size());
        }

        // Calculate Function Type width
        if (function_type.size() >
            static_cast<size_t>(function_type_field)) {
            function_type_field =
                static_cast<int>(function_type.size());
        }

        // Calculate Any Methods width
        if (any_methods.size() >
            static_cast<size_t>(any_methods_field)) {
            any_methods_field =
                static_cast<int>(any_methods.size());
        }

        // Calculate Methods width
        if (methods_str.size() >
            static_cast<size_t>(methods_field)) {
            methods_field =
                static_cast<int>(methods_str.size());
        }
    }

    // Limit field sizes
    name_field =
        (name_field > field_max) ? field_max : name_field;

    scope_id_field =
        (scope_id_field > field_max) ? field_max : scope_id_field;

    return_type_field =
        (return_type_field > field_max)
            ? field_max
            : return_type_field;

    function_type_field =
        (function_type_field > field_max)
            ? field_max
            : function_type_field;

    any_methods_field =
        (any_methods_field > field_max)
            ? field_max
            : any_methods_field;

    methods_field =
        (methods_field > field_max) ? field_max : methods_field;

    // Table column widths
    const int widths[] = {
        name_field,
        scope_id_field,
        return_type_field,
        function_type_field,
        any_methods_field,
        methods_field
    };

    // Format a cell: truncate if necessary, then pad with spaces
    auto format_cell = [](const std::string& value, int width)
        -> std::string {

        if (value.size() > static_cast<size_t>(width)) {

            if (width <= 3) {
                return value.substr(
                    0,
                    static_cast<size_t>(width)
                );
            }

            return value.substr(
                0,
                static_cast<size_t>(width - 3)
            ) + "...";
        }

        return value + std::string(
            static_cast<size_t>(width) - value.size(),
            ' '
        );
    };

    // Draw a horizontal border
    auto draw_border = [&]() {

        ss << '+';

        for (int width : widths) {

            for (int i = 0; i < width + 2; ++i) {
                ss << '-';
            }

            ss << '+';
        }

        ss << '\n';
    };

    // Draw a table row
    auto draw_row = [&](const std::string fields[]) {

        ss << '|';

        for (int i = 0; i < 6; ++i) {

            ss << ' '
               << format_cell(fields[i], widths[i])
               << ' '
               << '|';
        }

        ss << '\n';
    };

    // Draw table border
    draw_border();

    // Add column titles
    const std::string headers[] = {
        "Name",
        "Scope ID",
        "Return Type",
        "Function Type",
        "Any Methods",
        "Methods"
    };

    draw_row(headers);

    // Add header separator
    draw_border();

    // Add functions
    for (auto& func : funcs_table) {

        auto* func_ptr = func.func_ptr;

        if (func_ptr == nullptr) {
            continue;
        }

        // Scope ID
        std::stringstream scope_id_ss;
        scope_id_ss << func_ptr->scope_id;

        // Function type
        const std::string function_type =
            func_ptr->type ==
                Scopes::SymbolTableTypes::FunctionsTypes::Extenal
                ? "External"
                : "Inside";

        // Any methods
        const std::string any_methods =
            func_ptr->is_sepport_any_methods_ ? "True" : "False";

        // Methods
        const std::string methods_str =
            methods_strings[func.name];

        // Prepare row
        const std::string fields[] = {
            func.name,
            scope_id_ss.str(),
            func_ptr->return_type,
            function_type,
            any_methods,
            methods_str
        };

        draw_row(fields);
    }

    // Add final border
    draw_border();

    // Return table and JSON
    return {ss.str(), JSON};
}