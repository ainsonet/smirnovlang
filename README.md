# SmirnovLang v1.1.0

<img src="https://raw.githubusercontent.com/ainsonet/smirnovlang/master/logo.png" alt="SmirnovLang Logo" width="200"/>

Современный язык программирования общего назначения с инновационными возможностями.

## Статус: **Релиз v1.1.0** ✅

Ядро языка полностью реализовано! Это один из самых функциональных предметно-ориентированных языков с уникальными возможностями.

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

// === УНИКАЛЬНЫЕ ФИЧИ v1.0.0 ===

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

// === ДРУГИЕ ФИЧИ ===

// Жидкие трубы
let result = [1, 2, 3, 4, 5]
    |> filter(x => x > 2)
    |> map(x => x * 2)
    |> sum();

// Pattern matching
let point = (10, 20);
let (x, y) = point;

// Параллельные вычисления
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
