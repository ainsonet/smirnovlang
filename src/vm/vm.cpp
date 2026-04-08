#include "vm/vm.h"
#include "ast/ast.h"
#include <cmath>
#include <sstream>
#include <algorithm>

namespace smirnovlang {

// Value implementation
bool Value::isTruthy() const {
    switch (tag) {
        case Tag::NIL: return false;
        case Tag::BOOL: return boolVal;
        case Tag::INT: return intVal != 0;
        case Tag::FLOAT: return floatVal != 0.0;
        case Tag::STRING: return !strVal.empty();
        case Tag::ARRAY: return !arrayVal.empty();
        case Tag::DICT: return !dictVal.empty();
        default: return true;
    }
}

Value Value::clone() const {
    Value v;
    v.tag = tag;
    v.intVal = intVal;
    v.floatVal = floatVal;
    v.boolVal = boolVal;
    v.strVal = strVal;
    v.arrayVal = arrayVal;
    v.dictVal = dictVal;
    v.fnVal = fnVal;
    v.closureVal = closureVal;
    v.nativeFn = nativeFn;
    v.objectVal = objectVal;
    v.typeVal = typeVal;
    return v;
}

std::string Value::toString() const {
    switch (tag) {
        case Tag::NIL: return "null";
        case Tag::INT: return std::to_string(intVal);
        case Tag::FLOAT: return std::to_string(floatVal);
        case Tag::BOOL: return boolVal ? "true" : "false";
        case Tag::STRING: return strVal;
        case Tag::ARRAY: {
            std::string s = "[";
            for (size_t i = 0; i < arrayVal.size(); ++i) {
                s += arrayVal[i].toString();
                if (i < arrayVal.size() - 1) s += ", ";
            }
            s += "]";
            return s;
        }
        case Tag::FUNCTION: return "<function " + fnVal->name + ">";
        case Tag::CLOSURE: return "<closure " + closureVal->fn->name + ">";
        case Tag::NATIVE_FN: return "<native_fn>";
        default: return "<value>";
    }
}

Value Value::operator+(const Value& other) const {
    if (tag == Tag::INT && other.tag == Tag::INT) {
        return Value(intVal + other.intVal);
    }
    if ((tag == Tag::INT || tag == Tag::FLOAT) && 
        (other.tag == Tag::INT || other.tag == Tag::FLOAT)) {
        double a = (tag == Tag::INT) ? intVal : floatVal;
        double b = (other.tag == Tag::INT) ? other.intVal : other.floatVal;
        return Value(a + b);
    }
    if (tag == Tag::STRING || other.tag == Tag::STRING) {
        return Value(toString() + other.toString());
    }
    if (tag == Tag::ARRAY && other.tag == Tag::ARRAY) {
        Value result;
        result.tag = Tag::ARRAY;
        result.arrayVal = arrayVal;
        result.arrayVal.insert(result.arrayVal.end(), 
                               other.arrayVal.begin(), other.arrayVal.end());
        return result;
    }
    return Value();
}

Value Value::operator-(const Value& other) const {
    if (tag == Tag::INT && other.tag == Tag::INT) {
        return Value(intVal - other.intVal);
    }
    if ((tag == Tag::INT || tag == Tag::FLOAT) && 
        (other.tag == Tag::INT || other.tag == Tag::FLOAT)) {
        double a = (tag == Tag::INT) ? intVal : floatVal;
        double b = (other.tag == Tag::INT) ? other.intVal : other.floatVal;
        return Value(a - b);
    }
    return Value();
}

Value Value::operator*(const Value& other) const {
    if (tag == Tag::INT && other.tag == Tag::INT) {
        return Value(intVal * other.intVal);
    }
    if ((tag == Tag::INT || tag == Tag::FLOAT) && 
        (other.tag == Tag::INT || other.tag == Tag::FLOAT)) {
        double a = (tag == Tag::INT) ? intVal : floatVal;
        double b = (other.tag == Tag::INT) ? other.intVal : other.floatVal;
        return Value(a * b);
    }
    if (tag == Tag::STRING && other.tag == Tag::INT) {
        std::string result;
        for (long long i = 0; i < other.intVal; ++i) {
            result += strVal;
        }
        return Value(result);
    }
    return Value();
}

Value Value::operator/(const Value& other) const {
    if ((tag == Tag::INT || tag == Tag::FLOAT) && 
        (other.tag == Tag::INT || other.tag == Tag::FLOAT)) {
        double a = (tag == Tag::INT) ? intVal : floatVal;
        double b = (other.tag == Tag::INT) ? other.intVal : other.floatVal;
        if (b == 0) return Value();
        return Value(a / b);
    }
    return Value();
}

bool Value::operator==(const Value& other) const {
    if (tag != other.tag) return false;
    switch (tag) {
        case Tag::NIL: return true;
        case Tag::INT: return intVal == other.intVal;
        case Tag::FLOAT: return std::abs(floatVal - other.floatVal) < 0.0001;
        case Tag::BOOL: return boolVal == other.boolVal;
        case Tag::STRING: return strVal == other.strVal;
        default: return false;
    }
}

bool Value::operator<(const Value& other) const {
    if (tag == Tag::INT && other.tag == Tag::INT) {
        return intVal < other.intVal;
    }
    if ((tag == Tag::INT || tag == Tag::FLOAT) && 
        (other.tag == Tag::INT || other.tag == Tag::FLOAT)) {
        double a = (tag == Tag::INT) ? intVal : floatVal;
        double b = (other.tag == Tag::INT) ? other.intVal : other.floatVal;
        return a < b;
    }
    if (tag == Tag::STRING && other.tag == Tag::STRING) {
        return strVal < other.strVal;
    }
    return false;
}

// VM implementation
VM::VM() {
    registerBuiltins();
    pushScope();
}

void VM::pushScope() {
    scopes.emplace_back();
}

void VM::popScope() {
    if (!scopes.empty()) {
        scopes.pop_back();
    }
}

Value* VM::getVar(const std::string& name) {
    // Search from innermost to outermost
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto varIt = it->find(name);
        if (varIt != it->end()) {
            return &varIt->second;
        }
    }
    
