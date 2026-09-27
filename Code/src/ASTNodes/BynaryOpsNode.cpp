// ==================================================================
// Include neccessary headers
// ==================================================================

// == Locals ==
#include "ASTNodes/BynaryOpsNode.hpp"
#include "Errors.hpp"

// == Libs ==
#include <sstream>

// ==================================================================
// Binary operatins node classes
// ==================================================================

std::unordered_map<TokenType,ComparitonOpsTypes> ComparitonSymbols_ToOps = {
    {TokenType::EQUAL_EQUAL,ComparitonOpsTypes::EQUAL},
    {TokenType::NOT_EQUAL,ComparitonOpsTypes::NOT_EQUAL},
    {TokenType::LESS_THAN,ComparitonOpsTypes::LESS_THEN},
    {TokenType::GREATER_THAN,ComparitonOpsTypes::GREATER_THEN},
    {TokenType::LESS_EQUAL,ComparitonOpsTypes::LESS_THEN_OR_EQUAL},
    {TokenType::GREATER_EQUAL,ComparitonOpsTypes::GREATER_THEN_OR_EQUAL}
};

// ==================================================================
// Binary operatins node classes
// ==================================================================

// Constructure
BinOpsNode::BinOpsNode(
    OperationPartsList& parts_
): 
    Parts(std::move(parts_)) {}

// Get str to print
std::string 
BinOpsNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "Bynary operation " << ":\n";

    for (auto& part: Parts) {
        for (int i=0;i<level+1;i++)
            ss << "|  ";

        if (part.op == TokenType::PLUS) ss << "plus+" << ":\n";
        else if (part.op == TokenType::MINUS) ss << "minus-" << ":\n";
        else if (part.op == TokenType::STAR) ss << "multiple*" << ":\n";
        else if (part.op == TokenType::SLASH) ss << "divide/" << ":\n";
        else ss << "First: " << "\n";



        ss << part.node->get_str(level+2);
    }

    return ss.str();
}

// get type of node
ASTNodesTypes 
BinOpsNode::NType() {
    return NT__BinOpsNode;
}

// Typess checking
ReturnResult<bool> 
BinOpsNode::accept(
    Scopes::Scope* ParentScope
) {
    return_type = "float";
    for (auto& part: Parts) {
        auto accept_result = part.node->accept(ParentScope);
        if (!accept_result.success) {
            return {accept_result.Message,false,false};
        }
        if (part.node->return_type != "int" && part.node->return_type != "float") {
            return {Errors::TypeError(
                "int | float",
                part.node->return_type,
                part.token.line,
                part.token.column,
                "This operation does not support type else then int or float"
            ).msg,false,false};
        }
    }
    return {"",true,true};
}

// Do operation on a node
void
BinOpsNode::push_to_result(
    long double& result, 
    TokenType op, 
    long double input
) {
    if (op == TokenType::PLUS) {
        result += input;
    } else if (op == TokenType::MINUS) {
        result -= input;
    } else if (op == TokenType::STAR) {
        result *= input;
    } else if (op == TokenType::SLASH) {
        result /= input;
    }
}

// execute operation node
ReturnResult<Value> 
BinOpsNode::exec(
    Scopes::Scope* ParentScope
) { 
    long double result = 0;

    for (size_t i=0;i<Parts.size();i++) {
        auto& part = Parts[i];

        auto exec_result = part.node->exec(ParentScope);
        if (!exec_result.success) return {exec_result.Message,false,std::monostate{}};

        int64_t* val = std::get_if<int64_t>(&exec_result.value);

        if (val != nullptr) {
            if (i==0) {
                result = static_cast<long double>(*val);
            } else {
                push_to_result(result,part.op,static_cast<long double>(*val));
            }
        } else {
            long double* val = std::get_if<long double>(&exec_result.value);
            if (i==0) {
                result = static_cast<long double>(*val);
            } else {
                push_to_result(result,part.op,static_cast<long double>(*val));
            }
        }
    }

    return {"",true,result};
}

// Get a new copy of class
ASTNode*
BinOpsNode::clone() {
    OperationPartsList new_parts;

    for (auto& p: Parts) {
        std::unique_ptr<ASTNode> value_node(p.node->clone());
        new_parts.push_back({
            p.op,
            p.token,
            std::move(value_node)
        });
    }

    return new BinOpsNode(new_parts);
};

// ==================================================================
// Comparition operatins node classes
// ==================================================================

// Constructure
CompOpsNode::CompOpsNode(
    OperationPart& FToken, 
    OperationPart& SToken,
    ComparitonOpsTypes T
): 
    first_part(std::move(FToken)),
    second_part(std::move(SToken)),
    type(T) {}

