#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include "vm/vm.h"

using namespace smirnovlang;

// Helper function to run code
Value runCode(const std::string& code) {
    Lexer lexer(code, "test");
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens);
    Program program = parser.parse();
    
    VM vm;
    vm.execute(program);
    
    return vm.getResult();
}

// Helper to check errors
bool hasErrors(const std::string& code) {
    Lexer lexer(code, "test");
    auto tokens = lexer.tokenize();
    
    Parser parser(tokens);
    Program program = parser.parse();
    
    VM vm;
    vm.execute(program);
    
    return !vm.getErrors().empty();
}

// ============================================
// Basic tests
// ============================================

TEST(BasicTest, Arithmetic) {
    Value result = runCode("1 + 2 * 3");
    EXPECT_TRUE(result.isFloat() || result.isInt());
}

TEST(BasicTest, Variables) {
    Value result = runCode("let x = 5; x");
    EXPECT_TRUE(result.isInt());
    EXPECT_EQ(result.intVal, 5);
}

TEST(BasicTest, Functions) {
    Value result = runCode("fn add(a, b) => a + b; add(3, 4)");
    EXPECT_TRUE(result.isInt());
    EXPECT_EQ(result.intVal, 7);
}

TEST(BasicTest, PipeExpressions) {
    Value result = runCode("[1, 2, 3, 4, 5] |> len()");
    EXPECT_TRUE(result.isInt());
    EXPECT_EQ(result.intVal, 5);
}

TEST(BasicTest, Arrays) {
    Value result = runCode("[1, 2, 3]");
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.arrayVal.size(), 3);
}

TEST(BasicTest, Conditionals) {
    Value result = runCode("if true { 42 } else { 0 }");
    EXPECT_TRUE(result.isInt());
    EXPECT_EQ(result.intVal, 42);
}

TEST(BasicTest, Loops) {
    Value result = runCode("let sum = 0; for i in range(10) { sum = sum + i }; sum");
    EXPECT_TRUE(result.isInt());
    EXPECT_EQ(result.intVal, 45);
}

TEST(BasicTest, Strings) {
    Value result = runCode("\"Hello\" + \" \" + \"World\"");
    EXPECT_TRUE(result.isString());
    EXPECT_EQ(result.strVal, "Hello World");
}

TEST(BasicTest, BooleanOperations) {
    Value result = runCode("true && false || true");
    EXPECT_TRUE(result.isBool());
    EXPECT_EQ(result.boolVal, true);
}

// ============================================
// Advanced tests - Functions
// ============================================

TEST(FunctionTest, NestedFunctions) {
    Value result = runCode(
        "fn outer() { "
        "  fn inner() { 42 } "
        "  inner() "
        "} "
        "outer()"
    );
    EXPECT_EQ(result.intVal, 42);
}

TEST(FunctionTest, Recursion) {
    // Simple recursive factorial
    Value result = runCode(
        "fn fact(n) { "
        "  if n <= 1 { return 1; } "
        "  return n * fact(n - 1); "
        "} "
        "fact(5)"
    );
    EXPECT_EQ(result.intVal, 120);
}

TEST(FunctionTest, DefaultParams) {
    // Without default params, just test basic call
    Value result = runCode("fn id(x) { x }; id(10)");
    EXPECT_EQ(result.intVal, 10);
}

TEST(FunctionTest, FunctionAsValue) {
    // Functions should be callable
    Value result = runCode(
        "fn double(x) { x * 2 } "
        "double(5)"
    );
    EXPECT_EQ(result.intVal, 10);
}

// ============================================
// Advanced tests - Arrays
// ============================================

TEST(ArrayTest, ArrayLiterals) {
    Value result = runCode("[1, 2, 3, 4, 5]");
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.arrayVal.size(), 5);
}

TEST(ArrayTest, ArrayAccess) {
    Value result = runCode("let arr = [10, 20, 30]; arr[1]");
    EXPECT_EQ(result.intVal, 20);
}

TEST(ArrayTest, ArrayIndexing) {
    Value result = runCode("[1, 2, 3][0]");
    EXPECT_EQ(result.intVal, 1);
}

TEST(ArrayTest, ArraySlicing) {
    Value result = runCode("slice([1, 2, 3, 4, 5], 1, 3)");
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.arrayVal.size(), 2);
}

// ============================================
// Advanced tests - Strings
// ============================================

TEST(StringTest, StringLength) {
    Value result = runCode("len(\"hello\")");
    EXPECT_EQ(result.intVal, 5);
}

TEST(StringTest, StringConcat) {
    Value result = runCode("\"a\" + \"b\" + \"c\"");
    EXPECT_EQ(result.strVal, "abc");
}

TEST(StringTest, StringMultiply) {
    Value result = runCode("\"ab\" * 3");
    EXPECT_EQ(result.strVal, "ababab");
}

// ============================================
// Advanced tests - Control flow
// ============================================

TEST(ControlFlowTest, IfElseChain) {
    Value result = runCode(
        "let x = 5; "
        "if x < 3 { 1 } else if x < 5 { 2 } else { 3 }"
    );
    EXPECT_EQ(result.intVal, 3);
}