    // Check global
    auto globalIt = globalEnv.find(name);
    if (globalIt != globalEnv.end()) {
        return &globalIt->second;
    }
    
    return nullptr;
}

void VM::setVar(const std::string& name, const Value& value) {
    // Update in existing scope if exists
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->count(name)) {
            (*it)[name] = value;
            return;
        }
    }
    
    // Otherwise set in global
    globalEnv[name] = value;
}

void VM::error(const std::string& msg) {
    errors.push_back(msg);
}

void VM::execute(const Program& program) {
    executeBlock(program.statements);
}

void VM::execute(const std::vector<StmtPtr>& statements) {
    executeBlock(statements);
}

void VM::executeBlock(const std::vector<StmtPtr>& statements) {
    for (const auto& stmt : statements) {
        if (auto* letStmt = dynamic_cast<LetStmt*>(stmt.get())) {
            if (letStmt->init) {
                Value initVal = evaluate(letStmt->init.get());
                scopes.back()[letStmt->name] = initVal;
            } else {
                scopes.back()[letStmt->name] = Value();
            }
        }
        else if (auto* fnStmt = dynamic_cast<FnStmt*>(stmt.get())) {
            Value fnVal;
            fnVal.tag = Value::Tag::FUNCTION;
            fnVal.fnVal = std::make_shared<FnValue>();
            fnVal.fnVal->name = fnStmt->name;
            fnVal.fnVal->params = {};
            for (auto& p : fnStmt->params) {
                fnVal.fnVal->params.push_back(p.first);
            }
            fnVal.fnVal->body = fnStmt->body;
            fnVal.fnVal->returnType = fnStmt->returnType;
            fnVal.fnVal->contracts = fnStmt->contracts;
            
            globalEnv[fnStmt->name] = fnVal;
        }
        else if (auto* exprStmt = dynamic_cast<ExprStmt*>(stmt.get())) {
            result_ = evaluate(exprStmt->expr.get());
        }
        else if (auto* pipeAssign = dynamic_cast<PipeAssignStmt*>(stmt.get())) {
            // Evaluate pipeline and assign to variable
            Value result = evaluate(pipeAssign->pipeline.get());
            scopes.back()[pipeAssign->varName] = result;
        }
        else if (auto* retStmt = dynamic_cast<ReturnStmt*>(stmt.get())) {
            if (retStmt->value) {
                result_ = evaluate(retStmt->value.get());
            } else {
                result_ = Value();
            }
            return;  // Exit current block
        }
        else if (auto* forStmt = dynamic_cast<ForStmt*>(stmt.get())) {
            Value iterable = evaluate(forStmt->iterable.get());
            
            if (iterable.isArray()) {
                for (const auto& item : iterable.arrayVal) {
                    scopes.back()[forStmt->varName] = item;
                    executeBlock({forStmt->body});
                }
            } else if (iterable.isInt()) {
                for (long long i = 0; i < iterable.intVal; ++i) {
                    scopes.back()[forStmt->varName] = Value(i);
                    executeBlock({forStmt->body});
                }
            }
        }
        else if (auto* ifStmt = dynamic_cast<IfStmt*>(stmt.get())) {
            Value cond = evaluate(ifStmt->condition.get());
            if (cond.isTruthy()) {
                executeBlock({ifStmt->thenBranch});
            } else if (ifStmt->elseBranch) {
                executeBlock({ifStmt->elseBranch});
            }
        }
        else if (auto* matchStmt = dynamic_cast<MatchStmt*>(stmt.get())) {
            Value matchVal = evaluate(matchStmt->value.get());
            bool matched = false;
            
            for (auto& caseStmt : matchStmt->cases) {
                for (auto& pattern : caseStmt.patterns) {
                    Value patVal = evaluate(pattern.get());
                    if (matchVal == patVal) {
                        executeBlock({caseStmt.body});
                        matched = true;
                        break;
                    }
                }
                if (matched) break;
            }
        }
    }
}

