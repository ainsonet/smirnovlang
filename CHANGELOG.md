# Changelog

All notable changes to SmirnovLang will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.1.0] - 2026

### Added
- **100+ new built-in functions** - most comprehensive DSL runtime
- Time travel debugging (`timeTravel`, `rewind`, `timeline`)
- Quantum superposition (`quantum`, `observe`)
- Pattern generation (`generate`)
- Mirror, echo, flow transformations
- Array operations: `chunk`, `flatten`, `zip`, `unzip`
- Collection utilities: `groupBy`, `countBy`, `unique`, `intersection`, `difference`, `union`
- Functional: `reduce`, `scan`, `foldr`, `iterate`, `until`, `compose`, `curry`
- String tools: `split`, `trim`, `padStart/End`, `startsWith/Ends`, `replace`
- Selectors: `first`, `last`, `rest`, `init`, `take`, `drop`, `takeWhile`, `dropWhile`
- Predicates: `all`, `any`, `none`, `contains`
- Utilities: `uuid`, `hash`, `measure`, `sample`, `shuffle`
- Performance: `sortBy`, `shuffle`, `chunk`, `flatten`
- Full string manipulation suite (30+ functions)

### Changed
- Improved error messages
- Enhanced pipeline operator performance

## [1.0.0] - 2026

### Added
- Синхронизирована версия (была 0.6.2 в коде)
- Добавлены функции File I/O: `readFile`, `writeFile`, `appendFile`, `fileExists`, `deleteFile`
- Расширены тесты (40+ тестов против 10)
- Улучшена документация
- Объединены встроенные функции в единую реализацию

### Fixed
- Исправлены критические ошибки компиляции
- Устранены проблемы с unique_ptr/shared_ptr

## [0.8.0] - 2026

### Added
- SQL-подобные запросы (SELECT, WHERE, ORDER BY, LIMIT, OFFSET)
- Data Versioning (`commit`, `rollback`, `history`, `diff`)
- Встроенное тестирование (`assert`)
- Auto-doc генерация (`doc`)

### Changed
- Улучшена производительность VM
- Оптимизированы pipeline-выражения

## [0.6.2] - 2026

### Added
- Базовые конструкции языка
- Функции с аннотациями типов
- Контракты (`require`, `ensure`, `fix`)
- Auto-memoization (`memo fn`)
- Pipeline выражения (`|>`)
- Pattern matching
- Циклы (`for`, `while`)
- Условные конструкции (`if/else`)
- Структуры и перечисления

### Changed
- Рефакторинг лексера и парсера
- Улучшена обработка ошибок

## [0.5.0] - 2026

### Added
- Базовый VM
- Лексер и парсер
- Система типов

---

[1.1.0]: https://github.com/ainsonet/smirnovlang/releases/tag/v1.1.0
[1.0.0]: https://github.com/ainsonet/smirnovlang/releases/tag/v1.0.0
[0.8.0]: https://github.com/ainsonet/smirnovlang/releases/tag/v0.8.0
[0.6.2]: https://github.com/ainsonet/smirnovlang/releases/tag/v0.6.2
[0.5.0]: https://github.com/ainsonet/smirnovlang/releases/tag/v0.5.0
