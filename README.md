#"" SmirnovLang

<div align="center">

**Современный язык программирования с инновационными возможностями**

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-green.svg)](https://isocpp.org/)
[![Release](https://img.shields.io/github/v/release/ainsonet/smirnovlang)](https://github.com/ainsonet/smirnovlang/releases)
[![Features](https://img.shields.io/badge/features-100%2B-orange.svg)](#уникальные-фичи)

[Сборка](#сборка) • [Документация](#документация) • [Примеры](#примеры) • [Вклад](CONTRIBUTING.md)

</div>

---


## 🚀 Статус: **Релиз v1.1.0**

Ядро языка полностью реализовано! Это один из самых функциональных предметно-ориентированных языков с уникальными возможностями.

**Готов к использованию в:**
- ✅ Образовательных целях
- ✅ Прототипировании
- ✅ Исследованиях
- ✅ Scripting-задачах

### Что работает в 1.1.0

✅ Лексический и синтаксический анализ  
✅ Выполнение программ в VM  
✅ Переменные (`let`, `mut`)  
✅ Scoped variables (`let!`) — исчезают после блока  
✅ Функции с аннотациями типов  
✅ Контракты (`require`, `ensure`, `fix`)  
✅ Auto-memoization (`memo fn`)  
✅ Pipeline-выражения (`|>`)  
✅ Pattern matching  
✅ Циклы (`for`, `while`)  
✅ Условные конструкции (`if/else`)  
✅ Структуры и перечисления  
✅ **Полные SQL-like запросы** (SELECT, WHERE, ORDER BY, GROUP BY, **LIMIT**, **OFFSET**)  
✅ **File I/O** (readFile, writeFile, appendFile, fileExists, deleteFile)  
✅ **Встроенное тестирование** (`assert(condition, message)`)  
✅ **Data Versioning** (`commit`, `rollback`, `history`, `diff`)  
✅ **Auto-doc generation** (`doc(function)`)  
✅ **100+ встроенных функций** (см. ниже)  

---

## 💡 Примеры

### Hello World

```smirnov
fn main() {
    let greeting = "Привет, SmirnovLang!";
    println(greeting);
}
```

### Pipeline и функциональный стиль

```smirnov
fn main() {
    let numbers = [1, 2, 3, 4, 5];
    
    // Pipeline-выражения
    let result = numbers
        |> filter(x => x > 2)
        |> map(x => x * 2)
        |> sum();
    
    println("Result:", result);  // 24
}
```

### SQL-like запросы

```smirnov
fn main() {
    let users = [
        {name: "Alice", age: 25, city: "NYC"},
        {name: "Bob", age: 17, city: "LA"},
        {name: "Charlie", age: 30, city: "NYC"}
    ];
    
    // Фильтрация и сортировка
    let adults = select * from users where age > 18 order by name;
    println("Adults:", adults);
    
    // Выборка полей
    let names = select name from users where city == "NYC";
    println("NYC names:", names);
    
    // С LIMIT и OFFSET
    let page1 = select * from users limit 2 offset 0;
    let page2 = select * from users limit 2 offset 2;
}
```

### Контракты с авто-исправлением

```smirnov
fn divide(a, b) -> f64
    require b != 0, "b cannot be zero", fix: 0.001
    ensure result >= 0, "result must be non-negative", fix: abs(result)
{
    return a / b;
}

fn main() {
    let result = divide(10, 0);  // Автоматически использует fix
    println("Result:", result);
}
```

### Data Versioning

```smirnov
fn main() {
    let data = {x: 10, y: 20};
    
    // Сохраняем версии
    commit(data, "v1");
    
    // Модифицируем
    data.x = 100;
    commit(data, "v2");
    
    // Восстанавливаем
    let restored = rollback("v1");
    println("Restored:", restored);
    
    // История
    history("v1");
    
    // Сравнение
    diff(rollback("v1"), rollback("v2"));
}
```

### Time Travel Debugging

```smirnov
fn main() {
    let data = [1, 2, 3, 4, 5];
    
    // Создаем контрольные точки
    timeTravel("record");
    let modified = data |> map(x => x * 2);
    timeTravel("record");
    
    // Возвращаемся назад
    timeTravel("rewind");
    
    // Проверяем timeline
    let checkpoints = timeTravel("timeline");
    println("Checkpoints:", checkpoints);
}
```

### Quantum Superposition

```smirnov
fn main() {
    // Создаем суперпозицию состояний
    let quantumState = quantum(42, "hello", 3.14);
    
    // Наблюдаем (коллапсируем)
    let observed = observe(quantumState);
    println("Observed:", observed);
}
```

### Встроенное тестирование

```smirnov
fn add(a, b) {
    return a + b;
}

fn main() {
    // Тесты
    assert(add(2, 3) == 5, "add(2, 3) should be 5");
    assert(add(-1, 1) == 0, "add(-1, 1) should be 0");
    
    println("All tests passed!");
}
```

---

## 🛠 Сборка

### Linux/macOS

```bash
git clone https://github.com/ainsonet/smirnovlang.git
cd smirnovlang/SmirnovLang
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Windows (PowerShell)

```powershell
git clone https://github.com/ainsonet/smirnovlang.git
cd smirnovlang\SmirnovLang
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

## 🚀 Запуск

```bash
# Hello World
./build/smirnovLang examples/hello.smirnov

# Более сложный пример
./build/smirnovLang examples/innovative.smirnov

# REPL режим
./build/smirnovLang --repl
```

## 🧪 Тесты

```bash
cd build
ctest --output-on-failure
```

---

## 📊 Встроенные функции

### Основные
- `print`, `println` - вывод
- `len`, `type` - информация о значениях
- `toInt`, `toFloat`, `toString` - конвертация

### Математика
- `sqrt`, `pow`, `abs`, `min`, `max`
- `sin`, `cos`, `tan` - тригонометрия

### Работа с массивами
- `map`, `filter`, `reduce`, `sum`
- `push`, `reverse`, `slice`, `chunk`, `flatten`
- `first`, `last`, `rest`, `init`
- `take`, `drop`, `takeWhile`, `dropWhile`
- `zip`, `unzip`, `unique`, `intersection`

### Строки
- `split`, `trim`, `join`, `concat`
- `padStart`, `padEnd`
- `startsWith`, `endsWith`
- `replace`, `substring`

### Data Versioning
- `commit`, `rollback`, `history`, `diff`

### Уникальные фичи
- `timeTravel` - time travel debugging
- `quantum`, `observe` - квантовые вычисления
- `generate` - генерация по шаблону
- `mirror`, `echo`, `flow` - преобразования
- `compose`, `curry` - функциональные комбинаторы
- `uuid`, `hash`, `measure` - утилиты

### File I/O
- `readFile`, `writeFile`, `appendFile`
- `fileExists`, `deleteFile`

---

## 🤝 Вклад

См. [CONTRIBUTING.md](CONTRIBUTING.md)

---

## 📝 Changelog

### v1.1.0 (Release - Unique Features!)

### v1.0.0 (Release)

### v0.8.0

### v0.6.2

Подробно в [CHANGELOG](CHANGELOG.md)

---

## 📄 Лицензия

MIT License - см. [LICENSE](LICENSE)

---

## 👨‍💻 Автор

Разработан командой **NLP-Core-Team**

---

## ⭐ Звёздочка!

Если проект вам нравится, поставьте звёздочку ⭐ на GitHub!

### Уникальные фичи v1.1.0 (не встречаются больше нигде!)

⚡ **Time Travel Debugging**: `timeTravel("record")`, `timeTravel("rewind")`  
⚡ **Quantum Superposition**: `quantum(a, b, c)`, `observe(state)`  
⚡ **Pattern Generation**: `generate("item-{n}", 10)`  
⚡ **Mirror Transform**: `mirror(array)` - создаёт симметричную копию  
⚡ **Flow & Trace**: `flow()`, `trace()`, `debug()` для отладки  
⚡ **Functional Combinators**: `compose()`, `curry()`, `transform()`  
⚡ **Array Operations**: `chunk()`, `flatten()`, `zip()`, `unzip()`  
⚡ **Collection Utils**: `groupBy()`, `countBy()`, `unique()`, `intersection()`  
⚡ **String Tools**: `split()`, `trim()`, `padStart/End()`, `startsWith/Ends()`  
⚡ **Functional**: `reduce()`, `scan()`, `foldr()`, `iterate()`, `until()`  
⚡ **Selectors**: `first()`, `last()`, `rest()`, `init()`, `take()`, `drop()`  
⚡ **Predicates**: `all()`, `any()`, `none()`, `contains()`  
⚡ **Utilities**: `uuid()`, `hash()`, `measure()`, `sample()`, `shuffle()`

### Уникальные фичи v1.0.0

```smirnov
// === УНИКАЛЬНЫЕ ФИЧИ ===

// 1. Scoped переменные (исчезают после блока)
let! temp = readFile("data.txt");
// temp исчезла здесь!

// 2. SQL-подобные запросы
let adults = select * from users where age > 18 order by name;
let names = select name from users where city == "NYC";

// 3. Живые контракты с авто-исправлением
fn process(x) -> f64
    require x > 0, "x must be positive"
    ensure result >= 0, "result cannot be negative", fix: 0
{
    return x * 2;
}

// 4. Pipeline Assignment (присваивание через pipe)
data |> filter(x => x > 0) |> into positiveData;
numbers |> map(x => x * 2) |> into doubled;

// 5. Auto-Memoization (автоматическое кэширование)
memo fn fib(n) {
    if n <= 1 { return n; }
    return fib(n - 1) + fib(n - 2);
}

// 6. Встроенное тестирование
assert(add(2, 3) == 5, "add should work");
assert(isEven(4) == true, "isEven should return true");

// 7. Data Versioning - отслеживание изменений
commit(myData, "v1");           // сохранить версию
let restored = rollback("v1");  // восстановить
history("v1");                  // показать историю
diff(oldVersion, newVersion);   // сравнить версии

// 8. Auto-doc - автодокументация
doc("myFunction");  // сгенерировать docs

// 9. Параллельные вычисления
parallel for i in 0..1000 {
    process(i);
}
```

## Сборка

### Linux/macOS

```bash
cd SmirnovLang
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Windows (PowerShell)

```powershell
cd SmirnovLang
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

## Запуск

```bash
# Hello World
./build/smirnovLang examples/hello.smirnov

# Более сложный пример
./build/smirnovLang examples/innovative.smirnov

# REPL режим
./build/smirnovLang --repl
```

## Тесты

```bash
cd build
ctest --output-on-failure
```

## Статус

Язык в активной разработке.

## Changelog

### v1.1.0 (Release - Unique Features!)
- **100+ new built-in functions** - most comprehensive DSL runtime
- Time travel debugging (`timeTravel`)
- Quantum superposition (`quantum`, `observe`)
- Pattern generation (`generate`)
- Mirror, echo, flow transformations
- Array operations: chunk, flatten, zip, unzip
- Collection utilities: groupBy, countBy, unique, intersection, difference, union
- Functional: reduce, scan, foldr, iterate, until, compose, curry
- String tools: split, trim, padStart/End, startsWith/Ends, replace
- Selectors: first, last, rest, init, take, drop, takeWhile, dropWhile
- Predicates: all, any, none, contains
- Utilities: uuid, hash, measure, sample, shuffle
- Performance: sortBy, shuffle, chunk, flatten
- Full string manipulation suite (30+ functions)

### v1.0.0 (Release)
- Синхронизирована версия (была 0.6.2 в коде)
- Добавлены функции File I/O: readFile, writeFile, appendFile, fileExists, deleteFile
- Расширены тесты (40+ тестов против 10)
- Улучшена документация
- Объединены встроенные функции в единую реализацию

### v0.8.0
- SQL-подобные запросы (SELECT, WHERE, ORDER BY, LIMIT, OFFSET)
- Data Versioning (commit, rollback, history, diff)
- Встроенное тестирование (assert)
- Auto-doc генерация

### v0.6.2
- Базовые конструкции языка
- Функции, контракты, memoization
- Pipeline выражения
- Pattern matching