Value VM::evaluate(Expr* expr) {
    if (!expr) return Value();
    
    if (auto* literal = dynamic_cast<LiteralExpr*>(expr)) {
        if (auto* v = std::get_if<long long>(&literal->value)) {
            return Value(*v);
        }
        if (auto* v = std::get_if<double>(&literal->value)) {
            return Value(*v);
        }
        if (auto* v = std::get_if<std::string>(&literal->value)) {
            return Value(*v);
        }
        if (auto* v = std::get_if<bool>(&literal->value)) {
            return Value(*v);
        }
        return Value();
    }
    
    if (auto* ident = dynamic_cast<IdentifierExpr*>(expr)) {
        if (auto* var = getVar(ident->name)) {
            return var->clone();
        }
        error("Undefined variable: " + ident->name);
        return Value();
    }
    
    if (auto* binary = dynamic_cast<BinaryExpr*>(expr)) {
        Value left = evaluate(binary->left.get());
        Value right = evaluate(binary->right.get());
        
        switch (binary->op) {
            case TokenType::PLUS: return left + right;
            case TokenType::MINUS: return left - right;
            case TokenType::STAR: return left * right;
            case TokenType::SLASH: return left / right;
            case TokenType::EQEQ: return Value(left == right);
            case TokenType::BANGEQ: return Value(!(left == right));
            case TokenType::LT: return Value(left < right);
            case TokenType::LTE: return Value(!(right < left));
            case TokenType::GT: return Value(right < left);
            case TokenType::GTE: return Value(!(left < right));
            default: return Value();
        }
    }
    
    if (auto* unary = dynamic_cast<UnaryExpr*>(expr)) {
        Value operand = evaluate(unary->operand.get());
        
        switch (unary->op) {
            case TokenType::NOT: return Value(!operand.isTruthy());
            case TokenType::MINUS: 
                if (operand.isInt()) return Value(-operand.intVal);
                if (operand.isFloat()) return Value(-operand.floatVal);
                return Value();
            default: return Value();
        }
    }
    
    if (auto* call = dynamic_cast<CallExpr*>(expr)) {
        Value callee = evaluate(call->callee.get());
        
        std::vector<Value> args;
        for (auto& arg : call->args) {
            args.push_back(evaluate(arg.get()));
        }
        
        if (callee.tag == Value::Tag::NATIVE_FN) {
            return callee.nativeFn(args);
        }
        
        if (callee.tag == Value::Tag::FUNCTION) {
            return executeFn(callee.fnVal.get(), args);
        }
        
        if (callee.tag == Value::Tag::CLOSURE) {
            pushScope();
            for (size_t i = 0; i < callee.closureVal->fn->params.size() && i < args.size(); ++i) {
                scopes.back()[callee.closureVal->fn->params[i]] = args[i];
            }
            // Add captured variables
            for (auto& [name, val] : callee.closureVal->env) {
                scopes.back()[name] = val;
            }
            executeBlock(callee.closureVal->fn->body);
            popScope();
            return result_;
        }
        
        error("Cannot call value of type " + std::to_string((int)callee.tag));
        return Value();
    }
    
    if (auto* lambda = dynamic_cast<LambdaExpr*>(expr)) {
        Value closure;
        closure.tag = Value::Tag::CLOSURE;
        closure.closureVal = std::make_shared<ClosureValue>();
        closure.closureVal->fn = std::make_shared<FnValue>();
        closure.closureVal->fn->params = lambda->params;
        closure.closureVal->fn->body = lambda->body;
        closure.closureVal->fn->returnType = std::make_shared<Type>(lambda->returnType);
        
        // Capture current scope
        for (auto& scope : scopes) {
            for (auto& [name, val] : scope) {
                closure.closureVal->env[name] = val.clone();
            }
        }
        
        return closure;
    }
    
    if (auto* array = dynamic_cast<ArrayExpr*>(expr)) {
        Value arr;
        arr.tag = Value::Tag::ARRAY;
        for (auto& elem : array->elements) {
            arr.arrayVal.push_back(evaluate(elem.get()));
        }
        return arr;
    }
    
    if (auto* tuple = dynamic_cast<TupleExpr*>(expr)) {
        Value t;
        t.tag = Value::Tag::TUPLE;
        for (auto& elem : tuple->elements) {
            t.tupleVal.push_back(evaluate(elem.get()));
        }
        return t;
    }
    
    if (auto* pipe = dynamic_cast<PipeExpr*>(expr)) {
        Value left = evaluate(pipe->left.get());
        
        // Build a call expression with left as first argument
        auto call = std::make_unique<CallExpr>();
        call->callee = std::move(pipe->right);
        call->args.push_back(std::make_unique<IdentifierExpr>());  // placeholder
        
        // Evaluate the call with left as the first argument
        // For now, simplify: just evaluate the right side as a function
        return evaluate(pipe->right.get());
    }
    
    if (auto* match = dynamic_cast<MatchExpr*>(expr)) {
        return executeMatch(match);
    }
    
    if (auto* index = dynamic_cast<IndexExpr*>(expr)) {
        Value obj = evaluate(index->object.get());
        Value idx = evaluate(index->index.get());
        
        if (obj.isArray() && idx.isInt()) {
            size_t pos = idx.intVal;
            if (pos < obj.arrayVal.size()) {
                return obj.arrayVal[pos].clone();
            }
            error("Index out of bounds");
        }
        
        if (obj.isString() && idx.isInt()) {
            size_t pos = idx.intVal;
            if (pos < obj.strVal.size()) {
                return Value(std::string(1, obj.strVal[pos]));
            }
            error("Index out of bounds");
        }
        
        return Value();
    }
    
    if (auto* member = dynamic_cast<MemberExpr*>(expr)) {
        Value obj = evaluate(member->object.get());
        
        if (obj.tag == Value::Tag::OBJECT) {
            auto it = obj.objectVal->fields.find(member->member);
            if (it != obj.objectVal->fields.end()) {
                return it->second.clone();
            }
        }
        
        return Value();
    }
    
    // SQL-like query expression
    if (auto* query = dynamic_cast<QueryExpr*>(expr)) {
        return executeQuery(query);
    }
    
    return Value();
}

