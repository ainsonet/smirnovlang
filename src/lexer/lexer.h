#pragma once

#include "token.h"
#include <string>
#include <vector>
#include <cctype>

namespace smirnovlang {

class Lexer {
public:
    Lexer(const std::string& source, std::string filename = "input")
        : source_(source), filename_(filename), pos_(0), line_(1), column_(1) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        
        while (!isAtEnd()) {
            skipWhitespace();
            if (isAtEnd()) break;
            
            // Reset location for each token
            startLoc_ = currentLocation();
            char c = peek();
            
            if (std::isalpha(c) || c == '_') {
                tokens.push_back(identifier());
            } else if (std::isdigit(c)) {
                tokens.push_back(number());
            } else if (c == '"') {
                tokens.push_back(string());
            } else {
                tokens.push_back(symbol());
            }
        }
        
        tokens.push_back({TokenType::EOF_TOKEN, {}, {line_, column_, filename_}});
        return tokens;
    }

private:
    bool isAtEnd() const { return pos_ >= source_.size(); }
    char peek() const { return isAtEnd() ? '\0' : source_[pos_]; }
    char peekNext() const { return pos_ + 1 >= source_.size() ? '\0' : source_[pos_ + 1]; }
    
    SourceLocation currentLocation() const {
        return {line_, column_, filename_};
    }

    void advance() {
        if (peek() == '\n') {
            line_++;
            column_ = 1;
        } else {
            column_++;
        }
        pos_++;
    }

    char consume() {
        char c = peek();
        advance();
        return c;
    }

    bool match(char expected) {
        if (isAtEnd()) return false;
        if (source_[pos_] != expected) return false;
        advance();
        return true;
    }

