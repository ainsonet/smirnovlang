#pragma once

#include "ast/ast.h"
#include <stack>
#include <unordered_map>
#include <vector>
#include <memory>
#include <functional>
#include <string>

namespace smirnovlang {

// Runtime value - unified type for everything
struct Value {
    enum class Tag {
        NIL, INT, FLOAT, BOOL, STRING, ARRAY, TUPLE, DICT,
        FUNCTION, CLOSURE, NATIVE_FN, OBJECT, TYPE
    } tag;
    
    union {
        long long intVal;
        double floatVal;
        bool boolVal;
    };
    std::string strVal;
    std::vector<Value> arrayVal;
    std::vector<Value> tupleVal;
    std::unordered_map<std::string, Value> dictVal;
    std::shared_ptr<struct FnValue> fnVal;
    std::shared_ptr<struct ClosureValue> closureVal;
    std::function<Value(const std::vector<Value>&)> nativeFn;
    std::shared_ptr<struct ObjectValue> objectVal;
    std::shared_ptr<struct TypeValue> typeVal;
    
    Value() : tag(Tag::NIL) {}
    Value(long long v) : tag(Tag::INT), intVal(v) {}
    Value(double v) : tag(Tag::FLOAT), floatVal(v) {}
    Value(bool v) : tag(Tag::BOOL), boolVal(v) {}
    Value(const std::string& v) : tag(Tag::STRING), strVal(v) {}
    Value(const char* v) : tag(Tag::STRING), strVal(v) {}
    
    bool isTruthy() const;
    std::string toString() const;
    Value clone() const;
    
    // Type checking
    bool isInt() const { return tag == Tag::INT; }
    bool isFloat() const { return tag == Tag::FLOAT; }
    bool isBool() const { return tag == Tag::BOOL; }
    bool isString() const { return tag == Tag::STRING; }
    bool isArray() const { return tag == Tag::ARRAY; }
    bool isNil() const { return tag == Tag::NIL; }
    bool isCallable() const {
        return tag == Tag::FUNCTION || tag == Tag::CLOSURE || tag == Tag::NATIVE_FN;
    }
    
    // Numeric operations
    Value operator+(const Value& other) const;
    Value operator-(const Value& other) const;
    Value operator*(const Value& other) const;
    Value operator/(const Value& other) const;
    
    // Comparison
    bool operator==(const Value& other) const;
    bool operator<(const Value& other) const;
};

struct FnValue {
    std::string name;
    std::vector<std::string> params;
    std::vector<std::shared_ptr<Stmt>> body;
    std::shared_ptr<Type> returnType;
    std::vector<FnStmt::Contract> contracts;
    std::shared_ptr<std::unordered_map<std::string, Value>> captures;
    bool isMemo = false;
    std::unordered_map<std::string, Value> memoCache;  // кэш для memoized функций
};

struct ClosureValue {
    std::shared_ptr<FnValue> fn;
    std::unordered_map<std::string, Value> env;
};

struct ObjectValue {
    std::string typeName;
    std::unordered_map<std::string, Value> fields;
};

struct TypeValue {
    std::string name;
    std::vector<std::pair<std::string, std::shared_ptr<Type>>> fields;
};

// Execution context
class VM {
public:
    VM();
    
    void execute(const Program& program);
    void execute(const std::vector<StmtPtr>& statements);
    
    Value evaluate(Expr* expr);
    
    // Built-in functions
    void registerBuiltins();
    
    // Get last result
    Value getResult() const { return result_; }
    const std::vector<std::string>& getErrors() const { return errors_; }

private:
    // Environment
    std::unordered_map<std::string, Value> globalEnv;
    std::vector<std::unordered_map<std::string, Value>> scopes;
    
    Value result_;
    std::vector<std::string> errors;
    
    // Helper methods
    void pushScope();
    void popScope();
    Value* getVar(const std::string& name);
    void setVar(const std::string& name, const Value& value);
    
    // Execution helpers
    void executeBlock(const std::vector<StmtPtr>& statements);
    Value executeFn(FnValue* fn, const std::vector<Value>& args);
    Value executeMatch(MatchExpr* match);
    
    // Error handling
    void error(const std::string& msg);
};

// Pretty print for debugging
std::string toDebugString(const Value& val);

} // namespace smirnovlang