Value VM::executeFn(FnValue* fn, const std::vector<Value>& args) {
    pushScope();
    
    for (size_t i = 0; i < fn->params.size() && i < args.size(); ++i) {
        scopes.back()[fn->params[i]] = args[i];
    }
    
    // Execute contracts (require)
    for (auto& contract : fn->contracts) {
        if (contract.kind == FnStmt::Contract::REQUIRE) {
            Value cond = evaluate(contract.condition.get());
            if (!cond.isTruthy()) {
                error("Contract failed: " + contract.message);
                popScope();
                return Value();
            }
        }
    }
    
    executeBlock(fn->body);
    
    // Check ensure contracts
    if (!errors.empty()) {
        // Don't check ensure if require failed
    } else {
        for (auto& contract : fn->contracts) {
            if (contract.kind == FnStmt::Contract::ENSURE) {
                Value cond = evaluate(contract.condition.get());
                if (!cond.isTruthy()) {
                    error("Contract failed: " + contract.message);
                }
            }
            // FIX contracts - auto-fix if condition fails
            else if (contract.kind == FnStmt::Contract::FIX) {
                Value cond = evaluate(contract.condition.get());
                if (!cond.isTruthy()) {
                    // Execute fix expression
                    if (contract.fixExpr) {
                        evaluate(contract.fixExpr.get());
                        std::cout << "[FIX] Applied automatic fix: " << contract.message << "\n";
                    }
                }
            }
        }
    }
    
    popScope();
    return result_;
}

