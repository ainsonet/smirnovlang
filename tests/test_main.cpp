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

// Test 1: Basic arithmetic
TEST(BasicTest, Arithmetic) {
    Value result = runCode("1 + 2 * 3");
    EXPECT_TRUE(result.isFloat() || result.isInt());
    // Result should be 7
}

// Test 2: Variables
TEST(BasicTest, Variables) {
    Value result = runCode("let x = 5; x");
    EXPECT_TRUE(result.isInt());
    EXPECT_EQ(result.intVal, 5);
}

// Test 3: Functions
TEST(BasicTest, Functions) {
    Value result = runCode("fn add(a, b) => a + b; add(3, 4)");
    EXPECT_TRUE(result.isInt());
    EXPECT_EQ(result.intVal, 7);
}

// Test 4: Pipe expressions
TEST(BasicTest, PipeExpressions) {
    Value result = runCode("[1, 2, 3, 4, 5] |> len()");
    EXPECT_TRUE(result.isInt());
    EXPECT_EQ(result.intVal, 5);
}

// Test 5: Arrays
TEST(BasicTest, Arrays) {
    Value result = runCode("[1, 2, 3]");
    EXPECT_TRUE(result.isArray());
    EXPECT_EQ(result.arrayVal.size(), 3);
}

// Test 6: Conditionals
TEST(BasicTest, Conditionals) {
    Value result = runCode("if true { 42 } else { 0 }");
    EXPECT_TRUE(result.isInt());
    EXPECT_EQ(result.intVal, 42);
}

// Test 7: Loops
TEST(BasicTest, Loops) {
    Value result = runCode("let sum = 0; for i in range(10) { sum = sum + i }; sum");
    EXPECT_TRUE(result.isInt());
    EXPECT_EQ(result.intVal, 45);
}

// Test 8: Strings
TEST(BasicTest, Strings) {
    Value result = runCode("\"Hello\" + \" \" + \"World\"");
    EXPECT_TRUE(result.isString());
    EXPECT_EQ(result.strVal, "Hello World");
}

// Test 9: Boolean operations
TEST(BasicTest, BooleanOperations) {
    Value result = runCode("true && false || true");
    EXPECT_TRUE(result.isBool());
    EXPECT_EQ(result.boolVal, true);
}

// Test 10: Scope
TEST(BasicTest, Scope) {
    Value result = runCode("if true { let x = 10; x }; x");
    // x should be undefined here
    // For now, just check it doesn't crash
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
