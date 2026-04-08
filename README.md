# SmirnovLang v0.2.0

<img src="https://raw.githubusercontent.com/ainsonet/smirnovlang/master/logo.png" alt="SmirnovLang Logo" width="200"/>

Современный язык программирования общего назначения с инновационными возможностями.

## Концепция

SmirnovLang объединяет лучшие идеи из существующих языков и добавляет собственные **уникальные** инновации:

### Уникальные фичи (чего нет в других языках)

1. **Scoped Variables (исчезающие переменные)** — `let! name = value` автоматически удаляется после выхода из блока
2. **SQL-like Queries** — запросы к коллекциям: `select * from users where age > 18 order by name`
3. **Live Contracts (живые контракты)** — контракты с авто-исправлением: `ensure x > 0, fix: x = 1`
4. **Pipeline Assignment** — присваивание через pipe: `data |> filter() |> into result`

### Дополнительные фичи

4. **Fluid Pipes (Жидкие трубы)** — конвейерный оператор `|>` работает везде
5. **Smart Null Safety** — автоматический unwrap в if-контексте, опциональные типы `?Type`
6. **Pattern Matching everywhere** — деструктуризация в любом присваивании
7. **Built-in Contracts** — `require`/`ensure` как часть синтаксиса функций
8. **Concurrent loops** — ключевое слово `parallel` для автоматического распараллеливания
9. **Unified Value Type** — `Value` может быть чем угодно, с статической типизацией сверху

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

```bash
mkdir build && cd build
cmake ..
make
```

## Запуск

```bash
./smirnovLang examples/hello.smirnov
```

## Статус

Язык в активной разработке.
