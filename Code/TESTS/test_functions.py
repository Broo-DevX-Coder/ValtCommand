"""Built-in and user-defined function checks for valtc."""
import pathlib
import subprocess
import os
import sys
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
VALTC = ROOT / "Code" / "build" / "bin" / "valtc"


def run(code):
    env = os.environ.copy(); env["ASAN_OPTIONS"] = "detect_leaks=0"
    return subprocess.run([str(VALTC), "--eval", code], text=True, capture_output=True, env=env)


class Functions(unittest.TestCase):
    def assert_run(self, label, code, stdout, stderr="", rc=0):
        with self.subTest(name=label):
            p = run(code)
            self.assertEqual((p.stdout, p.stderr, p.returncode), (stdout, stderr, rc))

    def test_math_success(self):
        # Representative constants, trig/log/math utilities and seeded random.
        self.assert_run("math success", 'CALL println value<float>:GET PI END CALL println value<float>:CALL sin x<float>:0 END END CALL println value<float>:CALL log x<float>:1 END END CALL println value<int>:CALL abs x<int>:-4 END END CALL seed_random seed<int>:9 END CALL println value<int>:CALL randint lo<int>:1 hi<int>:1 END END', "3.14159\n0\n0\n4\n1\n")

    def test_stdio_and_format(self):
        # print, println, input and named format substitution.
        self.assert_run("format", 'CALL println value<str>:CALL format text<str>:"Hi {name}" name<str>:"Ada" END END', "Hi Ada\n")
        env = os.environ.copy(); env["ASAN_OPTIONS"] = "detect_leaks=0"
        p = subprocess.run([str(VALTC), "--eval", 'SET x<str> = CALL input prompt<str>:"?" END CALL println value<str>:GET x END'], input="yes\n", text=True, capture_output=True, env=env)
        self.assertEqual((p.stdout, p.stderr, p.returncode), ("?yes\n", "", 0))

    def test_user_functions(self):
        # No args, multiple args, return values, loop calls and expression calls.
        self.assert_run("user functions", 'FUNCTION add(a<int> b<int>)->int\nRETURN GET a + GET b\nEND\nFUNCTION ping()->void\nCALL print value<str>:"p" END\nEND\nCALL ping END\nCALL println value<int>:CALL add a<int>:2 b<int>:3 END END\nFOR SET i<int> = 0; GET i < 2; SET i = GET i + 1 THEN\nCALL println value<int>:CALL add a<int>:GET i b<int>:1 END END\nEND', "p5\n1\n2\n")

    def test_limitation_forward_call(self):
        # limitation: a function cannot be called before its definition.
        p = run('CALL later END FUNCTION later()->int\nRETURN 1\nEND')
        self.assertEqual((p.stdout, p.returncode), ("", 2))
        self.assertIn("NameError", p.stderr)

    def test_every_math_function(self):
        # Every registered math function has its observed output; random is checked by range.
        cases = {
            "abs": ('x<float>:2', '2\n'), "sqrt": ('x<float>:4', '2\n'), "cbrt": ('x<float>:8', '2\n'),
            "pow": ('base<float>:2 exp<float>:3', '8\n'), "root": ('x<float>:8 n<int>:3', '2\n'),
            "floor": ('x<float>:2.8', '2\n'), "ceil": ('x<float>:2.2', '3\n'), "round": ('x<float>:2.5', '3\n'),
            "trunc": ('x<float>:2.8', '2\n'), "fmod": ('a<float>:7 b<float>:2', '1\n'),
            "min": ('a<float>:2 b<float>:3', '2\n'), "max": ('a<float>:2 b<float>:3', '3\n'),
            "clamp": ('x<float>:5 lo<float>:0 hi<float>:10', '5\n'), "sign": ('x<float>:-2', '-1\n'),
            "exp": ('x<float>:0', '1\n'), "exp2": ('x<float>:0', '1\n'), "log": ('x<float>:1', '0\n'),
            "log2": ('x<float>:1', '0\n'), "log10": ('x<float>:1', '0\n'), "log1p": ('x<float>:0', '0\n'),
            "sin": ('x<float>:0', '0\n'), "cos": ('x<float>:0', '1\n'), "tan": ('x<float>:0', '0\n'),
            "asin": ('x<float>:0', '0\n'), "acos": ('x<float>:1', '0\n'), "atan": ('x<float>:0', '0\n'),
            "atan2": ('y<float>:0 x<float>:1', '0\n'), "sinh": ('x<float>:0', '0\n'), "cosh": ('x<float>:0', '1\n'),
            "tanh": ('x<float>:0', '0\n'), "degrees": ('x<float>:0', '0\n'), "radians": ('x<float>:0', '0\n'),
            "isnan": ('x<float>:0', 'false\n'), "isinf": ('x<float>:0', 'false\n'), "isfinite": ('x<float>:0', 'true\n'),
            "randint": ('lo<int>:4 hi<int>:4', '4\n'), "seed_random": ('seed<int>:4', '4\n'),
        }
        for name, (args, expected) in cases.items():
            with self.subTest(function=name):
                p = run(f'CALL println value<any>:CALL {name} {args} END END')
                self.assertEqual((p.stdout, p.stderr, p.returncode), (expected, '', 0))
        with self.subTest(function="random"):
            p = run('CALL println value<float>:CALL random END END')
            self.assertEqual((p.stderr, p.returncode), ('', 0))
            value = float(p.stdout)
            self.assertGreaterEqual(value, 0.0)
            self.assertLess(value, 1.0)

    def test_math_argument_errors(self):
        # Invalid domains and bounds exercise distinct runtime error paths.
        cases = [('sqrt x<float>:-1', 'ValueError'), ('log x<float>:0', 'ValueError'),
                 ('fmod a<float>:1 b<float>:0', 'ZeroDivisionError'),
                 ('root x<float>:-4 n<int>:2', 'ValueError'),
                 ('clamp x<float>:1 lo<float>:5 hi<float>:2', 'ValueError'),
                 ('randint lo<int>:5 hi<int>:2', 'ValueError'),
                 ('pow base<float>:0 exp<float>:-1', 'ZeroDivisionError')]
        for call, kind in cases:
            with self.subTest(call=call):
                p = run(f'CALL {call} END')
                self.assertEqual((p.stdout, p.returncode), ('', 2))
                self.assertIn(kind, p.stderr)

    def test_format_errors(self):
        # Format reports missing placeholders, malformed braces and wrong text types.
        cases = [('text<str>:"{missing}"', 'ArgumentError'),
                 ('text<str>:"{"', 'FormatError'),
                 ('text<str>:"{}"', 'FormatError'),
                 ('text<int>:3', 'TypeError')]
        for args, kind in cases:
            with self.subTest(args=args):
                p = run(f'CALL format {args} END')
                self.assertEqual(p.returncode, 2)
                self.assertIn(kind, p.stderr)

    def test_constants(self):
        # Constants are read-only numeric values; INF and NAN are recognized by predicates.
        self.assert_run("constants", 'CALL println value<float>:GET E END CALL println value<float>:GET TAU END CALL println value<bool>:CALL isinf x<float>:GET INF END END CALL println value<bool>:CALL isnan x<float>:GET NAN END END', "2.71828\n6.28319\ntrue\ntrue\n")

    def test_recursive_limitation(self):
        # limitation: self-recursive calls are rejected by semantic analysis.
        p = run('FUNCTION f(n<int>)->int\nRETURN CALL f n<int>:GET n END\nEND\nCALL f n<int>:1 END')
        self.assertEqual((p.stdout, p.returncode), ('', 2))
        self.assertIn("NameError", p.stderr)

    def test_function_calls_function(self):
        # A function defined earlier can be called from another function.
        self.assert_run("nested call", 'FUNCTION inner(x<int>)->int\nRETURN GET x + 1\nEND\nFUNCTION outer(x<int>)->int\nRETURN CALL inner x<int>:GET x END\nEND\nCALL println value<int>:CALL outer x<int>:2 END END', '3\n')

    def test_user_function_argument_errors(self):
        # Missing, surplus, and wrongly typed named arguments are semantic ArgumentErrors.
        cases = [('CALL f END', "missing required argument 'x'"),
                 ('CALL f x<int>:1 y<int>:2 END', 'Too many arguments'),
                 ('CALL f x<str>:"s" END', "has type 'str'; expected 'int'")]
        for call, message in cases:
            with self.subTest(call=call):
                p = run(f'FUNCTION f(x<int>)->int\nRETURN GET x\nEND\n{call}')
                self.assertEqual((p.stdout, p.returncode), ('', 2))
                self.assertIn(message, p.stderr)

    def test_format_multiple_placeholders(self):
        # Multiple and repeated placeholders are substituted from named arguments.
        self.assert_run("multiple placeholders", 'CALL println value<str>:CALL format text<str>:"{a}-{b}-{a}" a<int>:1 b<bool>:True END END', '1-true-1\n')


if __name__ == "__main__":
    class CountingResult(unittest.TextTestResult):
        def __init__(self, *args, **kwargs):
            super().__init__(*args, **kwargs); self.cases = 0; self.subtest_parents = set()
        def addSuccess(self, test):
            if id(test) not in self.subtest_parents: self.cases += 1
            super().addSuccess(test)
        def addSubTest(self, test, subtest, err):
            self.subtest_parents.add(id(test)); self.cases += 1
            super().addSubTest(test, subtest, err)
    class CountingRunner(unittest.TextTestRunner):
        resultclass = CountingResult
    result = unittest.main(exit=False, testRunner=CountingRunner)
    print(f"Cases: {result.result.cases}; passed: {result.result.cases-len(result.result.failures)-len(result.result.errors)}; failed: {len(result.result.failures)+len(result.result.errors)}")
    sys.exit(0 if result.result.wasSuccessful() else 1)
