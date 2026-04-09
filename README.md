# SmirnovLang v0.6.0

<img src="https://raw.githubusercontent.com/ainsonet/smirnovlang/master/logo.png" alt="SmirnovLang Logo" width="200"/>

Современный язык программирования общего назначения с инновационными возможностями.

## Статус: **Рабочий прототип** ✅

Ядро языка реализовано и работает! Базовые конструкции, функции, pipeline-выражения, контракты, scoped variables — всё уже функционирует.

### Что работает в 0.6.0

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
✅ SQL-like запросы (базовые)  
✅ Встроенные функции: `println`, `len`, `map`, `filter`, `sum`, `sqrt`, `min`, `max`, `range` и 20+ других

### Синтаксис

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
