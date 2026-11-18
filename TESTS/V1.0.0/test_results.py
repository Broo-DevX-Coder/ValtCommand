"""CLI behavior checks for values, operators, scopes and control flow."""
import pathlib
import re
import subprocess
import os
import sys
import unittest
from decimal import Context

ROOT = pathlib.Path(__file__).resolve().parents[2]
VALTC = pathlib.Path(os.environ.get("VALTC_BIN", str(ROOT / "Code" / "build" / "bin" / "valtc")))
EXAMPLES = ROOT / "DOCs" / "V1.0.0" / "Language-references" / "examples"
EXPECTED = EXAMPLES.parent / "exceptions"

_FLOAT = re.compile(r"-?\d+\.\d+(?:[eE][+-]?\d+)?|-?\d+[eE][+-]?\d+")

SKIP_EXAMPLES_ON_WINDOWS = {
    "types-float-limits.vlt",
    "types-float-precision.vlt",
    "types-int-limits.vlt",
}


def _cut(match):
    # Round one float token to 6 significant digits, formatted like C++ default / %g.
    d = Context(prec=6).create_decimal(match.group(0))
    if d == 0:
        return "0"
    sign, digits, exp = d.as_tuple()
    digits = "".join(map(str, digits))
    x = exp + len(digits) - 1
    s = digits.rstrip("0") or "0"
    if x < -4 or x >= 6:
        body = s[0] + ("." + s[1:] if len(s) > 1 else "") + f"e{'-' if x < 0 else '+'}{abs(x):02d}"
    elif x >= 0:
        body = s[:x + 1].ljust(x + 1, "0") + ("." + s[x + 1:] if len(s) > x + 1 else "")
    else:
        body = "0." + "0" * (-x - 1) + s
    return ("-" if sign else "") + body


def cut_floats(text):
    return _FLOAT.sub(_cut, text)


def run(code):
    env = os.environ.copy(); env["ASAN_OPTIONS"] = "detect_leaks=0"
    return subprocess.run([str(VALTC), "--eval", code], text=True, capture_output=True, env=env)


