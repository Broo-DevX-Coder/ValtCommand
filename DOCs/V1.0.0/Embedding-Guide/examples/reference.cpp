#include <valtcommand/valtcommand.hpp>
#include <cassert>
int main() {
  vltc::Scopes::Scope scope;
  auto* entry = scope.add_var("n", "int", int64_t{2});
  assert(entry && entry->value == vltc::Value{int64_t{2}});
  entry->type = "str";
  entry->value = std::string("changed");
  auto rows = scope.get_variables_table();
  assert(rows.size() == 1 && rows[0].var_ptr->type == "str");
  auto* replacement = scope.add_var("n", "bool", true);
  assert(replacement && replacement->value == vltc::Value{true});
}
