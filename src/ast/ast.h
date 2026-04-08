#pragma once

#include "lexer/token.h"
#include <memory>
#include <vector>
#include <string>
#include <variant>

namespace smirnovlang {

// Forward declarations
struct Expr;
struct Stmt;
using StmtPtr = std::unique_ptr<Stmt>;

// Type representation
struct Type {
    enum Kind {
        INT, FLOAT, BOOL, STRING, CHAR,
        ARRAY, TUPLE, DICT, FUNC, CUSTOM,
        OPTIONAL, REFERENCE, VOID,
        AUTO  // for type inference
    } kind;
    std::string name;  // for custom types
    std::shared_ptr<Type> inner;  // for optional, reference, array
    std::vector<std::shared_ptr<Type>> params;  // for generics
    
    static Type Int() { return {INT}; }
    static Type Float() { return {FLOAT}; }
    static Type Bool() { return {BOOL}; }
    static Type String() { return {STRING}; }
    static Type Char() { return {CHAR}; }
    static Type Void() { return {VOID}; }
    static Type Auto() { return {AUTO}; }
    static Type Optional(std::shared_ptr<Type> inner) { return {OPTIONAL, "", inner}; }
    static Type Array(std::shared_ptr<Type> inner = nullptr) { return {ARRAY, "", inner}; }
    static Type Custom(const std::string& name) { return {CUSTOM, name}; }
};

// Base node location
struct NodeLocation {
    SourceLocation start;
    SourceLocation end;
};

// Expression nodes
struct Expr {
    virtual ~Expr() = default;
    NodeLocation loc;
};

using ExprPtr = std::unique_ptr<Expr>;

// Literals
struct LiteralExpr : Expr {
    std::variant<long long, double, std::string, bool, std::nullptr_t> value;
    Type type;
    
    template<typename T>
    LiteralExpr(T val, Type t) : value(val), type(t) {}
};

struct IdentifierExpr : Expr {
    std::string name;
};

struct TupleExpr : Expr {
    std::vector<ExprPtr> elements;
};

struct ArrayExpr : Expr {
    std::vector<ExprPtr> elements;
};

struct DictExpr : Expr {
    std::vector<std::pair<ExprPtr, ExprPtr>> pairs;
};

struct LambdaExpr : Expr {
    std::vector<std::string> params;
    std::vector<Type> paramTypes;
    Type returnType;
    std::vector<StmtPtr> body;
};

struct BinaryExpr : Expr {
    ExprPtr left;
    TokenType op;
    ExprPtr right;
};

struct UnaryExpr : Expr {
    TokenType op;
    ExprPtr operand;
};

struct CallExpr : Expr {
    ExprPtr callee;
    std::vector<ExprPtr> args;
};

struct IndexExpr : Expr {
    ExprPtr object;
    ExprPtr index;
};

struct MemberExpr : Expr {
    ExprPtr object;
    std::string member;
    bool isOptional = false;
};

struct PipeExpr : Expr {
    ExprPtr left;
    ExprPtr right;  // function to pipe into
};

struct CastExpr : Expr {
    ExprPtr expr;
    Type targetType;
};

struct MatchExpr : Expr {
    ExprPtr value;
    struct Case {
        std::vector<ExprPtr> patterns;
        ExprPtr body;
    };
    std::vector<Case> cases;
};

// SQL-like query expression: select * from collection where condition
struct QueryExpr : Expr {
    std::vector<ExprPtr> select;  // что выбираем (*
    ExprPtr from;                 // откуда
    ExprPtr where;                // условие
    std::vector<std::pair<ExprPtr, bool>> orderBy;  // поле, asc/desc
    std::vector<ExprPtr> groupBy;
    ExprPtr into;                 // сохранить в переменную
};

// Statement nodes
struct Stmt {
    virtual ~Stmt() = default;
    NodeLocation loc;
};

struct ExprStmt : Stmt {
    ExprPtr expr;
};

struct LetStmt : Stmt {
    std::string name;
    bool isMutable = false;
    bool isScoped = false;  // исчезает после блока
    std::shared_ptr<Type> type;  // optional type annotation
    ExprPtr init;
};

struct AssignStmt : Stmt {
    ExprPtr target;
    ExprPtr value;
};

struct BlockStmt : Stmt {
    std::vector<StmtPtr> statements;
};

struct IfStmt : Stmt {
    ExprPtr condition;
    StmtPtr thenBranch;
    StmtPtr elseBranch;  // can be ifStmt or blockStmt
};

struct MatchStmt : Stmt {
    ExprPtr value;
    struct Case {
        std::vector<ExprPtr> patterns;
        StmtPtr body;
    };
    std::vector<Case> cases;
};

struct ForStmt : Stmt {
    std::string varName;
    ExprPtr iterable;
    StmtPtr body;
    bool isParallel = false;
};

struct WhileStmt : Stmt {
    ExprPtr condition;
    StmtPtr body;
};

struct FnStmt : Stmt {
    std::string name;
    std::vector<std::pair<std::string, std::shared_ptr<Type>>> params;
    std::shared_ptr<Type> returnType;
    std::vector<StmtPtr> body;
    
    // Contracts
    struct Contract {
        enum Kind { REQUIRE, ENSURE, FIX } kind;
        ExprPtr condition;     // условие
        ExprPtr fixExpr;       // выражение для авто-исправления (только для FIX)
        std::string message;
    };
    std::vector<Contract> contracts;
};

struct ReturnStmt : Stmt {
    ExprPtr value;
};

struct StructStmt : Stmt {
    std::string name;
    std::vector<std::pair<std::string, std::shared_ptr<Type>>> fields;
};

struct EnumStmt : Stmt {
    std::string name;
    std::vector<std::pair<std::string, std::vector<std::shared_ptr<Type>>>> variants;
};

struct ImportStmt : Stmt {
    std::string path;
    std::string alias;
};

struct ModuleStmt : Stmt {
    std::string name;
    std::vector<StmtPtr> body;
};

// Top-level program
struct Program {
    std::vector<StmtPtr> statements;
};

} // namespace smirnovlang