class Results(unittest.TestCase):
    def check(self, name, code, out):
        with self.subTest(name=name):
            p = run(code)
            self.assertEqual((cut_floats(p.stdout), p.stderr, p.returncode), (cut_floats(out), "", 0))

    def test_values(self):
        # println renders each supported primitive using its runtime spelling.
        self.check("primitive values", 'CALL println value<int>:2 END CALL println value<float>:2.5 END CALL println value<str>:"ok" END CALL println value<bool>:True END', "2\n2.5\nok\ntrue\n")

    def test_operators(self):
        # Arithmetic, comparisons, equality, logical operators and unary signs.
        self.check("operator matrix", 'CALL println value<int>:2+3*4 END CALL println value<int>:(2+3)*4 END CALL println value<float>:7/2 END CALL println value<int>:2**3 END CALL println value<int>:7%%2 END CALL println value<int>:7%2 END CALL println value<bool>:1<2 END CALL println value<bool>:1<=1 END CALL println value<bool>:2>1 END CALL println value<bool>:2>=2 END CALL println value<bool>:1==1 END CALL println value<bool>:1!=2 END CALL println value<bool>:True&&False END CALL println value<bool>:True||False END CALL println value<bool>:!False END CALL println value<int>:+2 END CALL println value<int>:-2 END', "14\n20\n3.5\n8\n3\n1\ntrue\ntrue\ntrue\ntrue\ntrue\ntrue\nfalse\ntrue\ntrue\n2\n-2\n")

    def test_short_circuit(self):
        # A skipped rhs must not evaluate its division by zero.
        self.check("short circuit", 'CALL println value<bool>:False&&(1/0==0) END CALL println value<bool>:True||(1/0==0) END', "false\ntrue\n")

    def test_variables_scopes(self):
        # SET/GET and inner-scope shadowing retain the outer binding.
        self.check("scope shadowing", 'SET x<int> = 1\nIF True THEN\nSET x<int> = 2\nCALL println value<int>:GET x END\nEND\nCALL println value<int>:GET x END', "2\n1\n")

    def test_control(self):
        # IF, FOR, WHILE, BREAK and CONTINUE combine to print 3.
        self.check("control flow", 'SET i<int> = 0\nFOR SET j<int> = 0; GET j < 3; SET j = GET j + 1 THEN\nIF GET j == 1 THEN\nCONTINUE\nEND\nSET i = GET i + GET j\nEND\nWHILE GET i < 3 THEN\nSET i = GET i + 1\nBREAK\nEND\nCALL println value<int>:GET i END', "3\n")

    def test_conversion(self):
        # Numeric tag conversion truncates toward zero.
        self.check("int conversion", 'CALL println value<int>:CALL trunc x<int>:2.6 END END', "2\n")

    def test_all_literal_types_and_void_any(self):
        # Runtime Value alternatives are void, int, float, str, and bool; any forwards values.
        self.check("void and any", 'CALL println value<any>:CALL print value<str>:"" END END CALL println value<any>:7 END CALL println value<any>:2.25 END CALL println value<any>:"s" END CALL println value<any>:False END', "\n7\n2.25\ns\nfalse\n")

    def test_operator_precedence_and_types(self):
        # Tests right-associative powers, signs, boolean precedence and float-valued %%/%.
        self.check("precedence", 'CALL println value<float>:2**3**2 END CALL println value<float>:-2**2 END CALL println value<float>:2**-1 END CALL println value<float>:7%%2 END CALL println value<float>:7%2 END CALL println value<bool>:True||False&&False END CALL println value<int>:7-3-1 END', "512\n-4\n0.5\n3\n1\ntrue\n3\n")

    def test_strings_and_numeric_conversions(self):
        # Strings print intact; 7.9%2 intentionally truncates operands before remainder.
        self.check("string value and numeric conversion", 'CALL println value<str>:"ab" END CALL println value<float>:1+2.5 END CALL println value<float>:7.9%2 END', "ab\n3.5\n1\n")

    def test_else_forms(self):
        # ELSE THEN runs its branch; ELSE THEN IF is the supported else-if spelling.
        self.check("else", 'IF False THEN\nCALL println value<str>:"if" END\nELSE THEN\nCALL println value<str>:"else" END\nEND', "else\n")
        self.check("else then if", 'IF False THEN\nCALL println value<str>:"first" END\nELSE THEN IF True THEN\nCALL println value<str>:"second" END\nEND\nEND', "second\n")

    def test_loop_edges(self):
        # BREAK exits FOR, CONTINUE skips one WHILE iteration, nested loops total four.
        self.check("break in for", 'SET i<int> = 0\nFOR SET j<int> = 0; GET j < 5; SET j = GET j + 1 THEN\nSET i = GET i + 1\nBREAK\nEND\nCALL println value<int>:GET i END', "1\n")
        self.check("continue in while", 'SET i<int> = 0\nSET s<int> = 0\nWHILE GET i < 4 THEN\nSET i = GET i + 1\nIF GET i == 2 THEN\nCONTINUE\nEND\nSET s = GET s + GET i\nEND\nCALL println value<int>:GET s END', "8\n")
        self.check("nested loops", 'SET n<int> = 0\nFOR SET i<int> = 0; GET i < 2; SET i = GET i + 1 THEN\nFOR SET j<int> = 0; GET j < 2; SET j = GET j + 1 THEN\nSET n = GET n + 1\nEND\nEND\nCALL println value<int>:GET n END', "4\n")

    def test_negative_remainder_and_string_comparison(self):
        # Integer quotient/remainder truncate toward zero; matching strings support equality.
        self.check("negative quotient and remainder", 'CALL println value<float>:-7/2 END CALL println value<float>:-7%%2 END CALL println value<float>:-7%2 END CALL println value<float>:7%-2 END', "-3.5\n-3\n-1\n1\n")
        self.check("string equality", 'CALL println value<bool>:"a"=="a" END CALL println value<bool>:"a"!="b" END', "true\ntrue\n")

    def test_negative_int_conversion(self):
        # Conversion truncates toward zero, including negative values.
        self.check("negative int conversion", 'CALL println value<int>:CALL trunc x<int>:-2.6 END END', "-2\n")

    def test_examples_results(self):
        # Every documented non-error example is executed and checked against its captured output.
        self.assertTrue(EXPECTED.is_dir())
        executed = 0
        for source in sorted(EXAMPLES.glob("*.vlt")):
            if sys.platform == "win32" and source.name in SKIP_EXAMPLES_ON_WINDOWS:
                continue
            exp = EXPECTED / (source.stem + ".expected")
            if not exp.exists() or not exp.read_text(encoding="utf-8").startswith("OK\n"):
                continue
            with self.subTest(example=source.name):
                executed += 1
                p = subprocess.run([str(VALTC), "--file", str(source)], text=True, capture_output=True, env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0"})
                self.assertEqual(p.returncode, 0, p.stderr)
                expected_text = exp.read_text(encoding="utf-8")[3:]
                self.assertEqual(cut_floats(p.stdout), cut_floats(expected_text))
                self.assertEqual(p.stderr, "")
        self.assertGreater(executed, 0)


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