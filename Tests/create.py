from pathlib import Path

# ============================================================
# ValtCommand - Test Files Generator
# ============================================================

TESTS_DIR = Path("./")

TEST_FILES = {
    "arithmetic.vlt": r'''
// ============================================================
// ValtCommand - Arithmetic Tests
// ============================================================

CALL print value<str>:"========== ADDITION ==========" END
CALL print value<int>:10 + 5 END
CALL print value<int>:10 + (-5) END
CALL print value<int>:(-10) + (-5) END
CALL print value<float>:2.5 + 3.5 END

CALL print value<str>:"========== SUBTRACTION ==========" END
CALL print value<int>:10 - 5 END
CALL print value<int>:5 - 10 END
CALL print value<int>:(-5) - 3 END
CALL print value<float>:10.5 - 2.5 END

CALL print value<str>:"========== MULTIPLICATION ==========" END
CALL print value<int>:5 * 4 END
CALL print value<int>:(-5) * 4 END
CALL print value<int>:(-5) * (-4) END
CALL print value<float>:2.5 * 4 END

CALL print value<str>:"========== DIVISION ==========" END
CALL print value<int>:20 / 5 END
CALL print value<float>:7.0 / 2 END
CALL print value<int>:(-10) / 2 END
CALL print value<float>:10.0 / 4 END

CALL print value<str>:"========== INTEGER DIVISION ==========" END
CALL print value<int>:24 %% 5 END
CALL print value<int>:20 %% 4 END
CALL print value<int>:10 %% 3 END
CALL print value<int>:20 %% 3 * 2 END

CALL print value<str>:"========== MODULO ==========" END
CALL print value<int>:10 % 3 END
CALL print value<int>:20 % 5 END
CALL print value<int>:17 % 4 END
CALL print value<int>:2 + 10 % 4 END

CALL print value<str>:"========== POWER ==========" END
CALL print value<int>:2 ** 3 END
CALL print value<int>:3 ** 4 END
CALL print value<int>:2 ** 3 ** 2 END
CALL print value<int>:(-2) ** 2 END
CALL print value<int>:-2 ** 2 END

CALL print value<str>:"========== UNARY SIGNS ==========" END
CALL print value<int>:-5 END
CALL print value<int>:+5 END
CALL print value<int>:-(-5) END
CALL print value<float>:-2.5 END

CALL print value<str>:"========== ARITHMETIC TESTS COMPLETED ==========" END
''',

    "comparison.vlt": r'''
// ============================================================
// ValtCommand - Comparison Tests
// ============================================================

CALL print value<str>:"========== GREATER THAN ==========" END
CALL print value<bool>:5 > 3 END
CALL print value<bool>:3 > 5 END
CALL print value<bool>:5 > 5 END

CALL print value<str>:"========== LESS THAN ==========" END
CALL print value<bool>:3 < 5 END
CALL print value<bool>:5 < 3 END
CALL print value<bool>:5 < 5 END

CALL print value<str>:"========== GREATER OR EQUAL ==========" END
CALL print value<bool>:5 >= 5 END
CALL print value<bool>:6 >= 5 END
CALL print value<bool>:4 >= 5 END

CALL print value<str>:"========== LESS OR EQUAL ==========" END
CALL print value<bool>:5 <= 5 END
CALL print value<bool>:4 <= 5 END
CALL print value<bool>:6 <= 5 END

CALL print value<str>:"========== EQUALITY ==========" END
CALL print value<bool>:5 == 5 END
CALL print value<bool>:5 == 3 END
CALL print value<bool>:5 != 3 END
CALL print value<bool>:5 != 5 END

CALL print value<str>:"========== COMPARISON TESTS COMPLETED ==========" END
''',

    "logical.vlt": r'''
// ============================================================
// ValtCommand - Logical Tests
// ============================================================

CALL print value<str>:"========== LOGICAL NOT ==========" END
CALL print value<bool>:!True END
CALL print value<bool>:!False END

CALL print value<str>:"========== LOGICAL AND ==========" END
CALL print value<bool>:True && True END
CALL print value<bool>:True && False END
CALL print value<bool>:False && True END
CALL print value<bool>:False && False END

CALL print value<str>:"========== LOGICAL OR ==========" END
CALL print value<bool>:True || True END
CALL print value<bool>:True || False END
CALL print value<bool>:False || True END
CALL print value<bool>:False || False END

CALL print value<str>:"========== COMBINED LOGICAL EXPRESSIONS ==========" END
CALL print value<bool>:(5 > 3) && (10 > 2) END
CALL print value<bool>:(5 < 3) || (10 > 2) END
CALL print value<bool>:!(5 > 10) END
CALL print value<bool>:(5 > 3) && !(2 > 10) END

CALL print value<str>:"========== LOGICAL TESTS COMPLETED ==========" END
''',

    "precedence.vlt": r'''
// ============================================================
// ValtCommand - Operator Precedence Tests
// ============================================================

CALL print value<str>:"========== BASIC PRECEDENCE ==========" END
CALL print value<int>:2 + 3 * 4 END
CALL print value<int>:(2 + 3) * 4 END
CALL print value<int>:2 * 3 ** 2 END
CALL print value<int>:(2 + 3) ** 2 END

CALL print value<str>:"========== POWER ASSOCIATIVITY ==========" END
CALL print value<int>:2 ** 3 ** 2 END
CALL print value<int>:2 ** (3 ** 2) END
CALL print value<int>:(2 ** 3) ** 2 END

CALL print value<str>:"========== UNARY PRECEDENCE ==========" END
CALL print value<int>:-2 ** 2 END
CALL print value<int>:(-2) ** 2 END
CALL print value<float>:-2.5 ** 2 END
CALL print value<float>:(-2.5) ** 2 END

CALL print value<str>:"========== MIXED OPERATORS ==========" END
CALL print value<int>:2 + 3 * 2 ** 3 %% 5 END
CALL print value<int>:20 %% 3 * 2 END
CALL print value<int>:2 + 10 % 4 * 3 END

CALL print value<str>:"========== PRECEDENCE TESTS COMPLETED ==========" END
''',

    "variables.vlt": r'''
// ============================================================
// ValtCommand - Variable Tests
// ============================================================

CALL print value<str>:"========== VARIABLE DECLARATION ==========" END

SET a<int> = 10
SET b<int> = 5
SET price<float> = 12.5
SET enabled<bool> = True
SET message<str> = "Hello ValtCommand"

CALL print value<int>:GET a END
CALL print value<int>:GET b END
CALL print value<float>:GET price END
CALL print value<bool>:GET enabled END
CALL print value<str>:GET message END

CALL print value<str>:"========== VARIABLE ASSIGNMENT ==========" END

SET a = 20
CALL print value<int>:GET a END

SET a = (GET a) + 5
CALL print value<int>:GET a END

SET b = (GET b) * 2
CALL print value<int>:GET b END

CALL print value<str>:"========== VARIABLES IN EXPRESSIONS ==========" END

CALL print value<int>:(GET a) + (GET b) END
CALL print value<int>:(GET a) - (GET b) END
CALL print value<int>:(GET a) * (GET b) END
CALL print value<bool>:(GET a) > (GET b) END

CALL print value<str>:"========== VARIABLE TESTS COMPLETED ==========" END
''',

    "conditionals.vlt": r'''
// ============================================================
// ValtCommand - Conditional Tests
// ============================================================

CALL print value<str>:"========== BASIC IF ==========" END

IF 5 > 3 THEN
    CALL print value<str>:"IF passed" END
END

CALL print value<str>:"========== IF / ELSE ==========" END

IF 5 < 3 THEN
    CALL print value<str>:"Unexpected branch" END
ELSE THEN
    CALL print value<str>:"ELSE passed" END
END

CALL print value<str>:"========== AND CONDITION ==========" END

IF (5 > 3) && (10 > 2) THEN
    CALL print value<str>:"AND passed" END
ELSE THEN
    CALL print value<str>:"AND failed" END
END

CALL print value<str>:"========== OR CONDITION ==========" END

IF (5 < 3) || (10 > 2) THEN
    CALL print value<str>:"OR passed" END
ELSE THEN
    CALL print value<str>:"OR failed" END
END

CALL print value<str>:"========== NOT CONDITION ==========" END

IF !(5 > 10) THEN
    CALL print value<str>:"NOT passed" END
ELSE THEN
    CALL print value<str>:"NOT failed" END
END

CALL print value<str>:"========== NESTED CONDITIONS ==========" END

IF 10 > 5 THEN
    IF 8 > 3 THEN
        CALL print value<str>:"Nested IF passed" END
    ELSE THEN
        CALL print value<str>:"Inner ELSE executed" END
    END
ELSE THEN
    CALL print value<str>:"Outer ELSE executed" END
END

CALL print value<str>:"========== CONDITIONAL TESTS COMPLETED ==========" END
''',

    "loops.vlt": r'''
// ============================================================
// ValtCommand - Loop Tests
// ============================================================

CALL print value<str>:"========== BASIC WHILE ==========" END

SET counter<int> = 0

WHILE (GET counter) < 5 THEN
    CALL print value<int>:GET counter END
    SET counter = (GET counter) + 1
END

CALL print value<str>:"========== WHILE ACCUMULATION ==========" END

SET counter<int> = 0
SET total<int> = 0

WHILE (GET counter) < 5 THEN
    SET total = (GET total) + (GET counter)
    SET counter = (GET counter) + 1
END

CALL print value<int>:GET total END

CALL print value<str>:"========== MULTIPLICATION LOOP ==========" END

SET counter<int> = 1
SET result<int> = 1

WHILE (GET counter) < 6 THEN
    SET result = (GET result) * (GET counter)
    SET counter = (GET counter) + 1
END

CALL print value<int>:GET result END

CALL print value<str>:"========== LOOP TESTS COMPLETED ==========" END
''',

    "functions.vlt": r'''
// ============================================================
// ValtCommand - Function Tests
// ============================================================

FUNCTION add (
    a<int>
    b<int>
)->int
    RETURN (GET a) + (GET b)
END

FUNCTION multiply (
    a<int>
    b<int>
)->int
    RETURN (GET a) * (GET b)
END

FUNCTION subtract (
    a<int>
    b<int>
)->int
    RETURN (GET a) - (GET b)
END

CALL print value<str>:"========== BASIC FUNCTION CALLS ==========" END

CALL print value<int>:CALL add a<int>:10 b<int>:20 END END
CALL print value<int>:CALL multiply a<int>:6 b<int>:7 END END
CALL print value<int>:CALL subtract a<int>:10 b<int>:3 END END

CALL print value<str>:"========== NESTED FUNCTION CALLS ==========" END

CALL print value<int>:CALL add a<int>:CALL multiply a<int>:2 b<int>:3 END b<int>:4 END END

CALL print value<str>:"========== FUNCTION TESTS COMPLETED ==========" END
''',

    "errors.vlt": r'''
// ============================================================
// ValtCommand - Error Handling Tests
// File: errors.vlt
//
// Uncomment ONE invalid test at a time.
// Invalid cases may stop execution.
// ============================================================

CALL print value<str>:"========== ERROR TESTS ==========" END

// Test 1: Undefined variable
// CALL print value<int>:GET undefined_variable END

// Test 2: Division by zero
// CALL print value<int>:10 / 0 END

// Test 3: Integer division by zero
// CALL print value<int>:10 %% 0 END

// Test 4: Modulo by zero
// CALL print value<int>:10 % 0 END

// Test 5: Wrong argument type
//
// FUNCTION expects_int (
//     number<int>
// )->int
//     RETURN GET number
// END
//
// CALL print value<int>:CALL expects_int number<str>:"hello" END END

// Test 6: Undefined function
// CALL print value<int>:CALL missing_function value<int>:10 END END

// Test 7: Invalid expression
// CALL print value<int>:10 + * 5 END

// Test 8: Missing closing parenthesis
// CALL print value<int>:(10 + 5 END

CALL print value<str>:"========== ERROR TEST CASES DOCUMENTED ==========" END
''',

    "complex_expressions.vlt": r'''
// ============================================================
// ValtCommand - Complex Expression Tests
// ============================================================

CALL print value<str>:"========== COMPLEX EXPRESSION 1 ==========" END
CALL print value<int>:(10 + 5) * (8 - 3) END
// Expected: 75

CALL print value<str>:"========== COMPLEX EXPRESSION 2 ==========" END
CALL print value<int>:((2 ** 3) + (4 * 5) - 6) / 2 END
// Expected: 11

CALL print value<str>:"========== COMPLEX EXPRESSION 3 ==========" END
CALL print value<int>:(((((12 + 8) * 3 ** 2) - (144 %% 4)) + ((7 ** 3) % 10) - (-5 * 6)) * 2 + (100 %% (2 ** 3))) END
// Expected: 366

CALL print value<str>:"========== COMPLEX EXPRESSION 4 ==========" END
CALL print value<int>:2 + 3 * 2 ** 3 %% 5 END
// Expected: 6

CALL print value<str>:"========== COMPLEX EXPRESSION 5 ==========" END
CALL print value<int>:((10 - 4) * 3) + (2 ** 4) END
// Expected: 34

CALL print value<str>:"========== COMPLEX EXPRESSION 6 ==========" END
CALL print value<bool>:((10 > 5) && (3 < 8)) || (2 == 7) END
// Expected: True

CALL print value<str>:"========== COMPLEX EXPRESSION 7 ==========" END
CALL print value<int>:((20 %% 6) + (17 % 5)) * 2 END
// Expected: 12

CALL print value<str>:"========== COMPLEX EXPRESSION TESTS COMPLETED ==========" END
''',
}


def main() -> None:
    TESTS_DIR.mkdir(parents=True, exist_ok=True)

    for filename, content in TEST_FILES.items():
        path = TESTS_DIR / filename
        path.write_text(
            content.strip() + "\n",
            encoding="utf-8",
        )
        print(f"[CREATED] {path}")

    print()
    print(f"Created {len(TEST_FILES)} ValtCommand test files.")
    print(f"Directory: {TESTS_DIR.resolve()}")


if __name__ == "__main__":
    main()
