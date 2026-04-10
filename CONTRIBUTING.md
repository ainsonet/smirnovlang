# Contributing to SmirnovLang

Thank you for your interest in contributing to SmirnovLang! This document outlines the guidelines and processes for contributing.

## How to Contribute

### 1. Reporting Bugs

Before creating bug reports, please check existing issues. When creating a bug report, include as much detail as possible:

- **Use a clear and descriptive title**
- **Describe the issue** with steps to reproduce
- **Include environment details** (OS, compiler version, etc.)
- **Add code examples** if applicable

### 2. Suggesting Enhancements

Enhancement suggestions are welcome! Include:

- **Clear title and description**
- **Step-by-step reproduction** for any related issues
- **Use cases** for the enhancement
- **Possible implementation ideas**

### 3. Pull Requests

- **Fork the repository** and create your branch from `master`
- **Follow the coding style** (consistent indentation, meaningful names)
- **Document new code** with comments
- **Test your changes** thoroughly
- **Write clear commit messages**

### Coding Standards

- **C++20** standard compliance
- **Consistent indentation** (4 spaces)
- **Descriptive variable/function names**
- **Comments for complex logic**
- **Error handling** with meaningful messages

### Commit Message Format

```
type: short description

detailed explanation (optional)

Closes #issue-number
```

Types: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`

## Development Setup

```bash
# Clone repository
git clone https://github.com/ainsonet/smirnovlang.git
cd smirnovlang

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

## Testing

```bash
# Run all tests
cd build
ctest --output-on-failure

# Run specific test
ctest -R test-name
```

## Code of Conduct

- Be respectful and inclusive
- Provide constructive feedback
- Accept constructive criticism
- Focus on what's best for the community

## Questions?

Open a discussion issue or reach out to the maintainers.

Thank you for contributing! 🚀
