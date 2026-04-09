#include "vm/vm.h"
#include <cmath>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <functional>
#include <map>

namespace fs = std::filesystem;

// Extend VM with more built-in functions
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
    
    // sqrt
    globalEnv["sqrt"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) return Value(0.0);
        double val = args[0].isInt() ? args[0].intVal : args[0].floatVal;
        if (val < 0) return Value(0.0);
        return Value(std::sqrt(val));
    });
    globalEnv["sqrt"].tag = Value::Tag::NATIVE_FN;
    
    // pow - a^b
    globalEnv["pow"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2) return Value(1.0);
        double a = args[0].isInt() ? args[0].intVal : args[0].floatVal;
        double b = args[1].isInt() ? args[1].intVal : args[1].floatVal;
        return Value(std::pow(a, b));
    });
    globalEnv["pow"].tag = Value::Tag::NATIVE_FN;
    
    // sin, cos, tan
    globalEnv["sin"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) return Value(0.0);
        return Value(std::sin(args[0].isInt() ? args[0].intVal : args[0].floatVal));
    });
    globalEnv["sin"].tag = Value::Tag::NATIVE_FN;
    
    globalEnv["cos"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) return Value(0.0);
        return Value(std::cos(args[0].isInt() ? args[0].intVal : args[0].floatVal));
    });
    globalEnv["cos"].tag = Value::Tag::NATIVE_FN;
    
    globalEnv["tan"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) return Value(0.0);
        return Value(std::tan(args[0].isInt() ? args[0].intVal : args[0].floatVal));
    });
    globalEnv["tan"].tag = Value::Tag::NATIVE_FN;
    
    // abs
    globalEnv["abs"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) return Value(0);
        if (args[0].isInt()) return Value(std::abs(args[0].intVal));
        return Value(std::abs(args[0].floatVal));
    });
    globalEnv["abs"].tag = Value::Tag::NATIVE_FN;
    
    // min, max
    globalEnv["min"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) return Value();
        auto minVal = args[0];
        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] < minVal) minVal = args[i];
        }
        return minVal;
    });
    globalEnv["min"].tag = Value::Tag::NATIVE_FN;
    
    globalEnv["max"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) return Value();
        auto maxVal = args[0];
        for (size_t i = 1; i < args.size(); ++i) {
            if (maxVal < args[i]) maxVal = args[i];
        }
        return maxVal;
    });
    globalEnv["max"].tag = Value::Tag::NATIVE_FN;
    
    // map - transform each element
    globalEnv["map"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray()) return Value();
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        // args[1] should be a function/closure
        if (!args[1].isCallable()) {
            // If not callable, just return identity
            return args[0];
        }
        
        // For each item, call the function
        for (const auto& item : args[0].arrayVal) {
            std::vector<Value> fnArgs;
            fnArgs.push_back(item.clone());
            Value transformed = args[1].nativeFn(fnArgs);
            result.arrayVal.push_back(transformed);
        }
        return result;
    });
    globalEnv["map"].tag = Value::Tag::NATIVE_FN;
    
    // filter - keep elements matching condition
    globalEnv["filter"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray()) return Value();
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        // args[1] should be a function/closure
        if (!args[1].isCallable()) {
            // If not callable, return original array
            return args[0];
        }
        
        // For each item, call the function and check if true
        for (const auto& item : args[0].arrayVal) {
            std::vector<Value> fnArgs;
            fnArgs.push_back(item.clone());
            Value condition = args[1].nativeFn(fnArgs);
            if (condition.isTruthy()) {
                result.arrayVal.push_back(item.clone());
            }
        }
        return result;
    });
    globalEnv["filter"].tag = Value::Tag::NATIVE_FN;
    
    // sum
    globalEnv["sum"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isArray()) return Value(0);
        
        double total = 0;
        for (const auto& item : args[0].arrayVal) {
            if (item.isInt()) total += item.intVal;
            else if (item.isFloat()) total += item.floatVal;
        }
        if (total == (long long)total) return Value((long long)total);
        return Value(total);
    });
    globalEnv["sum"].tag = Value::Tag::NATIVE_FN;
    
    // type
    globalEnv["type"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) return Value("null");
        const Value& v = args[0];
        switch (v.tag) {
            case Value::Tag::NIL: return Value("null");
            case Value::Tag::INT: return Value("int");
            case Value::Tag::FLOAT: return Value("float");
            case Value::Tag::BOOL: return Value("bool");
            case Value::Tag::STRING: return Value("string");
            case Value::Tag::ARRAY: return Value("array");
            case Value::Tag::TUPLE: return Value("tuple");
            case Value::Tag::DICT: return Value("dict");
            case Value::Tag::FUNCTION: return Value("function");
            case Value::Tag::CLOSURE: return Value("closure");
            case Value::Tag::OBJECT: return Value("object");
            default: return Value("unknown");
        }
    });
    globalEnv["type"].tag = Value::Tag::NATIVE_FN;
    
    // isEmpty
    globalEnv["isEmpty"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) return Value(true);
        const Value& v = args[0];
        if (v.isArray()) return Value(v.arrayVal.empty());
        if (v.isString()) return Value(v.strVal.empty());
        return Value(false);
    });
    globalEnv["isEmpty"].tag = Value::Tag::NATIVE_FN;
    
    // push - add element to array
    globalEnv["push"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray()) return Value();
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        result.arrayVal = args[0].arrayVal;
        result.arrayVal.push_back(args[1].clone());
        return result;
    });
    globalEnv["push"].tag = Value::Tag::NATIVE_FN;
    
    // reverse - reverse array
    globalEnv["reverse"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isArray()) return Value();
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        result.arrayVal = args[0].arrayVal;
        std::reverse(result.arrayVal.begin(), result.arrayVal.end());
        return result;
    });
    globalEnv["reverse"].tag = Value::Tag::NATIVE_FN;
    
    // slice - get subarray
    globalEnv["slice"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isArray()) return Value();
        
        long long start = 0;
        long long end = args[0].arrayVal.size();
        
        if (args.size() >= 2 && args[1].isInt()) start = args[1].intVal;
        if (args.size() >= 3 && args[2].isInt()) end = args[2].intVal;
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        for (long long i = start; i < end && i < (long long)args[0].arrayVal.size(); ++i) {
            if (i >= 0) result.arrayVal.push_back(args[0].arrayVal[i].clone());
        }
        return result;
    });
    globalEnv["slice"].tag = Value::Tag::NATIVE_FN;
    
    // join - join array of strings with separator
    globalEnv["join"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) return Value("");
        std::string separator = args.size() > 1 ? args[1].strVal : "";
        
        Value result;
        if (args[0].isArray()) {
            for (size_t i = 0; i < args[0].arrayVal.size(); ++i) {
                result.strVal += args[0].arrayVal[i].toString();
                if (i < args[0].arrayVal.size() - 1) result.strVal += separator;
            }
        }
        return result;
    });
    globalEnv["join"].tag = Value::Tag::NATIVE_FN;
    
    // concat - concatenate arrays
    globalEnv["concat"] = Value([](const std::vector<Value>& args) {
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        for (const auto& arr : args) {
            if (arr.isArray()) {
                result.arrayVal.insert(result.arrayVal.end(), 
                                      arr.arrayVal.begin(), arr.arrayVal.end());
            }
        }
        return result;
    });
    globalEnv["concat"].tag = Value::Tag::NATIVE_FN;
    
    // toInt - convert to integer
    globalEnv["toInt"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) return Value(0);
        if (args[0].isInt()) return args[0].intVal;
        if (args[0].isFloat()) return Value((long long)args[0].floatVal);
        if (args[0].isString()) {
            try {
                return Value(std::stoll(args[0].strVal));
            } catch (...) {
                return Value(0);
            }
        }
        return Value(0);
    });
    globalEnv["toInt"].tag = Value::Tag::NATIVE_FN;
    
    // toString - convert to string
    globalEnv["toString"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) return Value("");
        return Value(args[0].toString());
    });
    globalEnv["toString"].tag = Value::Tag::NATIVE_FN;
    
    // toFloat - convert to float
    globalEnv["toFloat"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) return Value(0.0);
        if (args[0].isFloat()) return args[0].floatVal;
        if (args[0].isInt()) return Value((double)args[0].intVal);
        if (args[0].isString()) {
            try {
                return Value(std::stod(args[0].strVal));
            } catch (...) {
                return Value(0.0);
            }
        }
        return Value(0.0);
    });
    globalEnv["toFloat"].tag = Value::Tag::NATIVE_FN;
    
    // UNIQUE FEATURE: assert - assertion for testing
    globalEnv["assert"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) {
            std::cout << "[ASSERT] No arguments provided\n";
            return Value(false);
        }
        
        bool condition = args[0].isTruthy();
        std::string message = args.size() > 1 ? args[1].strVal : "Assertion failed";
        
        if (!condition) {
            std::cout << "[ASSERT FAILED] " << message << "\n";
        } else {
            std::cout << "[ASSERT PASSED] " << message << "\n";
        }
        
        return Value(condition);
    });
    globalEnv["assert"].tag = Value::Tag::NATIVE_FN;
    
    // UNIQUE FEATURE: Data Versioning - commit
    // version.commit(data, "name") - save version with name
    globalEnv["commit"] = Value([](const std::vector<Value>& args) {
        // We'll use a static map to store versions in this implementation
        static std::map<std::string, std::vector<Value>> versionStore;
        
        if (args.size() < 2) {
            std::cout << "[VERSION] Usage: commit(data, \"name\")\n";
            return Value(false);
        }
        
        std::string name = args[1].strVal;
        
        // Store this version
        Value data = args[0].clone();
        versionStore[name].push_back(data);
        
        std::cout << "[VERSION] Committed: " << name 
                  << " (total versions: " << versionStore[name].size() << ")\n";
        
        return Value(true);
    });
    globalEnv["commit"].tag = Value::Tag::NATIVE_FN;
    
    // UNIQUE FEATURE: Data Versioning - rollback
    // version.rollback("name") - get last version
    globalEnv["rollback"] = Value([](const std::vector<Value>& args) {
        static std::map<std::string, std::vector<Value>> versionStore;
        
        if (args.empty()) {
            std::cout << "[VERSION] Usage: rollback(\"name\")\n";
            return Value();
        }
        
        std::string name = args[0].strVal;
        
        if (versionStore[name].empty()) {
            std::cout << "[VERSION] No versions found for: " << name << "\n";
            return Value();
        }
        
        // Return the last version
        Value result = versionStore[name].back().clone();
        std::cout << "[VERSION] Rolled back to: " << name << "\n";
        
        return result;
    });
    globalEnv["rollback"].tag = Value::Tag::NATIVE_FN;
    
    // UNIQUE FEATURE: Data Versioning - history
    // version.history("name") - get all versions
    globalEnv["history"] = Value([](const std::vector<Value>& args) {
        static std::map<std::string, std::vector<Value>> versionStore;
        
        if (args.empty()) {
            std::cout << "[VERSION] Usage: history(\"name\")\n";
            return Value();
        }
        
        std::string name = args[0].strVal;
        
        if (versionStore[name].empty()) {
            std::cout << "[VERSION] No history for: " << name << "\n";
            return Value();
        }
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        std::cout << "[VERSION] History for " << name << ":\n";
        for (size_t i = 0; i < versionStore[name].size(); ++i) {
            std::cout << "  v" << i << ": " << versionStore[name][i].toString() << "\n";
            result.arrayVal.push_back(versionStore[name][i].clone());
        }
        
        return result;
    });
    globalEnv["history"].tag = Value::Tag::NATIVE_FN;
    
    // UNIQUE FEATURE: Data Versioning - diff
    // version.diff(v1, v2) - compare two versions
    globalEnv["diff"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2) {
            std::cout << "[VERSION] Usage: diff(version1, version2)\n";
            return Value();
        }
        
        std::string v1 = args[0].toString();
        std::string v2 = args[1].toString();
        
        if (v1 == v2) {
            std::cout << "[VERSION] No differences (identical)\n";
            return Value(true); // true = no diff
        }
        
        std::cout << "[VERSION] DIFF:\n";
        std::cout << "  - " << v1 << "\n";
        std::cout << "  + " << v2 << "\n";
        
        return Value(false); // false = has diff
    });
    globalEnv["diff"].tag = Value::Tag::NATIVE_FN;
    
    // UNIQUE FEATURE: Auto-doc - generate documentation
    // doc(function) - generate docs for a function
    globalEnv["doc"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) {
            std::cout << "[DOC] Usage: doc(functionName)\n";
            return Value();
        }
        
        // Try to get function info
        std::string name = args[0].toString();
        
        std::cout << "[DOC] Generating documentation for: " << name << "\n";
        std::cout << "========================================\n";
        std::cout << "Function: " << name << "\n";
        
        // For now, just return a string representation
        // In full implementation, this would use DocComment from AST
        Value result;
        result.tag = Value::Tag::STRING;
        result.strVal = "Documentation for " + name + " - auto-generated";
        
        return result;
    });
    globalEnv["doc"].tag = Value::Tag::NATIVE_FN;
}

} // namespace smirnovlang
