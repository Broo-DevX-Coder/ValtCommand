#include <valtcommand/valtcommand.hpp>
#include <cassert>
int main() {
  vltc::Runtime::RunTime rt;
  vltc::Runtime::ExternalFunctionArgument arg{"int", false, true, false, int64_t{0}};
  rt.add_external_function([](vltc::ExternalFunInType in) {
    vltc::ReturnResult<vltc::Value> r; r.success=true;
    r.value=std::get<int64_t>(in.arguments.at("x"))+int64_t{1}; return r;
  }, "increment", "int", {{"x", arg}});
  auto r=rt.run("SET answer<int> = CALL increment x<int>:4 END");
  assert(r && rt.get_variable("answer").value=="5");
}