    void skipWhitespace() {
        while (!isAtEnd()) {
            char c = peek();
            if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                consume();
            } else if (c == '/' && peekNext() == '/') {
                // Single line comment
                while (!isAtEnd() && peek() != '\n') consume();
            } else if (c == '/' && peekNext() == '*') {
                // Multi-line comment
                consume(); consume();
                while (!isAtEnd()) {
                    if (peek() == '*' && peekNext() == '/') {
                        consume(); consume();
                        break;
                    }
                    consume();
                }
            } else {
                break;
            }
        }
    }

    Token identifier() {
        std::string value;
        while (std::isalnum(peek()) || peek() == '_') {
            value += consume();
        }
        
        // Keywords
        static const std::unordered_map<std::string, TokenType> keywords = {
            {"fn", TokenType::FN}, {"let", TokenType::LET}, {"mut", TokenType::MUT},
            {"if", TokenType::IF}, {"else", TokenType::ELSE}, {"match", TokenType::MATCH},
            {"for", TokenType::FOR}, {"while", TokenType::WHILE}, {"return", TokenType::RETURN},
            {"require", TokenType::REQUIRE}, {"ensure", TokenType::ENSURE}, {"fix", TokenType::FIX},
            {"memo", TokenType::MEMO},
            {"parallel", TokenType::PARALLEL}, {"concurrent", TokenType::CONCURRENT},
            {"true", TokenType::TRUE}, {"false", TokenType::FALSE}, {"null", TokenType::NULLPTR},
            {"pub", TokenType::PUB}, {"mod", TokenType::MOD}, {"use", TokenType::USE},
            {"struct", TokenType::STRUCT}, {"enum", TokenType::ENUM},
            {"class", TokenType::CLASS}, {"import", TokenType::IMPORT},
            {"in", TokenType::IN},
            // Testing - UNIQUE FEATURE!
            {"test", TokenType::TEST}, {"assert", TokenType::ASSERT},
            {"doc", TokenType::DOC},
            // SQL-like query keywords
            {"select", TokenType::SELECT}, {"from", TokenType::FROM}, {"where", TokenType::WHERE},
            {"order", TokenType::ORDER}, {"by", TokenType::BY}, {"group", TokenType::GROUP}, {"into", TokenType::INTO},
        };
        
        auto it = keywords.find(value);
        TokenType type = (it != keywords.end()) ? it->second : TokenType::IDENT;
        
        return {type, value, startLoc_};
    }

    Token number() {
        std::string value;
        bool isFloat = false;
        
        while (std::isdigit(peek()) || peek() == '.' || peek() == 'e' || peek() == 'E') {
            if (peek() == '.') {
                if (peekNext() == '.' || !std::isdigit(peekNext())) break;
                isFloat = true;
            }
            value += consume();
        }
        
        if (isFloat) {
            return {TokenType::FLOAT, std::stod(value), startLoc_};
        } else {
            return {TokenType::INTEGER, std::stoll(value), startLoc_};
        }
    }

    Token string() {
        consume(); // opening "
        std::string value;
        
        while (!isAtEnd() && peek() != '"') {
            if (peek() == '\\') {
                consume();
                switch (peek()) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '"': value += '"'; break;
                    case '\\': value += '\\'; break;
                    default: value += peek();
                }
            } else {
                value += consume();
            }
        }
        
        consume(); // closing "
        return {TokenType::STRING, value, startLoc_};
    }

    Token symbol() {
        char c = consume();
        
        // Two-character operators
        char next = peek();
        
        // Arrow
        if (c == '-' && next == '>') {
            consume();
            return {TokenType::ARROW, {}, startLoc_};
        }
        
        // Equality
        if (c == '=' && next == '=') { consume(); return {TokenType::EQEQ, {}, startLoc_}; }
        if (c == '!' && next == '=') { consume(); return {TokenType::BANGEQ, {}, startLoc_}; }
        
        // Comparison
        if (c == '<' && next == '=') { consume(); return {TokenType::LTE, {}, startLoc_}; }
        if (c == '>' && next == '=') { consume(); return {TokenType::GTE, {}, startLoc_}; }
        
        // Range
        if (c == '.' && next == '.') {
            consume();
            if (peek() == '=') {
                consume();
                return {TokenType::RANGE_INCLUSIVE, {}, startLoc_};
            }
            return {TokenType::RANGE, {}, startLoc_};
        }
        
        // Pipe operators
        if (c == '|' && next == '>') { consume(); return {TokenType::PIPE_ARROW, {}, startLoc_}; }
        if (c == '|' && next == '|') { consume(); return {TokenType::OR, {}, startLoc_}; }
        
        // Null coalescing ??
        if (c == '?' && next == '?') { consume(); return {TokenType::NULL_COALESCE, {}, startLoc_}; }
        
        // Safe navigation ?.
        if (c == '?' && next == '.') { consume(); return {TokenType::QUESTION_DOT, {}, startLoc_}; }
        
        // Single character tokens
        switch (c) {
            case '+': return {TokenType::PLUS, {}, startLoc_};
            case '-': return {TokenType::MINUS, {}, startLoc_};
            case '*': return {TokenType::STAR, {}, startLoc_};
            case '/': return {TokenType::SLASH, {}, startLoc_};
            case '%': return {TokenType::PERCENT, {}, startLoc_};
            case '=': return {TokenType::EQ, {}, startLoc_};
            case '<': return {TokenType::LT, {}, startLoc_};
            case '>': return {TokenType::GT, {}, startLoc_};
            case '!': return {TokenType::NOT, {}, startLoc_};
            case '&': return {TokenType::AMP, {}, startLoc_};
            case '|': return {TokenType::PIPE, {}, startLoc_};
            case '?': return {TokenType::QUESTION, {}, startLoc_};
            case '(': return {TokenType::LPAREN, {}, startLoc_};
            case ')': return {TokenType::RPAREN, {}, startLoc_};
            case '{': return {TokenType::LBRACE, {}, startLoc_};
            case '}': return {TokenType::RBRACE, {}, startLoc_};
            case '[': return {TokenType::LBRACKET, {}, startLoc_};
            case ']': return {TokenType::RBRACKET, {}, startLoc_};
            case ',': return {TokenType::COMMA, {}, startLoc_};
            case ':': return {TokenType::COLON, {}, startLoc_};
            case ';': return {TokenType::SEMICOLON, {}, startLoc_};
            case '.': return {TokenType::DOT, {}, startLoc_};
            default: return {TokenType::ERROR, std::string("Unknown character: ") + c, startLoc_};
        }
    }

    std::string source_;
    std::string filename_;
    size_t pos_;
    size_t line_;
    size_t column_;
    SourceLocation startLoc_;
};

} // namespace smirnovlang
