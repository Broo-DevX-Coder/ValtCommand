"""Error formatting and CLI failure behavior checks for valtc."""
import pathlib
import subprocess
import os
import sys
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
VALTC = pathlib.Path(os.environ.get("VALTC_BIN", str(ROOT / "Code" / "build" / "bin" / "valtc")))
EXAMPLES = ROOT / 'DOCs' / 'V1.0.0' / 'Language-references' / 'examples'
EXPECTED = EXAMPLES.parent / 'exceptions'


def run(*args):
    env = os.environ.copy(); env["ASAN_OPTIONS"] = "detect_leaks=0"
    return subprocess.run([str(VALTC), *map(str, args)], text=True, capture_output=True, env=env)


class Errors(unittest.TestCase):
    def test_eval_syntax(self):
        # Unexpected END is a SyntaxError at zero-based source position.
        p = run("--eval", "END")
        self.assertEqual((p.stdout, p.returncode), ("", 2))
        self.assertIn("SyntaxError:", p.stderr)
        self.assertIn("1-|  END", p.stderr)
        self.assertNotIn("File:", p.stderr)

    def test_eval_runtime(self):
        # Division by zero is reported with its runtime category and source.
        p = run("--eval", "CALL println value<int>:1/0 END")
        self.assertEqual((p.stdout, p.returncode), ("", 2))
        self.assertIn("RuntimeError: division by zero", p.stderr)
        self.assertIn("1-|  CALL println", p.stderr)

    def test_file_error_format(self):
        # File errors include source context and the absolute File line.
        with tempfile.TemporaryDirectory() as d:
            f = pathlib.Path(d) / "bad.vlt"
            f.write_text('SET x<int> = "bad"\n')
            p = run("--file", f)
            self.assertEqual((p.stdout, p.returncode), ("", 2))
            self.assertIn("TypeError:", p.stderr)
            self.assertIn('1-|  SET x<int> = "bad"', p.stderr)
            self.assertIn(f'File: "{f}"', p.stderr)

    def test_cli_no_work_and_missing_file(self):
        # No source is exit 1; a bad --file path is an explicit open error with exit 3.
        p = run()
        self.assertEqual((p.stdout, p.stderr, p.returncode), ("", "No work to do ...\n", 1))
        missing = ROOT / "missing.vlt"
        p = run("--file", missing)
        self.assertEqual((p.stdout, p.stderr, p.returncode), ("", f'Cannot open file: "{missing}"\n', 3))

    def test_cli_no_help(self):
        # No arguments do not print parser help.
        p = run()
        self.assertNotIn("Usage:", p.stdout + p.stderr)

    def test_limitation_break_continue_top_level(self):
        # limitation: top-level BREAK and CONTINUE are accepted and ignored.
        p = run("--eval", 'BREAK\nCONTINUE\nCALL println value<str>:"ok" END')
        self.assertEqual((p.stdout, p.stderr, p.returncode), ("ok\n", "", 0))

    def test_error_categories_from_implementation(self):
        # Representative core and library failures cover each emitted diagnostic category.
        cases = [('SET x<int> = "s"', 'TypeError'), ('GET missing', 'NameError'),
                 ('CALL print value<int>:1 END CALL print value<int>:2 extra<int>:3 END', 'ArgumentError'),
                 ('CALL println value<int>:True END', 'TypeError'),
                 ('CALL println value<int>:1<"x" END', 'IncompatibleTypesError'),
                 ('SET x<int> = 1\nSET x<int> = 2', 'VariableError'),
                 ('SET_CONST x<int> = 1\nSET x = 2', 'ImmutableError'),
                 ('FUNCTION f()->int\nRETURN "x"\nEND\nCALL f END', 'ReturnTypeError'),
                 ('FUNCTION f()->int\nCALL print value<str>:"x" END\nEND', 'MissingReturnError'),
                 ('FUNCTION f()->void\nEND\nFUNCTION f()->void\nEND', 'FunctionError'),
                 ('CALL sqrt x<float>:-1 END', 'ValueError'),
                 ('CALL pow base<int>:2 exp<int>:9223372036854775807 END', 'OverflowError')]
        for code, kind in cases:
            with self.subTest(category=kind, source=code[:30]):
                p = run('--eval', code)
                self.assertEqual((p.stdout, p.returncode), ('', 2))
                self.assertIn(kind, p.stderr)

    def test_full_diagnostics_positions(self):
        # snapshot of current caret placement, not a specification
        # Check exact message, zero-based source location rendered as one-based text, and caret.
        cases = [
            ('IF 1 THEN\nEND', "\nTypeError: expected 'bool', got 'int'\n------------------------------------------\n1-|  IF 1 THEN\n2-|  END\n--|     ^\n"),
            ('WHILE 1 THEN\nEND', "\nTypeError: expected 'bool', got 'int'\n------------------------------------------\n1-|  WHILE 1 THEN\n2-|  END\n--|  ^^^^^\n------------------------------------------\nCondition of while loop must be boolean\n\n"),
            ('SET x<int> = GET NAN', "\nOverflowError: cannot convert non-finite or out-of-range float to int\n------------------------------------------\n1-|  SET x<int> = GET NAN\n\n"),
        ]
        for code, expected in cases:
            with self.subTest(source=code):
                p = run('--eval', code)
                self.assertEqual((p.stdout, p.stderr, p.returncode), ('', expected, 2))

    def test_else_if_hint(self):
        # ELSE IF is rejected with the parser's explicit replacement hint.
        code = 'IF False THEN\nCALL println value<str>:"a" END\nELSE IF True THEN\nCALL println value<str>:"b" END\nEND'
        p = run('--eval', code)
        self.assertEqual(p.returncode, 2)
        self.assertIn("Use `ELSE THEN IF ... END` for an else-if branch.", p.stderr)

    def test_multibyte_utf8_lexer_error(self):
        # A multibyte UTF-8 identifier is consumed as one unknown token and diagnosed by the parser.
        code = 'é'
        p = run('--eval', code)
        self.assertEqual((p.stdout, p.returncode), ('', 2))
        self.assertEqual(p.stderr, "\nSyntaxError: unexpected token 'é'\n------------------------------------------\n1-|  é\n\n")

    def test_any_runtime_conversion_errors(self):
        # An any binding carrying str, bool, or void fails when assigned to int.
        cases = [('"x"', "cannot convert 'str' to 'int'"),
                 ('True', "cannot convert 'bool' to 'int'"),
                 ('CALL print value<str>:"" END', "cannot convert 'void' to 'int'")]
        for value, message in cases:
            code = f'SET a<any> = {value}\nSET i<int> = GET a'
            with self.subTest(payload=value):
                p = run('--eval', code)
                self.assertEqual((p.stdout, p.returncode), ('', 2))
                self.assertIn(f'TypeError: {message}', p.stderr)
                self.assertIn('2-|  SET i<int> = GET a', p.stderr)

    def test_float_to_int_overflow_variants(self):
        # NAN, INF, and a finite out-of-range value all report the conversion overflow.
        for value in ('GET NAN', 'GET INF', 'GET FLOAT_MAX'):
            with self.subTest(value=value):
                p = run('--eval', f'SET x<int> = {value}')
                self.assertEqual(p.returncode, 2)
                self.assertIn('OverflowError: cannot convert non-finite or out-of-range float to int', p.stderr)

    def test_lexer_failures(self):
        # Unterminated string, block comment and unsupported character are parser diagnostics.
        for code in ('CALL println value<str>:"unfinished END', '/* never closed', 'CALL println value<int>:1 @ END'):
            with self.subTest(source=code):
                p = run('--eval', code)
                self.assertEqual((p.stdout, p.returncode), ('', 2))
                self.assertIn('SyntaxError:', p.stderr)

    def test_cli_errors(self):
        # Unknown option, missing option value and mutually exclusive sources use CLI11 codes.
        cases = [(('--unknown',), 109), (('--file',), 114),
                 (('--file', ROOT / 'Code' / 'app' / 'main.cpp', '--eval', 'x'), 108)]
        for args, code in cases:
            with self.subTest(args=args):
                p = run(*args)
                self.assertEqual((p.returncode, p.stdout), (code, ''))
                self.assertNotEqual(p.stderr, '')

    def test_file_io_error(self):
        # A directory is an invalid --file path and follows the CLI open-error branch.
        with tempfile.TemporaryDirectory() as d:
            p = run('--file', d)
            # A directory is not a regular source file and gets the CLI open diagnostic.
            self.assertEqual((p.stdout, p.stderr, p.returncode), ('', f'Cannot open file: "{d}"\n', 3))

    def test_error_example_fixtures(self):
        # Every expected-error example is compared byte-for-byte after portable path substitution.
        self.assertTrue(EXPECTED.is_dir())
        executed = 0
        for source in sorted(EXAMPLES.glob('*.vlt')):
            exp = EXPECTED / (source.stem + '.expected')
            if not exp.exists() or not exp.read_text(encoding='utf-8').startswith('ERROR\n'):
                continue
            with self.subTest(example=source.name):
                executed += 1
                p = run('--file', source)
                self.assertEqual(p.returncode, 2)
                actual = ('ERROR\n' + p.stdout + p.stderr).replace(str(source.resolve()), f'<exemples_path>/{source.name}')
                self.assertEqual(actual, exp.read_text(encoding='utf-8'))
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
