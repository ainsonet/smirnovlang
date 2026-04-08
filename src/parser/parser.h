#pragma once

#include "lexer/lexer.h"
#include "ast/ast.h"
#include <vector>
#include <memory>

namespace smirnovlang {

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens) 
        : tokens_(tokens), current_(0) {}

    Program parse() {
        Program program;
        while (!isAtEnd()) {
            if (auto stmt = parseStatement()) {
                program.statements.push_back(std::move(stmt));
            }
        }
        return program;
    }

private:
    const std::vector<Token>& tokens_;
    size_t current_;

    // Helper methods
    bool isAtEnd() const { 
        return peek().type == TokenType::EOF_TOKEN; 
    }
    
    const Token& peek() const { 
        return tokens_[current_]; 
    }
    
    const Token& previous() const { 
        return tokens_[current_ - 1]; 
    }
    
    bool check(TokenType type) const {
        if (isAtEnd()) return false;
        return peek().type == type;
    }
    
    bool match(std::initializer_list<TokenType> types) {
        for (TokenType type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }
    
    const Token& advance() {
        if (!isAtEnd()) current_++;
        return previous();
    }

    // Error handling
    std::vector<std::string> errors;
    
    void error(const std::string& message) {
        errors.push_back(message);
    }

    // Statement parsing
    StmtPtr parseStatement() {
        if (match({TokenType::LET, TokenType::MUT})) {
            return parseLetStatement();
        }
        if (match({TokenType::FN})) {
            return parseFnStatement();
        }
        if (match({TokenType::STRUCT})) {
            return parseStructStatement();
        }
        if (match({TokenType::ENUM})) {
            return parseEnumStatement();
        }
        if (match({TokenType::IMPORT})) {
            return parseImportStatement();
        }
        if (match({TokenType::MODULE})) {
            return parseModuleStatement();
        }
        if (match({TokenType::FOR})) {
            return parseForStatement();
        }
        if (match({TokenType::WHILE})) {
            return parseWhileStatement();
        }
        if (match({TokenType::IF})) {
            return parseIfStatement();
        }
        if (match({TokenType::MATCH})) {
            return parseMatchStatement();
        }
        if (match({TokenType::RETURN})) {
            return parseReturnStatement();
        }
        
        // Block or expression
        if (check(TokenType::LBRACE)) {
            return parseBlock();
        }
        
        // Default to expression statement
        auto expr = parseExpression();
        match({TokenType::SEMICOLON});
        auto stmt = std::make_unique<ExprStmt>();
        stmt->expr = std::move(expr);
        return stmt;
    }

    StmtPtr parseLetStatement() {
        bool isMutable = previous().type == TokenType::MUT;
        bool isScoped = false;
        
        // Check for scoped variable (let!)
        if (check(TokenType::IDENT)) {
            std::string ident = std::get<std::string>(peek().value);
            if (!ident.empty() && ident[0] == '!') {
                isScoped = true;
                advance(); // consume !
                // Get actual variable name
                if (!check(TokenType::IDENT)) {
                    error("Expected variable name after '!'");
                    return nullptr;
                }
            }
        }
        
        if (!check(TokenType::IDENT)) {
            error("Expected variable name after let/mut");
            return nullptr;
        }
        
        std::string name = std::get<std::string>(advance().value);
        
        // Type annotation
        std::shared_ptr<Type> type = nullptr;
        if (match({TokenType::COLON})) {
            type = parseType();
        }
        
        // Initializer
        ExprPtr init = nullptr;
        if (match({TokenType::EQ})) {
            init = parseExpression();
        }
        
        match({TokenType::SEMICOLON});
        
        auto stmt = std::make_unique<LetStmt>();
        stmt->name = name;
        stmt->isMutable = isMutable;
        stmt->isScoped = isScoped;
        stmt->type = type;
        stmt->init = std::move(init);
        return stmt;
    }

    StmtPtr parseFnStatement() {
        if (!check(TokenType::IDENT)) {
            error("Expected function name");
            return nullptr;
        }
        
        auto fn = std::make_unique<FnStmt>();
        fn->name = std::get<std::string>(advance().value);
        
        // Parameters
        if (!match({TokenType::LPAREN})) {
            error("Expected '(' after function name");
            return nullptr;
        }
        
        if (!check(TokenType::RPAREN)) {
            do {
                if (!check(TokenType::IDENT)) {
                    error("Expected parameter name");
                    break;
                }
                std::string paramName = std::get<std::string>(advance().value);
                
                std::shared_ptr<Type> paramType = nullptr;
                if (match({TokenType::COLON})) {
                    paramType = parseType();
                }
                
                fn->params.emplace_back(paramName, paramType);
            } while (match({TokenType::COMMA}));
        }
        
        if (!match({TokenType::RPAREN})) {
            error("Expected ')' after parameters");
            return nullptr;
        }
        
        // Return type
        if (match({TokenType::ARROW})) {
            fn->returnType = parseType();
        }
        
        // Contracts (require/ensure/fix)
        while (match({TokenType::REQUIRE, TokenType::ENSURE, TokenType::FIX})) {
            FnStmt::Contract contract;
            if (previous().type == TokenType::REQUIRE) {
                contract.kind = FnStmt::Contract::REQUIRE;
            } else if (previous().type == TokenType::FIX) {
                contract.kind = FnStmt::Contract::FIX;
            } else {
                contract.kind = FnStmt::Contract::ENSURE;
            }
            
            contract.condition = parseExpression();
            
            // Parse optional message
            if (match({TokenType::COMMA})) {
                if (check(TokenType::STRING)) {
                    contract.message = std::get<std::string>(advance().value);
                }
            }
            
            // Parse fix expression for FIX contracts
            if (contract.kind == FnStmt::Contract::FIX) {
                if (match({TokenType::COMMA}) || match({TokenType::IDENT})) {
                    // Check if there's a fix: expression
                    // Reset and parse properly
                    if (previous().type != TokenType::IDENT || std::get<std::string>(previous().value) != "fix") {
                        // Not a fix expression, just continue
                    } else {
                        // Already consumed "fix"
                    }
                }
                // Try to parse fix: expression
                if (match({TokenType::IDENT})) {
                    std::string ident = std::get<std::string>(previous().value);
                    if (ident == "fix") {
                        if (match({TokenType::COLON})) {
                            contract.fixExpr = parseExpression();
                        }
                    }
                }
            }
            
            fn->contracts.push_back(std::move(contract));
        }
        
        // Body
        if (check(TokenType::LBRACE)) {
            fn->body = parseBlockStatements();
        } else if (match({TokenType::ARROW})) {
            // Arrow syntax for single expression: fn add(a, b) => a + b
            auto expr = parseExpression();
            auto block = std::make_unique<BlockStmt>();
            auto ret = std::make_unique<ReturnStmt>();
            ret->value = std::move(expr);
            block->statements.push_back(std::move(ret));
            fn->body.push_back(std::move(block));
        } else {
            error("Expected function body");
        }
        
        return fn;
    }

    std::shared_ptr<Type> parseType() {
        std::shared_ptr<Type> type = std::make_shared<Type>();
        
        if (match({TokenType::QUESTION})) {
            // Optional type
            type->kind = Type::OPTIONAL;
            type->inner = parseType();
            return type;
        }
        
        if (match({TokenType::IDENT})) {
            std::string name = std::get<std::string>(previous().value);
            if (name == "int") type->kind = Type::INT;
            else if (name == "float") type->kind = Type::FLOAT;
            else if (name == "bool") type->kind = Type::BOOL;
            else if (name == "string") type->kind = Type::STRING;
            else if (name == "char") type->kind = Type::CHAR;
            else if (name == "void") type->kind = Type::VOID;
            else if (name == "auto") type->kind = Type::AUTO;
            else {
                type->kind = Type::CUSTOM;
                type->name = name;
            }
        }
        
        // Array type
        if (match({TokenType::LBRACKET})) {
            if (!match({TokenType::RBRACKET})) {
                error("Expected ']' in array type");
            }
            auto arrType = std::make_shared<Type>();
            arrType->kind = Type::ARRAY;
            arrType->inner = type;
            return arrType;
        }
        
        return type;
    }

    StmtPtr parseStructStatement() {
        if (!check(TokenType::IDENT)) {
            error("Expected struct name");
            return nullptr;
        }
        
        auto structStmt = std::make_unique<StructStmt>();
        structStmt->name = std::get<std::string>(advance().value);
        
        if (!match({TokenType::LBRACE})) {
            error("Expected '{' after struct name");
            return nullptr;
        }
        
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            if (!check(TokenType::IDENT)) {
                error("Expected field name");
                break;
            }
            std::string fieldName = std::get<std::string>(advance().value);
            
            std::shared_ptr<Type> fieldType = nullptr;
            if (match({TokenType::COLON})) {
                fieldType = parseType();
            }
            
            match({TokenType::COMMA});
            structStmt->fields.emplace_back(fieldName, fieldType);
        }
        
        match({TokenType::RBRACE});
        return structStmt;
    }

    StmtPtr parseEnumStatement() {
        if (!check(TokenType::IDENT)) {
            error("Expected enum name");
            return nullptr;
        }
        
        auto enumStmt = std::make_unique<EnumStmt>();
        enumStmt->name = std::get<std::string>(advance().value);
        
        if (!match({TokenType::LBRACE})) {
            error("Expected '{' after enum name");
            return nullptr;
        }
        
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            if (!check(TokenType::IDENT)) {
                error("Expected variant name");
                break;
            }
            std::string variantName = std::get<std::string>(advance().value);
            
            std::vector<std::shared_ptr<Type>> variantTypes;
            if (match({TokenType::LPAREN})) {
                do {
                    variantTypes.push_back(parseType());
                } while (match({TokenType::COMMA}));
                match({TokenType::RPAREN});
            }
            
            match({TokenType::COMMA});
            enumStmt->variants.emplace_back(variantName, variantTypes);
        }
        
        match({TokenType::RBRACE});
        return enumStmt;
    }

    StmtPtr parseImportStatement() {
        if (!check(TokenType::STRING)) {
            error("Expected import path");
            return nullptr;
        }
        
        auto import = std::make_unique<ImportStmt>();
        import->path = std::get<std::string>(advance().value);
        
        if (match({TokenType::AS})) {
            if (!check(TokenType::IDENT)) {
                error("Expected alias after 'as'");
            }
            import->alias = std::get<std::string>(advance().value);
        }
        
        match({TokenType::SEMICOLON});
        return import;
    }

    StmtPtr parseModuleStatement() {
        if (!check(TokenType::IDENT)) {
            error("Expected module name");
            return nullptr;
        }
        
        auto module = std::make_unique<ModuleStmt>();
        module->name = std::get<std::string>(advance().value);
        
        if (!match({TokenType::LBRACE})) {
            error("Expected '{' after module name");
            return nullptr;
        }
        
        module->body = parseBlockStatements();
        return module;
    }

    StmtPtr parseForStatement() {
        bool isParallel = match({TokenType::PARALLEL});
        
        if (!check(TokenType::IDENT)) {
            error("Expected loop variable name");
            return nullptr;
        }
        
        auto forStmt = std::make_unique<ForStmt>();
        forStmt->varName = std::get<std::string>(advance().value);
        forStmt->isParallel = isParallel;
        
        if (!match({TokenType::IN})) {
            error("Expected 'in' after loop variable");
            return nullptr;
        }
        
        forStmt->iterable = parseExpression();
        
        if (check(TokenType::LBRACE)) {
            forStmt->body = parseBlock();
        } else {
            forStmt->body = parseStatement();
        }
        
        return forStmt;
    }

    StmtPtr parseWhileStatement() {
        auto whileStmt = std::make_unique<WhileStmt>();
        whileStmt->condition = parseExpression();
        
        if (check(TokenType::LBRACE)) {
            whileStmt->body = parseBlock();
        } else {
            whileStmt->body = parseStatement();
        }
        
        return whileStmt;
    }

    StmtPtr parseIfStatement() {
        auto ifStmt = std::make_unique<IfStmt>();
        ifStmt->condition = parseExpression();
        
        if (check(TokenType::LBRACE)) {
            ifStmt->thenBranch = parseBlock();
        } else {
            ifStmt->thenBranch = parseStatement();
        }
        
        if (match({TokenType::ELSE})) {
            if (check(TokenType::IF)) {
                ifStmt->elseBranch = parseIfStatement();
            } else if (check(TokenType::LBRACE)) {
                ifStmt->elseBranch = parseBlock();
            } else {
                ifStmt->elseBranch = parseStatement();
            }
        }
        
        return ifStmt;
    }

    StmtPtr parseMatchStatement() {
        auto matchStmt = std::make_unique<MatchStmt>();
        matchStmt->value = parseExpression();
        
        if (!match({TokenType::LBRACE})) {
            error("Expected '{' after match value");
            return nullptr;
        }
        
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            MatchStmt::Case caseStmt;
            
            // Patterns
            do {
                caseStmt.patterns.push_back(parseExpression());
            } while (match({TokenType::COMMA}));
            
            if (!match({TokenType::ARROW}) && !match({TokenType::COLON})) {
                error("Expected '->' or ':' after match pattern");
            }
            
            if (check(TokenType::LBRACE)) {
                caseStmt.body = parseBlock();
            } else {
                caseStmt.body = parseStatement();
            }
            
            matchStmt->cases.push_back(std::move(caseStmt));
        }
        
        match({TokenType::RBRACE});
        return matchStmt;
    }

    StmtPtr parseReturnStatement() {
        auto ret = std::make_unique<ReturnStmt>();
        
        if (!check(TokenType::SEMICOLON) && !check(TokenType::RBRACE)) {
            ret->value = parseExpression();
        }
        
        match({TokenType::SEMICOLON});
        return ret;
    }

    StmtPtr parseBlock() {
        auto block = std::make_unique<BlockStmt>();
        block->statements = parseBlockStatements();
        return block;
    }

    std::vector<StmtPtr> parseBlockStatements() {
        std::vector<StmtPtr> statements;
        
        if (!match({TokenType::LBRACE})) {
            error("Expected '{'");
            return statements;
        }
        
        while (!check(TokenType::RBRACE) && !isAtEnd()) {
            if (auto stmt = parseStatement()) {
                statements.push_back(std::move(stmt));
            }
        }
        
        match({TokenType::RBRACE});
        return statements;
    }

    // Expression parsing (precedence climbing)
    ExprPtr parseExpression() {
        return parsePipe();
    }

    ExprPtr parsePipe() {
        auto left = parseOr();
        
        while (match({TokenType::PIPE_ARROW})) {
            auto right = parseOr();
            auto pipe = std::make_unique<PipeExpr>();
            pipe->left = std::move(left);
            pipe->right = std::move(right);
            left = std::move(pipe);
        }
        
        return left;
    }

    ExprPtr parseOr() {
        auto left = parseAnd();
        
        while (match({TokenType::OR, TokenType::PIPE})) {
            auto right = parseAnd();
            auto binary = std::make_unique<BinaryExpr>();
            binary->left = std::move(left);
            binary->op = previous().type;
            binary->right = std::move(right);
            left = std::move(binary);
        }
        
        return left;
    }

    ExprPtr parseAnd() {
        auto left = parseEquality();
        
        while (match({TokenType::AMP, TokenType::AND})) {
            auto right = parseEquality();
            auto binary = std::make_unique<BinaryExpr>();
            binary->left = std::move(left);
            binary->op = previous().type;
            binary->right = std::move(right);
            left = std::move(binary);
        }
        
        return left;
    }

    ExprPtr parseEquality() {
        auto left = parseComparison();
        
        while (match({TokenType::EQEQ, TokenType::BANGEQ})) {
            auto right = parseComparison();
            auto binary = std::make_unique<BinaryExpr>();
            binary->left = std::move(left);
            binary->op = previous().type;
            binary->right = std::move(right);
            left = std::move(binary);
        }
        
        return left;
    }

    ExprPtr parseComparison() {
        auto left = parseRange();
        
        while (match({TokenType::LT, TokenType::LTE, TokenType::GT, TokenType::GTE})) {
            auto right = parseRange();
            auto binary = std::make_unique<BinaryExpr>();
            binary->left = std::move(left);
            binary->op = previous().type;
            binary->right = std::move(right);
            left = std::move(binary);
        }
        
        return left;
    }

    ExprPtr parseRange() {
        auto left = parseAdditive();
        
        if (match({TokenType::RANGE, TokenType::RANGE_INCLUSIVE})) {
            auto right = parseAdditive();
            auto binary = std::make_unique<BinaryExpr>();
            binary->left = std::move(left);
            binary->op = previous().type;
            binary->right = std::move(right);
            left = std::move(binary);
        }
        
        return left;
    }

    ExprPtr parseAdditive() {
        auto left = parseMultiplicative();
        
        while (match({TokenType::PLUS, TokenType::MINUS})) {
            auto right = parseMultiplicative();
            auto binary = std::make_unique<BinaryExpr>();
            binary->left = std::move(left);
            binary->op = previous().type;
            binary->right = std::move(right);
            left = std::move(binary);
        }
        
        return left;
    }

    ExprPtr parseMultiplicative() {
        auto left = parseUnary();
        
        while (match({TokenType::STAR, TokenType::SLASH, TokenType::PERCENT})) {
            auto right = parseUnary();
            auto binary = std::make_unique<BinaryExpr>();
            binary->left = std::move(left);
            binary->op = previous().type;
            binary->right = std::move(right);
            left = std::move(binary);
        }
        
        return left;
    }

    ExprPtr parseUnary() {
        if (match({TokenType::NOT, TokenType::MINUS, TokenType::AMP, TokenType::STAR})) {
            auto op = previous().type;
            auto operand = parseUnary();
            auto unary = std::make_unique<UnaryExpr>();
            unary->op = op;
            unary->operand = std::move(operand);
            return unary;
        }
        
        return parsePostfix();
    }

    ExprPtr parsePostfix() {
        auto expr = parsePrimary();
        
        while (true) {
            if (match({TokenType::DOT})) {
                if (!check(TokenType::IDENT)) {
                    error("Expected member name after '.'");
                    break;
                }
                auto member = std::make_unique<MemberExpr>();
                member->object = std::move(expr);
                member->member = std::get<std::string>(advance().value);
                expr = std::move(member);
            }
            else if (match({TokenType::QUESTION_DOT})) {
                if (!check(TokenType::IDENT)) {
                    error("Expected member name after '?.'");
                    break;
                }
                auto member = std::make_unique<MemberExpr>();
                member->object = std::move(expr);
                member->member = std::get<std::string>(advance().value);
                member->isOptional = true;
                expr = std::move(member);
            }
            else if (match({TokenType::LBRACKET})) {
                auto index = std::make_unique<IndexExpr>();
                index->object = std::move(expr);
                index->index = parseExpression();
                if (!match({TokenType::RBRACKET})) {
                    error("Expected ']' after index");
                }
                expr = std::move(index);
            }
            else if (match({TokenType::LPAREN})) {
                auto call = std::make_unique<CallExpr>();
                call->callee = std::move(expr);
                
                if (!check(TokenType::RPAREN)) {
                    do {
                        call->args.push_back(parseExpression());
                    } while (match({TokenType::COMMA}));
                }
                
                if (!match({TokenType::RPAREN})) {
                    error("Expected ')' after arguments");
                }
                expr = std::move(call);
            }
            else {
                break;
            }
        }
        
        // Null coalescing ??
        if (match({TokenType::NULL_COALESCE})) {
            auto right = parseExpression();
            auto binary = std::make_unique<BinaryExpr>();
            binary->left = std::move(expr);
            binary->op = TokenType::NULL_COALESCE;
            binary->right = std::move(right);
            return binary;
        }
        
        return expr;
    }

    ExprPtr parsePrimary() {
        // Literals
        if (match({TokenType::INTEGER})) {
            auto lit = std::make_unique<LiteralExpr>(
                std::get<long long>(previous().value), Type::Int()
            );
            return lit;
        }
        
        if (match({TokenType::FLOAT})) {
            auto lit = std::make_unique<LiteralExpr>(
                std::get<double>(previous().value), Type::Float()
            );
            return lit;
        }
        
        if (match({TokenType::STRING})) {
            auto lit = std::make_unique<LiteralExpr>(
                std::get<std::string>(previous().value), Type::String()
            );
            return lit;
        }
        
        if (match({TokenType::TRUE, TokenType::FALSE})) {
            bool val = previous().type == TokenType::TRUE;
            auto lit = std::make_unique<LiteralExpr>(val, Type::Bool());
            return lit;
        }
        
        if (match({TokenType::NULLPTR})) {
            auto lit = std::make_unique<LiteralExpr>(nullptr, Type::Optional(std::make_shared<Type>(Type::VOID)));
            return lit;
        }
        
        // Grouping / tuple / lambda / match
        if (match({TokenType::LPAREN})) {
            // Check for tuple or grouping
            if (check(TokenType::RPAREN)) {
                // Empty tuple
                advance();
                auto tuple = std::make_unique<TupleExpr>();
                return tuple;
            }
            
            auto first = parseExpression();
            
            if (match({TokenType::COMMA})) {
                // Tuple
                auto tuple = std::make_unique<TupleExpr>();
                tuple->elements.push_back(std::move(first));
                
                while (!check(TokenType::RPAREN) && !isAtEnd()) {
                    match({TokenType::COMMA});
                    if (check(TokenType::RPAREN)) break;
                    tuple->elements.push_back(parseExpression());
                }
                
                match({TokenType::RPAREN});
                return tuple;
            }
            
            // Grouping or lambda
            if (match({TokenType::RPAREN})) {
                // Just grouping - return the expression
                return first;
            }
            
            // Check for arrow (lambda)
            if (match({TokenType::ARROW}) || match({TokenType::COMMA})) {
                // Lambda
                auto lambda = std::make_unique<LambdaExpr>();
                // This is a simplified lambda - we need proper param handling
                lambda->returnType = Type::Auto();
                
                // For now, just return a simple lambda with the expression as body
                auto block = std::make_unique<BlockStmt>();
                auto ret = std::make_unique<ReturnStmt>();
                ret->value = std::move(first);
                block->statements.push_back(std::move(ret));
                lambda->body.push_back(std::move(block));
                
                return lambda;
            }
        }
        
        // Array literal
        if (match({TokenType::LBRACKET})) {
            auto arr = std::make_unique<ArrayExpr>();
            
            if (!check(TokenType::RBRACKET)) {
                do {
                    arr->elements.push_back(parseExpression());
                } while (match({TokenType::COMMA}));
            }
            
            match({TokenType::RBRACKET});
            return arr;
        }
        
        // Identifier
        if (match({TokenType::IDENT})) {
            auto ident = std::make_unique<IdentifierExpr>();
            ident->name = std::get<std::string>(previous().value);
            return ident;
        }
        
        // Match expression
        if (match({TokenType::MATCH})) {
            auto matchExpr = std::make_unique<MatchExpr>();
            matchExpr->value = parseExpression();
            
            if (!match({TokenType::LBRACE})) {
                error("Expected '{' after match expression");
                return nullptr;
            }
            
            while (!check(TokenType::RBRACE) && !isAtEnd()) {
                MatchExpr::Case caseExpr;
                
                do {
                    caseExpr.patterns.push_back(parseExpression());
                } while (match({TokenType::COMMA}));
                
                if (!match({TokenType::ARROW})) {
                    error("Expected '->' in match expression");
                }
                
                caseExpr.body = parseExpression();
                matchExpr->cases.push_back(std::move(caseExpr));
            }
            
            match({TokenType::RBRACE});
            return matchExpr;
        }
        
        // Lambda
        if (match({TokenType::FN})) {
            auto lambda = std::make_unique<LambdaExpr>();
            
            if (match({TokenType::LPAREN})) {
                if (!check(TokenType::RPAREN)) {
                    do {
                        if (!check(TokenType::IDENT)) break;
                        lambda->params.push_back(std::get<std::string>(advance().value));
                    } while (match({TokenType::COMMA}));
                }
                match({TokenType::RPAREN});
            }
            
            if (match({TokenType::ARROW})) {
                lambda->returnType = Type::Auto();
                auto block = std::make_unique<BlockStmt>();
                auto ret = std::make_unique<ReturnStmt>();
                ret->value = parseExpression();
                block->statements.push_back(std::move(ret));
                lambda->body.push_back(std::move(block));
            } else if (check(TokenType::LBRACE)) {
                lambda->body = parseBlockStatements();
            }
            
            return lambda;
        }
        
        // SQL-like query: select ... from ... where ...
        if (match({TokenType::SELECT})) {
            return parseQueryExpression();
        }
        
        error("Unexpected token in expression");
        return nullptr;
    }

    // SQL-like query parser
    ExprPtr parseQueryExpression() {
        auto query = std::make_unique<QueryExpr>();
        
        // SELECT clause: * or comma-separated expressions
        if (match({TokenType::STAR})) {
            // select * - all fields
            auto star = std::make_unique<IdentifierExpr>();
            star->name = "*";
            query->select.push_back(std::move(star));
        } else {
            // select field1, field2, ...
            do {
                query->select.push_back(parseExpression());
            } while (match({TokenType::COMMA}));
        }
        
        // FROM clause (required)
        if (!match({TokenType::FROM})) {
            error("Expected 'from' in query");
            return nullptr;
        }
        query->from = parseExpression();
        
        // WHERE clause (optional)
        if (match({TokenType::WHERE})) {
            query->where = parseExpression();
        }
        
        // ORDER BY clause (optional)
        if (match({TokenType::ORDER})) {
            if (!match({TokenType::BY})) {
                error("Expected 'by' after 'order'");
                return nullptr;
            }
            do {
                auto field = parseExpression();
                bool asc = true;
                // check for ASC/DESC
                if (match({TokenType::IDENT})) {
                    std::string dir = std::get<std::string>(previous().value);
                    asc = (dir == "asc" || dir == "ASC");
                }
                query->orderBy.emplace_back(std::move(field), asc);
            } while (match({TokenType::COMMA}));
        }
        
        // GROUP BY clause (optional)
        if (match({TokenType::GROUP})) {
            if (!match({TokenType::BY})) {
                error("Expected 'by' after 'group'");
                return nullptr;
            }
            do {
                query->groupBy.push_back(parseExpression());
            } while (match({TokenType::COMMA}));
        }
        
        // INTO clause (optional) - store result in variable
        if (match({TokenType::INTO})) {
            query->into = parseExpression();
        }
        
        return query;
    }
};

} // namespace smirnovlang
