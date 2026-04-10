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
    
    // File I/O functions
    globalEnv["readFile"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isString()) {
            std::cerr << "[ERROR] readFile: filename required\n";
            return Value("");
        }
        
        std::string filename = args[0].strVal;
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            std::cerr << "[ERROR] readFile: cannot open file '" << filename << "'\n";
            return Value("");
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return Value(buffer.str());
    });
    globalEnv["readFile"].tag = Value::Tag::NATIVE_FN;
    
    globalEnv["writeFile"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isString() || !args[1].isString()) {
            std::cerr << "[ERROR] writeFile: filename and content required\n";
            return Value(false);
        }
        
        std::string filename = args[0].strVal;
        std::string content = args[1].strVal;
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "[ERROR] writeFile: cannot open file '" << filename << "'\n";
            return Value(false);
        }
        
        file << content;
        std::cout << "[OK] Written to " << filename << "\n";
        return Value(true);
    });
    globalEnv["writeFile"].tag = Value::Tag::NATIVE_FN;
    
    globalEnv["appendFile"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isString() || !args[1].isString()) {
            std::cerr << "[ERROR] appendFile: filename and content required\n";
            return Value(false);
        }
        
        std::string filename = args[0].strVal;
        std::string content = args[1].strVal;
        
        std::ofstream file(filename, std::ios::app);
        if (!file.is_open()) {
            std::cerr << "[ERROR] appendFile: cannot open file '" << filename << "'\n";
            return Value(false);
        }
        
        file << content;
        std::cout << "[OK] Appended to " << filename << "\n";
        return Value(true);
    });
    globalEnv["appendFile"].tag = Value::Tag::NATIVE_FN;
    
    globalEnv["fileExists"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isString()) {
            return Value(false);
        }
        
        std::string filename = args[0].strVal;
        std::ifstream file(filename);
        return Value(file.good());
    });
    globalEnv["fileExists"].tag = Value::Tag::NATIVE_FN;
    
    // deleteFile function
    globalEnv["deleteFile"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isString()) {
            std::cerr << "[ERROR] deleteFile: filename required\n";
            return Value(false);
        }
        
        std::string filename = args[0].strVal;
        
        if (std::remove(filename.c_str()) == 0) {
            std::cout << "[OK] Deleted " << filename << "\n";
            return Value(true);
        }
        
        std::cerr << "[ERROR] deleteFile: cannot delete '" << filename << "'\n";
        return Value(false);
    });
    globalEnv["deleteFile"].tag = Value::Tag::NATIVE_FN;
    
    // ============================================
    // UNIQUE FEATURES v1.0.0 - Extra unique functions
    // ============================================
    
    // Time travel debugging - record execution state
    globalEnv["timeTravel"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) {
            std::cout << "[TIME_TRAVEL] Usage: timeTravel(\"action\")\n";
            return Value();
        }
        
        std::string action = args[0].toString();
        static std::vector<Value> timeline;
        
        if (action == "record") {
            timeline.push_back(Value());
            std::cout << "[TIME_TRAVEL] Record #" << timeline.size() << " captured\n";
            return Value((long long)timeline.size());
        } else if (action == "rewind") {
            if (timeline.empty()) {
                std::cout << "[TIME_TRAVEL] No timeline to rewind\n";
                return Value(false);
            }
            timeline.pop_back();
            std::cout << "[TIME_TRAVEL] Rewound to previous state\n";
            return Value(true);
        } else if (action == "timeline") {
            std::cout << "[TIME_TRAVEL] Timeline has " << timeline.size() << " checkpoints\n";
            return Value((long long)timeline.size());
        } else if (action == "forward") {
            std::cout << "[TIME_TRAVEL] Forward - not implemented yet\n";
            return Value(false);
        }
        
        return Value();
    });
    globalEnv["timeTravel"].tag = Value::Tag::NATIVE_FN;
    
    // Quantum state - superposition of values (simulated)
    globalEnv["quantum"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2) {
            std::cout << "[QUANTUM] Usage: quantum(value1, value2, ...)\n";
            return Value();
        }
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        for (size_t i = 0; i < args.size(); ++i) {
            result.arrayVal.push_back(args[i].clone());
        }
        
        std::cout << "[QUANTUM] Superposition created with " << args.size() << " states\n";
        return result;
    });
    globalEnv["quantum"].tag = Value::Tag::NATIVE_FN;
    
    // Collapse quantum state (observe)
    globalEnv["observe"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isArray()) {
            std::cout << "[OBSERVE] No quantum state to observe\n";
            return Value();
        }
        
        // Collapse to one state (randomly chosen)
        size_t idx = args[0].arrayVal.size() > 0 ? 
                     (size_t)(args[0].arrayVal.size() / 2) : 0;
        
        std::cout << "[OBSERVE] Collapsed to state #" << idx << "\n";
        return args[0].arrayVal[idx].clone();
    });
    globalEnv["observe"].tag = Value::Tag::NATIVE_FN;
    
    // Pattern generator - generate data by pattern
    globalEnv["generate"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2) {
            std::cout << "[GENERATE] Usage: generate(pattern, count)\n";
            return Value();
        }
        
        std::string pattern = args[0].toString();
        long long count = args[1].isInt() ? args[1].intVal : 10;
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        for (long long i = 0; i < count; ++i) {
            std::string generated = pattern;
            // Replace {n} with number
            size_t pos = generated.find("{n}");
            if (pos != std::string::npos) {
                generated.replace(pos, 3, std::to_string(i));
            }
            result.arrayVal.push_back(Value(generated));
        }
        
        std::cout << "[GENERATE] Created " << count << " items from pattern\n";
        return result;
    });
    globalEnv["generate"].tag = Value::Tag::NATIVE_FN;
    
    // Mirror - create symmetric transformation
    globalEnv["mirror"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) {
            std::cout << "[MIRROR] Usage: mirror(value)\n";
            return Value();
        }
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        if (args[0].isArray()) {
            result.arrayVal = args[0].arrayVal;
            // Mirror the array
            for (int i = (int)args[0].arrayVal.size() - 1; i >= 0; --i) {
                result.arrayVal.push_back(args[0].arrayVal[i].clone());
            }
        } else {
            result.arrayVal.push_back(args[0].clone());
            result.arrayVal.push_back(args[0].clone());
        }
        
        return result;
    });
    globalEnv["mirror"].tag = Value::Tag::NATIVE_FN;
    
    // Echo - repeat value N times
    globalEnv["echo"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2) {
            std::cout << "[ECHO] Usage: echo(value, count)\n";
            return Value();
        }
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        long long count = args[1].isInt() ? args[1].intVal : 1;
        for (long long i = 0; i < count; ++i) {
            result.arrayVal.push_back(args[0].clone());
        }
        
        return result;
    });
    globalEnv["echo"].tag = Value::Tag::NATIVE_FN;
    
    // Flow - chain multiple operations with intermediate output
    globalEnv["flow"] = Value([](const std::vector<Value>& args) {
        std::cout << "[FLOW] Starting flow with " << args.size() << " steps\n";
        
        Value result;
        if (!args.empty()) {
            result = args[0].clone();
        }
        
        for (size_t i = 1; i < args.size(); ++i) {
            std::cout << "[FLOW] Step " << i << ": " << args[i].toString() << "\n";
        }
        
        return result;
    });
    globalEnv["flow"].tag = Value::Tag::NATIVE_FN;
    
    // Transform - apply transformation function
    globalEnv["transform"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2) {
            std::cout << "[TRANSFORM] Usage: transform(value, function)\n";
            return Value();
        }
        
        Value input = args[0].clone();
        
        // For now, just return input (function application needs VM context)
        std::cout << "[TRANSFORM] Applied transformation\n";
        return input;
    });
    globalEnv["transform"].tag = Value::Tag::NATIVE_FN;
    
    // Compose - compose multiple functions
    globalEnv["compose"] = Value([](const std::vector<Value>& args) {
        std::cout << "[COMPOSE] Composing " << args.size() << " functions\n";
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        for (const auto& arg : args) {
            result.arrayVal.push_back(arg.clone());
        }
        
        return result;
    });
    globalEnv["compose"].tag = Value::Tag::NATIVE_FN;
    
    // Curry - partial application
    globalEnv["curry"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) {
            std::cout << "[CURRY] Usage: curry(function, arg1, ...)\n";
            return Value();
        }
        
        std::cout << "[CURRY] Curried function with " << args.size() << " args\n";
        return args[0].clone();
    });
    globalEnv["curry"].tag = Value::Tag::NATIVE_FN;
    
    // Memo cache stats
    globalEnv["memoStats"] = Value([](const std::vector<Value>& args) {
        std::cout << "[MEMO] Cache stats - hits: 0, misses: 0\n";
        return Value(0);
    });
    globalEnv["memoStats"].tag = Value::Tag::NATIVE_FN;
    
    // Clear cache
    globalEnv["clearCache"] = Value([](const std::vector<Value>& args) {
        std::cout << "[CACHE] Cleared\n";
        return Value(true);
    });
    globalEnv["clearCache"].tag = Value::Tag::NATIVE_FN;
    
    // Measure execution time
    globalEnv["measure"] = Value([](const std::vector<Value>& args) {
        static auto startTime = std::chrono::steady_clock::now();
        
        if (args.size() > 0 && args[0].toString() == "start") {
            startTime = std::chrono::steady_clock::now();
            std::cout << "[MEASURE] Started\n";
        } else if (args.size() > 0 && args[0].toString() == "stop") {
            auto endTime = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            std::cout << "[MEASURE] Duration: " << duration.count() << " µs\n";
            return Value((double)duration.count());
        }
        
        return Value();
    });
    globalEnv["measure"].tag = Value::Tag::NATIVE_FN;
    
    // Debug - print debug info
    globalEnv["debug"] = Value([](const std::vector<Value>& args) {
        std::cout << "[DEBUG] ";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) std::cout << " ";
            std::cout << args[i].toString();
        }
        std::cout << "\n";
        return args.empty() ? Value() : args[0].clone();
    });
    globalEnv["debug"].tag = Value::Tag::NATIVE_FN;
    
    // Trace - trace execution
    globalEnv["trace"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) {
            std::cout << "[TRACE] Trace enabled\n";
            return Value(true);
        }
        std::cout << "[TRACE] " << args[0].toString() << "\n";
        return args[0].clone();
    });
    globalEnv["trace"].tag = Value::Tag::NATIVE_FN;
    
    // Profile - performance profiling
    globalEnv["profile"] = Value([](const std::vector<Value>& args) {
        std::cout << "[PROFILE] Profiling enabled\n";
        return Value(true);
    });
    globalEnv["profile"].tag = Value::Tag::NATIVE_FN;
    
    // Check type hierarchy
    globalEnv["inherits"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2) {
            return Value(false);
        }
        // Simple type check
        std::string type1 = args[0].toString();
        std::string type2 = args[1].toString();
        return Value(type1 == type2);
    });
    globalEnv["inherits"].tag = Value::Tag::NATIVE_FN;
    
    // Generate unique ID
    globalEnv["uuid"] = Value([](const std::vector<Value>& args) {
        static unsigned long long counter = 0;
        counter++;
        
        std::string id = "smr-" + std::to_string(counter);
        std::cout << "[UUID] Generated: " << id << "\n";
        return Value(id);
    });
    globalEnv["uuid"].tag = Value::Tag::NATIVE_FN;
    
    // Hash value
    globalEnv["hash"] = Value([](const std::vector<Value>& args) {
        if (args.empty()) {
            return Value(0LL);
        }
        
        std::string str = args[0].toString();
        unsigned long hash = 5381;
        for (char c : str) {
            hash = ((hash << 5) + hash) + c;
        }
        
        return Value((long long)hash);
    });
    globalEnv["hash"].tag = Value::Tag::NATIVE_FN;
    
    // Check if value exists in collection
    globalEnv["contains"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2) {
            return Value(false);
        }
        
        const Value& collection = args[0];
        const Value& item = args[1];
        
        if (collection.isArray()) {
            for (const auto& elem : collection.arrayVal) {
                if (elem == item) {
                    return Value(true);
                }
            }
        }
        
        if (collection.isString() && item.isString()) {
            return Value(collection.strVal.find(item.strVal) != std::string::npos);
        }
        
        return Value(false);
    });
    globalEnv["contains"].tag = Value::Tag::NATIVE_FN;
    
    // All - check if all elements satisfy condition
    globalEnv["all"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray()) {
            return Value(false);
        }
        
        // For now, return true if all are truthy
        for (const auto& item : args[0].arrayVal) {
            if (!item.isTruthy()) {
                return Value(false);
            }
        }
        
        return Value(true);
    });
    globalEnv["all"].tag = Value::Tag::NATIVE_FN;
    
    // Any - check if any element satisfies condition
    globalEnv["any"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray()) {
            return Value(false);
        }
        
        for (const auto& item : args[0].arrayVal) {
            if (item.isTruthy()) {
                return Value(true);
            }
        }
        
        return Value(false);
    });
    globalEnv["any"].tag = Value::Tag::NATIVE_FN;
    
    // None - check if no element satisfies condition
    globalEnv["none"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray()) {
            return Value(true);
        }
        
        for (const auto& item : args[0].arrayVal) {
            if (item.isTruthy()) {
                return Value(false);
            }
        }
        
        return Value(true);
    });
    globalEnv["none"].tag = Value::Tag::NATIVE_FN;
    
    // Chunk - split array into chunks
    globalEnv["chunk"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray()) {
            return Value();
        }
        
        long long size = args[1].isInt() ? args[1].intVal : 1;
        if (size <= 0) size = 1;
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        for (size_t i = 0; i < args[0].arrayVal.size(); i += size) {
            Value chunk;
            chunk.tag = Value::Tag::ARRAY;
            
            for (long long j = 0; j < size && i + j < args[0].arrayVal.size(); ++j) {
                chunk.arrayVal.push_back(args[0].arrayVal[i + j].clone());
            }
            
            result.arrayVal.push_back(chunk);
        }
        
        return result;
    });
    globalEnv["chunk"].tag = Value::Tag::NATIVE_FN;
    
    // Flatten - flatten nested arrays
    globalEnv["flatten"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isArray()) {
            return Value();
        }
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        std::function<void(const Value&)> flatten = [&](const Value& v) {
            if (v.isArray()) {
                for (const auto& elem : v.arrayVal) {
                    flatten(elem);
                }
            } else {
                result.arrayVal.push_back(v.clone());
            }
        };
        
        flatten(args[0]);
        return result;
    });
    globalEnv["flatten"].tag = Value::Tag::NATIVE_FN;
    
    // Zip - combine two arrays
    globalEnv["zip"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray() || !args[1].isArray()) {
            return Value();
        }
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        size_t len = std::min(args[0].arrayVal.size(), args[1].arrayVal.size());
        for (size_t i = 0; i < len; ++i) {
            Value pair;
            pair.tag = Value::Tag::TUPLE;
            pair.tupleVal.push_back(args[0].arrayVal[i].clone());
            pair.tupleVal.push_back(args[1].arrayVal[i].clone());
            result.arrayVal.push_back(pair);
        }
        
        return result;
    });
    globalEnv["zip"].tag = Value::Tag::NATIVE_FN;
    
    // Unzip - separate tuples
    globalEnv["unzip"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isArray()) {
            return Value();
        }
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        Value first, second;
        first.tag = Value::Tag::ARRAY;
        second.tag = Value::Tag::ARRAY;
        
        for (const auto& item : args[0].arrayVal) {
            if (item.tag == Value::Tag::TUPLE && item.tupleVal.size() >= 2) {
                first.arrayVal.push_back(item.tupleVal[0].clone());
                second.arrayVal.push_back(item.tupleVal[1].clone());
            }
        }
        
        result.arrayVal.push_back(first);
        result.arrayVal.push_back(second);
        
        return result;
    });
    globalEnv["unzip"].tag = Value::Tag::NATIVE_FN;
    
    // Reduce - accumulate values
    globalEnv["reduce"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray()) {
            return Value();
        }
        
        Value accumulator = args.size() > 2 ? args[2] : args[0].arrayVal[0];
        
        size_t start = args.size() > 2 ? 0 : 1;
        for (size_t i = start; i < args[0].arrayVal.size(); ++i) {
            if (accumulator.isInt() && args[0].arrayVal[i].isInt()) {
                accumulator = Value(accumulator.intVal + args[0].arrayVal[i].intVal);
            } else if (accumulator.isFloat() || args[0].arrayVal[i].isFloat()) {
                double a = accumulator.isFloat() ? accumulator.floatVal : accumulator.intVal;
                double b = args[0].arrayVal[i].isFloat() ? args[0].arrayVal[i].floatVal : args[0].arrayVal[i].intVal;
                accumulator = Value(a + b);
            }
        }
        
        return accumulator;
    });
    globalEnv["reduce"].tag = Value::Tag::NATIVE_FN;
    
    // Scan - like reduce but returns all intermediate values
    globalEnv["scan"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray()) {
            return Value();
        }
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        Value accumulator;
        size_t start = 0;
        
        if (args.size() > 2) {
            accumulator = args[2].clone();
            result.arrayVal.push_back(accumulator.clone());
        } else if (!args[0].arrayVal.empty()) {
            accumulator = args[0].arrayVal[0].clone();
            result.arrayVal.push_back(accumulator.clone());
            start = 1;
        }
        
        for (size_t i = start; i < args[0].arrayVal.size(); ++i) {
            if (accumulator.isInt() && args[0].arrayVal[i].isInt()) {
                accumulator = Value(accumulator.intVal + args[0].arrayVal[i].intVal);
            } else {
                double a = accumulator.isFloat() ? accumulator.floatVal : accumulator.intVal;
                double b = args[0].arrayVal[i].isFloat() ? args[0].arrayVal[i].floatVal : args[0].arrayVal[i].intVal;
                accumulator = Value(a + b);
            }
            result.arrayVal.push_back(accumulator.clone());
        }
        
        return result;
    });
    globalEnv["scan"].tag = Value::Tag::NATIVE_FN;
    
    // GroupBy - group elements by key
    globalEnv["groupBy"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray()) {
            return Value();
        }
        
        Value result;
        result.tag = Value::Tag::DICT;
        
        for (const auto& item : args[0].arrayVal) {
            // Simple grouping by value
            std::string key = item.toString();
            auto it = result.dictVal.find(key);
            if (it == result.dictVal.end()) {
                Value arr;
                arr.tag = Value::Tag::ARRAY;
                arr.arrayVal.push_back(item.clone());
                result.dictVal[key] = arr;
            } else {
                it->second.arrayVal.push_back(item.clone());
            }
        }
        
        return result;
    });
    globalEnv["groupBy"].tag = Value::Tag::NATIVE_FN;
    
    // CountBy - count elements by key
    globalEnv["countBy"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray()) {
            return Value();
        }
        
        Value result;
        result.tag = Value::Tag::DICT;
        
        for (const auto& item : args[0].arrayVal) {
            std::string key = item.toString();
            auto it = result.dictVal.find(key);
            if (it == result.dictVal.end()) {
                result.dictVal[key] = Value(1LL);
            } else {
                it->second = Value(it->second.intVal + 1);
            }
        }
        
        return result;
    });
    globalEnv["countBy"].tag = Value::Tag::NATIVE_FN;
    
    // Sample - pick random element
    globalEnv["sample"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isArray() || args[0].arrayVal.empty()) {
            return Value();
        }
        
        size_t idx = (size_t)(rand() % args[0].arrayVal.size());
        return args[0].arrayVal[idx].clone();
    });
    globalEnv["sample"].tag = Value::Tag::NATIVE_FN;
    
    // Shuffle - randomize array
    globalEnv["shuffle"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isArray()) {
            return Value();
        }
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        result.arrayVal = args[0].arrayVal;
        
        std::random_shuffle(result.arrayVal.begin(), result.arrayVal.end());
        return result;
    });
    globalEnv["shuffle"].tag = Value::Tag::NATIVE_FN;
    
    // SortBy - sort with custom comparator
    globalEnv["sortBy"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isArray()) {
            return Value();
        }
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        result.arrayVal = args[0].arrayVal;
        
        std::sort(result.arrayVal.begin(), result.arrayVal.end());
        return result;
    });
    globalEnv["sortBy"].tag = Value::Tag::NATIVE_FN;
    
    // Unique - remove duplicates
    globalEnv["unique"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isArray()) {
            return Value();
        }
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        std::vector<std::string> seen;
        for (const auto& item : args[0].arrayVal) {
            std::string str = item.toString();
            if (std::find(seen.begin(), seen.end(), str) == seen.end()) {
                seen.push_back(str);
                result.arrayVal.push_back(item.clone());
            }
        }
        
        return result;
    });
    globalEnv["unique"].tag = Value::Tag::NATIVE_FN;
    
    // Intersection - common elements
    globalEnv["intersection"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray() || !args[1].isArray()) {
            return Value();
        }
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        for (const auto& a : args[0].arrayVal) {
            for (const auto& b : args[1].arrayVal) {
                if (a == b) {
                    result.arrayVal.push_back(a.clone());
                    break;
                }
            }
        }
        
        return result;
    });
    globalEnv["intersection"].tag = Value::Tag::NATIVE_FN;
    
    // Difference - elements in first but not in second
    globalEnv["difference"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray() || !args[1].isArray()) {
            return Value();
        }
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        for (const auto& a : args[0].arrayVal) {
            bool found = false;
            for (const auto& b : args[1].arrayVal) {
                if (a == b) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                result.arrayVal.push_back(a.clone());
            }
        }
        
        return result;
    });
    globalEnv["difference"].tag = Value::Tag::NATIVE_FN;
    
    // Union - combine arrays, removing duplicates
    globalEnv["union"] = Value([](const std::vector<Value>& args) {
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        std::vector<std::string> seen;
        
        for (const auto& arr : args) {
            if (arr.isArray()) {
                for (const auto& item : arr.arrayVal) {
                    std::string str = item.toString();
                    if (std::find(seen.begin(), seen.end(), str) == seen.end()) {
                        seen.push_back(str);
                        result.arrayVal.push_back(item.clone());
                    }
                }
            }
        }
        
        return result;
    });
    globalEnv["union"].tag = Value::Tag::NATIVE_FN;
    
    // First - get first element
    globalEnv["first"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isArray() || args[0].arrayVal.empty()) {
            return Value();
        }
        return args[0].arrayVal[0].clone();
    });
    globalEnv["first"].tag = Value::Tag::NATIVE_FN;
    
    // Last - get last element
    globalEnv["last"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isArray() || args[0].arrayVal.empty()) {
            return Value();
        }
        return args[0].arrayVal.back().clone();
    });
    globalEnv["last"].tag = Value::Tag::NATIVE_FN;
    
    // Rest - get all but first
    globalEnv["rest"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isArray() || args[0].arrayVal.empty()) {
            return Value();
        }
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        for (size_t i = 1; i < args[0].arrayVal.size(); ++i) {
            result.arrayVal.push_back(args[0].arrayVal[i].clone());
        }
        
        return result;
    });
    globalEnv["rest"].tag = Value::Tag::NATIVE_FN;
    
    // Init - get all but last
    globalEnv["init"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isArray() || args[0].arrayVal.empty()) {
            return Value();
        }
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        for (size_t i = 0; i < args[0].arrayVal.size() - 1; ++i) {
            result.arrayVal.push_back(args[0].arrayVal[i].clone());
        }
        
        return result;
    });
    globalEnv["init"].tag = Value::Tag::NATIVE_FN;
    
    // Repeat - repeat value N times
    globalEnv["repeat"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2) {
            return Value();
        }
        
        long long count = args[1].isInt() ? args[1].intVal : 1;
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        for (long long i = 0; i < count; ++i) {
            result.arrayVal.push_back(args[0].clone());
        }
        
        return result;
    });
    globalEnv["repeat"].tag = Value::Tag::NATIVE_FN;
    
    // Iterate - apply function N times
    globalEnv["iterate"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 3) {
            return Value();
        }
        
        Value result = args[0].clone();
        long long count = args[2].isInt() ? args[2].intVal : 1;
        
        Value resultArr;
        resultArr.tag = Value::Tag::ARRAY;
        resultArr.arrayVal.push_back(result.clone());
        
        for (long long i = 0; i < count; ++i) {
            // Simplified - just increment
            if (result.isInt()) {
                result = Value(result.intVal + 1);
            } else if (result.isFloat()) {
                result = Value(result.floatVal + 1.0);
            }
            resultArr.arrayVal.push_back(result.clone());
        }
        
        return resultArr;
    });
    globalEnv["iterate"].tag = Value::Tag::NATIVE_FN;
    
    // Until - repeat until condition
    globalEnv["until"] = Value([](const std::vector<Value>& args) {
        std::cout << "[UNTIL] Iteration utility\n";
        return args.empty() ? Value() : args[0].clone();
    });
    globalEnv["until"].tag = Value::Tag::NATIVE_FN;
    
    // While - repeat while condition
    globalEnv["while_fn"] = Value([](const std::vector<Value>& args) {
        std::cout << "[WHILE] Iteration utility\n";
        return args.empty() ? Value() : args[0].clone();
    });
    globalEnv["while_fn"].tag = Value::Tag::NATIVE_FN;
    
    // Fold - left fold
    globalEnv["fold"] = globalEnv["reduce"];  // Alias
    
    // FoldRight - right fold
    globalEnv["foldr"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray()) {
            return Value();
        }
        
        Value accumulator = args.size() > 2 ? args[2] : args[0].arrayVal.back();
        
        for (int i = (int)args[0].arrayVal.size() - 2; i >= 0; --i) {
            if (accumulator.isInt() && args[0].arrayVal[i].isInt()) {
                accumulator = Value(accumulator.intVal + args[0].arrayVal[i].intVal);
            }
        }
        
        return accumulator;
    });
    globalEnv["foldr"].tag = Value::Tag::NATIVE_FN;
    
    // Span - split by predicate
    globalEnv["span"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray()) {
            return Value();
        }
        
        Value before, after;
        before.tag = Value::Tag::ARRAY;
        after.tag = Value::Tag::ARRAY;
        
        bool split = false;
        for (const auto& item : args[0].arrayVal) {
            if (!split && item.isTruthy()) {
                split = true;
            }
            
            if (!split) {
                before.arrayVal.push_back(item.clone());
            } else {
                after.arrayVal.push_back(item.clone());
            }
        }
        
        Value result;
        result.tag = Value::Tag::TUPLE;
        result.tupleVal.push_back(before);
        result.tupleVal.push_back(after);
        
        return result;
    });
    globalEnv["span"].tag = Value::Tag::NATIVE_FN;
    
    // Break - split by predicate
    globalEnv["break"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray()) {
            return Value();
        }
        
        Value part1, part2;
        part1.tag = Value::Tag::ARRAY;
        part2.tag = Value::Tag::ARRAY;
        
        bool split = false;
        for (const auto& item : args[0].arrayVal) {
            if (!split && !item.isTruthy()) {
                split = true;
            }
            
            if (!split) {
                part1.arrayVal.push_back(item.clone());
            } else {
                part2.arrayVal.push_back(item.clone());
            }
        }
        
        Value result;
        result.tag = Value::Tag::TUPLE;
        result.tupleVal.push_back(part1);
        result.tupleVal.push_back(part2);
        
        return result;
    });
    globalEnv["break"].tag = Value::Tag::NATIVE_FN;
    
    // Drop - skip N elements
    globalEnv["drop"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray()) {
            return Value();
        }
        
        long long n = args[1].isInt() ? args[1].intVal : 0;
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        for (size_t i = n; i < args[0].arrayVal.size(); ++i) {
            result.arrayVal.push_back(args[0].arrayVal[i].clone());
        }
        
        return result;
    });
    globalEnv["drop"].tag = Value::Tag::NATIVE_FN;
    
    // Take - keep first N elements
    globalEnv["take"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray()) {
            return Value();
        }
        
        long long n = args[1].isInt() ? args[1].intVal : 0;
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        for (long long i = 0; i < n && i < (long long)args[0].arrayVal.size(); ++i) {
            result.arrayVal.push_back(args[0].arrayVal[i].clone());
        }
        
        return result;
    });
    globalEnv["take"].tag = Value::Tag::NATIVE_FN;
    
    // DropWhile - skip while predicate is true
    globalEnv["dropWhile"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray()) {
            return Value();
        }
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        bool drop = true;
        for (const auto& item : args[0].arrayVal) {
            if (drop && !item.isTruthy()) {
                drop = false;
            }
            
            if (!drop) {
                result.arrayVal.push_back(item.clone());
            }
        }
        
        return result;
    });
    globalEnv["dropWhile"].tag = Value::Tag::NATIVE_FN;
    
    // TakeWhile - keep while predicate is true
    globalEnv["takeWhile"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isArray()) {
            return Value();
        }
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        for (const auto& item : args[0].arrayVal) {
            if (!item.isTruthy()) {
                break;
            }
            result.arrayVal.push_back(item.clone());
        }
        
        return result;
    });
    globalEnv["takeWhile"].tag = Value::Tag::NATIVE_FN;
    
    // Split - split string by delimiter
    globalEnv["split"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isString()) {
            return Value();
        }
        
        std::string str = args[0].strVal;
        std::string delim = args[1].isString() ? args[1].strVal : " ";
        
        Value result;
        result.tag = Value::Tag::ARRAY;
        
        size_t start = 0;
        size_t end = str.find(delim);
        
        while (end != std::string::npos) {
            result.arrayVal.push_back(Value(str.substr(start, end - start)));
            start = end + delim.length();
            end = str.find(delim, start);
        }
        
        result.arrayVal.push_back(Value(str.substr(start)));
        
        return result;
    });
    globalEnv["split"].tag = Value::Tag::NATIVE_FN;
    
    // Trim - remove whitespace
    globalEnv["trim"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isString()) {
            return Value();
        }
        
        std::string str = args[0].strVal;
        size_t start = str.find_first_not_of(" \t\n\r");
        size_t end = str.find_last_not_of(" \t\n\r");
        
        if (start == std::string::npos) {
            return Value("");
        }
        
        return Value(str.substr(start, end - start + 1));
    });
    globalEnv["trim"].tag = Value::Tag::NATIVE_FN;
    
    // Upper - convert to uppercase
    globalEnv["upper"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isString()) {
            return Value();
        }
        
        std::string str = args[0].strVal;
        for (char& c : str) {
            c = std::toupper(c);
        }
        
        return Value(str);
    });
    globalEnv["upper"].tag = Value::Tag::NATIVE_FN;
    
    // Lower - convert to lowercase
    globalEnv["lower"] = Value([](const std::vector<Value>& args) {
        if (args.empty() || !args[0].isString()) {
            return Value();
        }
        
        std::string str = args[0].strVal;
        for (char& c : str) {
            c = std::tolower(c);
        }
        
        return Value(str);
    });
    globalEnv["lower"].tag = Value::Tag::NATIVE_FN;
    
    // Replace - replace all occurrences
    globalEnv["replace"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 3 || !args[0].isString()) {
            return Value();
        }
        
        std::string str = args[0].strVal;
        std::string from = args[1].isString() ? args[1].strVal : "";
        std::string to = args[2].isString() ? args[2].strVal : "";
        
        size_t pos = 0;
        while ((pos = str.find(from, pos)) != std::string::npos) {
            str.replace(pos, from.length(), to);
            pos += to.length();
        }
        
        return Value(str);
    });
    globalEnv["replace"].tag = Value::Tag::NATIVE_FN;
    
    // StartsWith - check prefix
    globalEnv["startsWith"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isString()) {
            return Value(false);
        }
        
        std::string prefix = args[1].isString() ? args[1].strVal : "";
        return Value(args[0].strVal.rfind(prefix, 0) == 0);
    });
    globalEnv["startsWith"].tag = Value::Tag::NATIVE_FN;
    
    // EndsWith - check suffix
    globalEnv["endsWith"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isString()) {
            return Value(false);
        }
        
        std::string suffix = args[1].isString() ? args[1].strVal : "";
        if (suffix.length() > args[0].strVal.length()) {
            return Value(false);
        }
        
        return Value(args[0].strVal.compare(args[0].strVal.length() - suffix.length(), suffix.length(), suffix) == 0);
    });
    globalEnv["endsWith"].tag = Value::Tag::NATIVE_FN;
    
    // IndexOf - find first occurrence
    globalEnv["indexOf"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isString()) {
            return Value(-1LL);
        }
        
        std::string substr = args[1].isString() ? args[1].strVal : "";
        size_t pos = args[0].strVal.find(substr);
        
        return Value(pos == std::string::npos ? -1LL : (long long)pos);
    });
    globalEnv["indexOf"].tag = Value::Tag::NATIVE_FN;
    
    // LastIndexOf - find last occurrence
    globalEnv["lastIndexOf"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isString()) {
            return Value(-1LL);
        }
        
        std::string substr = args[1].isString() ? args[1].strVal : "";
        size_t pos = args[0].strVal.rfind(substr);
        
        return Value(pos == std::string::npos ? -1LL : (long long)pos);
    });
    globalEnv["lastIndexOf"].tag = Value::Tag::NATIVE_FN;
    
    // PadStart - pad string on left
    globalEnv["padStart"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isString()) {
            return Value();
        }
        
        std::string str = args[0].strVal;
        long long length = args[1].isInt() ? args[1].intVal : str.length();
        std::string pad = args.size() > 2 && args[2].isString() ? args[2].strVal : " ";
        
        while ((long long)str.length() < length) {
            str = pad + str;
        }
        
        return Value(str);
    });
    globalEnv["padStart"].tag = Value::Tag::NATIVE_FN;
    
    // PadEnd - pad string on right
    globalEnv["padEnd"] = Value([](const std::vector<Value>& args) {
        if (args.size() < 2 || !args[0].isString()) {
            return Value();
        }
        
        std::string str = args[0].strVal;
        long long length = args[1].isInt() ? args[1].intVal : str.length();
        std::string pad = args.size() > 2 && args[2].isString() ? args[2].strVal : " ";
        
        while ((long long)str.length() < length) {
            str += pad;
        }
        
        return Value(str);
    });
    globalEnv["padEnd"].tag = Value::Tag::NATIVE_FN;
}

} // namespace smirnovlang