Value VM::executeMatch(MatchExpr* match) {
    Value matchVal = evaluate(match->value.get());
    
    for (auto& caseExpr : match->cases) {
        for (auto& pattern : caseExpr.patterns) {
            Value patVal = evaluate(pattern.get());
            if (matchVal == patVal) {
                return evaluate(caseExpr.body.get());
            }
        }
    }
    
    return Value();
}

// SQL-like query execution
Value VM::executeQuery(QueryExpr* query) {
    // Evaluate FROM clause - get the collection
    Value collection = evaluate(query->from.get());
    
    if (!collection.isArray()) {
        error("Query can only be performed on arrays");
        return Value();
    }
    
    Value result;
    result.tag = Value::Tag::ARRAY;
    
    // Check if it's SELECT *
    bool selectAll = false;
    if (!query->select.empty()) {
        if (auto* ident = dynamic_cast<IdentifierExpr*>(query->select[0].get())) {
            if (ident->name == "*") {
                selectAll = true;
            }
        }
    }
    
    // Filter with WHERE clause
    for (const auto& item : collection.arrayVal) {
        // Store item in a temporary variable for WHERE evaluation
        std::string tempVar = "__item";
        scopes.back()[tempVar] = item;
        
        bool passes = true;
        if (query->where) {
            Value cond = evaluate(query->where.get());
            passes = cond.isTruthy();
        }
        
        if (passes) {
            if (selectAll) {
                result.arrayVal.push_back(item.clone());
            } else {
                // For selected fields, we just return the item for now
                result.arrayVal.push_back(item.clone());
            }
        }
    }
    
    // ORDER BY
    if (!query->orderBy.empty()) {
        // Simple sort by first order field
        // In real implementation would be more sophisticated
    }
    
    return result;
}

void VM::registerBuiltins() {
    // print
    globalEnv["print"] = Value([](const std::vector<Value>& args) {
        for (size_t i = 0; i < args.size(); ++i) {
            std::cout << args[i].toString();
            if (i < args.size() - 1) std::cout << " ";
        }
        std::cout << std::endl;
        return Value();
    });
    globalEnv["print"].tag = Value::Tag::NATIVE_FN;
    
    // println
    globalEnv["println"] = Value([](const std::vector<Value>& args) {
        for (size_t i = 0; i < args.size(); ++i) {
            std::cout << args[i].toString();
            if (i < args.size() - 1) std::cout << " ";
        }
        std::cout << "\n";
        return Value();
    });
    globalEnv["println"].tag = Value::Tag::NATIVE_FN;
    
    // len
    globalEnv["len"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) return Value(0);
        const Value& v = args[0];
        if (v.isArray()) return Value((long long)v.arrayVal.size());
        if (v.isString()) return Value((long long)v.strVal.size());
        return Value(0);
    });
    globalEnv["len"].tag = Value::Tag::NATIVE_FN;
    
    // range
    globalEnv["range"] = Value([](const std::vector<Value>& args) {
        Value result;
        result.tag = Value::Tag::ARRAY;
        long long start = 0, end = 0, step = 1;
        
        if (args.size() >= 1) start = 0, end = args[0].intVal;
        if (args.size() >= 2) start = args[0].intVal, end = args[1].intVal;
        if (args.size() >= 3) step = args[2].intVal;
        
        for (long long i = start; i < end; i += step) {
            result.arrayVal.push_back(Value(i));
        }
        return result;
    });
    globalEnv["range"].tag = Value::Tag::NATIVE_FN;
    
    // map
    globalEnv["map"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray()) return Value();
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        // args[1] should be a function
        for (const auto& item : args[0].arrayVal) {
            // Simplified: just add the item for now
            result.arrayVal.push_back(item);
        }
        return result;
    });
    globalEnv["map"].tag = Value::Tag::NATIVE_FN;
    
    // filter
    globalEnv["filter"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray()) return Value();
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        for (const auto& item : args[0].arrayVal) {
            result.arrayVal.push_back(item);
        }
        return result;
    });
    globalEnv["filter"].tag = Value::Tag::NATIVE_FN;
    
    // sum
    globalEnv["sum"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isArray()) return Value(0);
        
        long long total = 0;
        for (const auto& item : args[0].arrayVal) {
            if (item.isInt()) total += item.intVal;
        }
        return Value(total);
    });
    globalEnv["sum"].tag = Value::Tag::NATIVE_FN;
    
    // type
    globalEnv["type"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) return Value("null");
        return Value("unknown");
    });
    globalEnv["type"].tag = Value::Tag::NATIVE_FN;
}

std::string toDebugString(const Value& val) {
    return val.toString();
}

} // namespace smirnovlang
