# SmirnovLang

<img src="https://raw.githubusercontent.com/ainsonet/smirnovlang/master/logo.png" alt="SmirnovLang Logo" width="200"/>

Современный язык программирования общего назначения с инновационными возможностями.

## Концепция

SmirnovLang объединяет лучшие идеи из существующих языков и добавляет собственные инновации:

### Ключевые фичи

1. **Fluid Pipes (Жидкие трубы)** — конвейерный оператор `|>` работает везде
2. **Smart Null Safety** — автоматический unwrap в if-контексте, опциональные типы `?Type`
3. **Pattern Matching everywhere** — деструктуризация в любом присваивании
4. **Built-in Contracts** — `require`/`ensure` как часть синтаксиса функций
5. **Concurrent loops** — ключевое слово `parallel` для автоматического распараллеливания
6. **Unified Value Type** — `Value` может быть чем угодно, с статической типизацией сверху

### Синтаксис

```smirnov
// Жидкие трубы
let result = [1, 2, 3, 4, 5]
    |> filter(x => x > 2)
    |> map(x => x * 2)
    |> sum();

// Pattern matching
let point = (10, 20);
let (x, y) = point;

// Контракты
fn divide(a, b) -> f64
    require b != 0, "Division by zero"
    ensure result => result * b == a
{
    return a / b;
}

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
