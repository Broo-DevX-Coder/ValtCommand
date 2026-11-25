#include <valtcommand/valtcommand.hpp>
#include <cassert>
int main() {
  vltc::Runtime::RunTime rt;
  auto ok = rt.set_variable("x", "1", "int");
  auto rejected = rt.set_variable("x", "not-an-int", "int");
  assert(ok && !rejected && rejected.error.stage == vltc::Runtime::ErrorStage::Host);
  assert(rt.get_variable("x").value == "1");
}