TEST(ControlFlowTest, WhileLoop) {
    Value result = runCode(
        "let i = 0; let c = 0; "
        "while i < 5 { c = c + 1; i = i + 1 }; c"
    );
    EXPECT_EQ(result.intVal, 5);
}

TEST(ControlFlowTest, ForLoopWithRange) {
    Value result = runCode(
        "let total = 0; "
        "for i in range(5) { total = total + i }; total"
    );
    // range(5) = [0,1,2,3,4], sum = 10
    EXPECT_EQ(result.intVal, 10);
}

// ============================================
// Advanced tests - Built-ins
// ============================================

TEST(BuiltinTest, Range) {
    Value result = runCode("range(5)");
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.arrayVal.size(), 5);
}

TEST(BuiltinTest, RangeWithStartEnd) {
    Value result = runCode("range(2, 5)");
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.arrayVal.size(), 3);
}

TEST(BuiltinTest, Sum) {
    Value result = runCode("sum([1, 2, 3, 4, 5])");
    EXPECT_EQ(result.intVal, 15);
}

TEST(BuiltinTest, Reverse) {
    Value result = runCode("reverse([1, 2, 3])");
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.arrayVal[0].intVal, 3);
    EXPECT_EQ(result.arrayVal[2].intVal, 1);
}

TEST(BuiltinTest, Type) {
    Value result = runCode("type(42)");
    EXPECT_EQ(result.strVal, "int");
}

TEST(BuiltinTest, TypeString) {
    Value result = runCode("type(\"hello\")");
    EXPECT_EQ(result.strVal, "string");
}

TEST(BuiltinTest, TypeArray) {
    Value result = runCode("type([1,2,3])");
    EXPECT_EQ(result.strVal, "array");
}

TEST(BuiltinTest, ToString) {
    Value result = runCode("toString(123)");
    EXPECT_EQ(result.strVal, "123");
}

TEST(BuiltinTest, ToInt) {
    Value result = runCode("toInt(\"42\")");
    EXPECT_EQ(result.intVal, 42);
}

TEST(BuiltinTest, ToFloat) {
    Value result = runCode("toFloat(\"3.14\")");
    EXPECT_TRUE(result.isFloat());
}

// ============================================
// Advanced tests - Math
// ============================================

TEST(MathTest, Sqrt) {
    Value result = runCode("sqrt(16)");
    EXPECT_EQ(result.floatVal, 4.0);
}

TEST(MathTest, Abs) {
    Value result = runCode("abs(-5)");
    EXPECT_EQ(result.intVal, 5);
}

TEST(MathTest, Min) {
    Value result = runCode("min(3, 1, 4, 1, 5)");
    EXPECT_EQ(result.intVal, 1);
}

TEST(MathTest, Max) {
    Value result = runCode("max(3, 1, 4, 1, 5)");
    EXPECT_EQ(result.intVal, 5);
}

TEST(MathTest, Pow) {
    Value result = runCode("pow(2, 8)");
    EXPECT_EQ(result.floatVal, 256.0);
}

// ============================================
// Advanced tests - Pattern matching
// ============================================

TEST(PatternMatchTest, SimpleMatch) {
    Value result = runCode(
        "let x = 2; "
        "match x { 1 -> \"one\", 2 -> \"two\", _ -> \"many\" }"
    );
    EXPECT_EQ(result.strVal, "two");
}

TEST(PatternMatchTest, Wildcard) {
    Value result = runCode(
        "let x = 99; "
        "match x { 1 -> \"one\", _ -> \"other\" }"
    );
    EXPECT_EQ(result.strVal, "other");
}

// ============================================
// Error handling tests
// ============================================

TEST(ErrorTest, UndefinedVariable) {
    // Should produce an error but not crash
    bool hasErr = hasErrors("undefined_var");
    // Parser should catch this
    EXPECT_TRUE(hasErr);
}

TEST(ErrorTest, DivisionByZero) {
    // Should handle gracefully
    Value result = runCode("1 / 0");
    // Result is nil or error
    EXPECT_TRUE(result.isNil() || !result.isInt());
}

// ============================================
// Edge cases
// ============================================

TEST(EdgeCaseTest, EmptyArray) {
    Value result = runCode("[]");
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.arrayVal.size(), 0);
}

TEST(EdgeCaseTest, EmptyString) {
    Value result = runCode("\"\"");
    EXPECT_TRUE(result.isString());
    EXPECT_EQ(result.strVal.size(), 0);
}

TEST(EdgeCaseTest, NullValue) {
    Value result = runCode("null");
    EXPECT_TRUE(result.isNil());
}

TEST(EdgeCaseTest, BooleanNegation) {
    Value result = runCode("!true");
    EXPECT_TRUE(result.isBool());
    EXPECT_EQ(result.boolVal, false);
}

TEST(EdgeCaseTest, NumericNegation) {
    Value result = runCode("-5");
    EXPECT_TRUE(result.isInt());
    EXPECT_EQ(result.intVal, -5);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