// Get str to print
std::string 
CompOpsNode::get_str(
    int level
) {
    std::stringstream ss;

    for (int i=0;i<level;i++)
        ss << "|  ";
    ss << "Comparition operation " << ":\n";

    std::string op_type = "EQUAL";

    if (type == ComparitonOpsTypes::NOT_EQUAL) op_type = "NOT_EQUAL";
    else if (type == ComparitonOpsTypes::LESS_THEN) op_type = "LESS_THEN";
    else if (type == ComparitonOpsTypes::GREATER_THEN) op_type = "GREATER_THEN";
    else if (type == ComparitonOpsTypes::LESS_THEN_OR_EQUAL) op_type = "LESS_THEN_OR_EQUAL";
    else if (type == ComparitonOpsTypes::GREATER_THEN_OR_EQUAL) op_type = "GREATER_THEN_OR_EQUAL";

    for (int i=0;i<level+1;i++)
        ss << "|  ";
    ss << "Type: " << op_type << ":\n";

    for (int i=0;i<level+1;i++)
        ss << "|  ";
    ss << "First part: \n" << first_part.node->get_str(level+2);

    for (int i=0;i<level+1;i++)
        ss << "|  ";
    ss << "Second part: \n" << second_part.node->get_str(level+2);

    return ss.str();
}

// get type of node
ASTNodesTypes 
CompOpsNode::NType() {
    return NT__CompOpsNode;
}

// Clone the class or get a new copy from them
ASTNode* 
CompOpsNode::clone() {

    std::unique_ptr<ASTNode> fpart_ptr(first_part.node->clone());
    std::unique_ptr<ASTNode> spart_ptr(second_part.node->clone());

    OperationPart FirstP = {
        TokenType::UNKNOWN,
        first_part.token,
        std::move(fpart_ptr)
    };
    OperationPart SecondP = {
        TokenType::UNKNOWN,
        second_part.token,
        std::move(spart_ptr)
    };

    return new CompOpsNode(FirstP,SecondP,type);
}

// type and value checking
ReturnResult<bool> 
CompOpsNode::accept(
    Scopes::Scope* ParentScope
) {
    return_type = "bool";

    // Verifi parts
    auto fresult = first_part.node->accept(ParentScope);
    if (!fresult.success) return fresult;
    auto sresult = second_part.node->accept(ParentScope);
    if (!sresult.success) return sresult;

    // Verifi parts's types
    if (!are_types_compatible(first_part.node->return_type,second_part.node->return_type))  {
        return {
            fmt::format(
                "UncompatibleTypesError: can't compare `{}` with `{}` at line:{} ,column:{}",
                first_part.node->return_type, second_part.node->return_type, second_part.token.line, second_part.token.column
            ),
            false,false};
    }

    if (
        (
            type == ComparitonOpsTypes::LESS_THEN ||
            type == ComparitonOpsTypes::GREATER_THEN ||
            type == ComparitonOpsTypes::LESS_THEN_OR_EQUAL ||
            type == ComparitonOpsTypes::GREATER_THEN_OR_EQUAL

        ) && !(
            (first_part.node->return_type == "int" && second_part.node->return_type == "float") ||
            (first_part.node->return_type == "float" && second_part.node->return_type == "int")

        ) && first_part.node->return_type != second_part.node->return_type
    ) {
        return {
            fmt::format(
                "UncompatibleTypesError: can't compare `{}` with `{}` at line:{} ,column:{} \n These types deos't sepport this compare operation",
                first_part.node->return_type, second_part.node->return_type, second_part.token.line, second_part.token.column
            ),
            false,false};
    }

    return {"",true,true};
}

// execute and get the result of compariton
ReturnResult<Value> 
CompOpsNode::exec(
    Scopes::Scope* ParentScope
) {

    // Execute parts
    auto fresult = first_part.node->exec(ParentScope);
    if (!fresult.success) return fresult;
    auto sresult = second_part.node->exec(ParentScope);
    if (!sresult.success) return sresult;

    // Delete parts nodes
    first_part.node = nullptr;
    second_part.node = nullptr;

    // Do comparition op ------
    Value cp_result;

    // Equal op (==)
    if (type == ComparitonOpsTypes::EQUAL) 
        cp_result = fresult.value == sresult.value;

    // Non-equal op (!=)
    else if(type == ComparitonOpsTypes::NOT_EQUAL)
        cp_result = fresult.value != sresult.value;

    // Less-then op (<)
    else if (type == ComparitonOpsTypes::LESS_THEN)
        cp_result = fresult.value < sresult.value;

    // GREATER-then op (>)
    else if (type == ComparitonOpsTypes::GREATER_THEN)
        cp_result = fresult.value > sresult.value;

    // Less-then or equal op (<=)
    else if (type == ComparitonOpsTypes::LESS_THEN_OR_EQUAL)
        cp_result = fresult.value <= sresult.value;

    // GREATER-then or equal op (>=)
    else if (type == ComparitonOpsTypes::GREATER_THEN_OR_EQUAL)
        cp_result = fresult.value >= sresult.value;

    // Return result -----
    return {"",true,cp_result};
}