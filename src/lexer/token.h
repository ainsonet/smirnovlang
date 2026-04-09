#pragma once

#include <string>
#include <variant>
#include <optional>

namespace smirnovlang {

enum class TokenType {
    // Keywords
    FN, LET, MUT, IF, ELSE, MATCH, FOR, WHILE, RETURN,
    REQUIRE, ENSURE, PARALLEL, CONCURRENT, FIX, MEMO,
    TRUE, FALSE, NULLPTR,
    PUB, MOD, USE, STRUCT, ENUM, CLASS, IMPORT, IN,

    // SQL-like queries
    SELECT, FROM, WHERE, ORDER, BY, GROUP, INTO, LIMIT, OFFSET,

    // Operators
    PLUS, MINUS, STAR, SLASH, PERCENT,
    EQ, EQEQ, BANGEQ, LT, LTE, GT, GTE,
    AND, OR, NOT,
    AMP, PIPE, PIPE_ARROW,
    QUESTION, QUESTION_DOT, NULL_COALESCE,

    // Symbols
    LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET,
    COMMA, COLON, SEMICOLON, ARROW, DOT,
    RANGE, RANGE_INCLUSIVE,

    // Literals
    IDENT, STRING, INTEGER, FLOAT,

    // Special
    EOF_TOKEN, ERROR
};

struct SourceLocation {
    size_t line;
    size_t column;
    std::string filename;
};

struct Token {
    TokenType type;
    std::variant<std::string, long long, double> value;
    SourceLocation loc;
};

} // namespace smirnovlang
