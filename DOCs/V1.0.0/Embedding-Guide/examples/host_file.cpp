#include <valtcommand/valtcommand.hpp>
#include <cassert>
#include <iostream>
int main(int argc, char** argv) {
  if(argc != 2) return 2;
  vltc::Runtime::RunTime rt;
  vltc::Runtime::ExternalFunctionArgument arg{"int", false, true, false, int64_t{0}};
  rt.add_external_function([](vltc::ExternalFunInType in) {
    vltc::ReturnResult<vltc::Value> r; r.success=true;
    r.value=std::get<int64_t>(in.arguments.at("x"))+int64_t{2}; return r;
  }, "increment", "int", {{"x", arg}});
  auto r=rt.run_file(argv[1]);
  if(!r) { std::cerr << r.error.message << '\n'; return 1; }
  assert(rt.get_variable("answer").value=="42");
}
