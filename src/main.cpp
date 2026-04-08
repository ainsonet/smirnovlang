#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "lexer/lexer.h"
#include "parser/parser.h"
#include "vm/vm.h"

using namespace smirnovlang;

void printUsage(const char* programName) {
    std::cout << "SmirnovLang v0.1.0\n";
    std::cout << "Usage: " << programName << " [options] <file.smirnov>\n";
    std::cout << "Options:\n";
    std::cout << "  -c, --compile  Compile to output (not implemented)\n";
    std::cout << "  -r, --repl     Start REPL mode\n";
    std::cout << "  -h, --help     Show this help\n";
}

void runREPL() {
    std::cout << "SmirnovLang REPL v0.1.0\n";
    std::cout << "Type 'exit' to quit\n\n";
    
    VM vm;
    std::string line;
    
    while (true) {
        std::cout << ">>> ";
        if (!std::getline(std::cin, line)) break;
        
        if (line == "exit" || line == "quit") break;
        if (line.empty()) continue;
        
        try {
            Lexer lexer(line, "repl");
            auto tokens = lexer.tokenize();
            
            Parser parser(tokens);
            auto program = parser.parse();
            
            VM localVm;
            localVm.execute(program);
            
            if (!localVm.getErrors().empty()) {
                for (auto& err : localVm.getErrors()) {
                    std::cerr << "Error: " << err << "\n";
                }
            } else {
                auto result = localVm.getResult();
                if (!result.isNil()) {
                    std::cout << result.toString() << "\n";
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }
}

void runFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file '" << filename << "'\n";
        exit(1);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    
    // Tokenize
    Lexer lexer(source, filename);
    auto tokens = lexer.tokenize();
    
    // Check for lexer errors
    for (const auto& tok : tokens) {
        if (tok.type == TokenType::ERROR) {
            std::cerr << "Lexer error at " << tok.loc.line << ":" << tok.loc.column 
                      << ": " << std::get<std::string>(tok.value) << "\n";
            exit(1);
        }
    }
    
    // Parse
    Parser parser(tokens);
    Program program = parser.parse();
    
    // Check for parser errors
    if (!parser.errors.empty()) {
        for (auto& err : parser.errors) {
            std::cerr << "Parse error: " << err << "\n";
        }
        exit(1);
    }
    
    // Execute
    VM vm;
    vm.execute(program);
    
    // Print errors
    for (auto& err : vm.getErrors()) {
        std::cerr << "Runtime error: " << err << "\n";
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string arg = argv[1];
    
    if (arg == "-h" || arg == "--help") {
        printUsage(argv[0]);
        return 0;
    }
    
    if (arg == "-r" || arg == "--repl") {
        runREPL();
        return 0;
    }
    
    if (arg == "-c" || arg == "--compile") {
        std::cerr << "Compilation not yet implemented\n";
        return 1;
    }
    
    // Assume it's a file
    runFile(arg);
    
    return 0;
}
