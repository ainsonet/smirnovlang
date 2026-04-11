# 🌐 SmirnovLang Website

Официальный сайт языка программирования SmirnovLang.

## 🚀 Запуск

### Локально

1. **Используйте Live Server** (рекомендуется):
   - Откройте папку `website/` в VS Code
   - Нажмите правой кнопкой на `index.html`
   - Выберите "Open with Live Server"

2. **Или откройте напрямую**:
   ```bash
   # Windows
   start website/index.html

   # macOS
   open website/index.html

   # Linux
   xdg-open website/index.html
   ```

## 📁 Структура

```
website/
├── index.html          # Главная страница
├── docs/
│   ├── index.html      # Документация
│   ├── css/
│   │   └── docs.css
│   └── js/
│       └── docs.js
├── css/
│   └── style.css       # Основные стили
└── js/
    └── main.js         # Интерактив
```

## 🎨 Особенности

- ✅ **Тёмная тема** — приятная для глаз
- ✅ **Адаптивный дизайн** — работает на мобильных
- ✅ **Подсветка синтаксиса** — примеры кода
- ✅ **Интерактивные вкладки** — демонстрация фич
- ✅ **Копирование кода** — кнопка в примерах
- ✅ **Плавная навигация** — smooth scroll
- ✅ **Анимации** — приятные переходы

## 🛠 Технологии

- Чистый HTML5
- CSS3 с CSS Variables
- Vanilla JavaScript
- Никаких фреймворков!

## 🌐 Развёртывание

### GitHub Pages

1. Зайдите в настройки репозитория
2. Раздел **Pages**
3. Source: **gh-pages branch** или **root folder**
4. Сайт будет доступен по адресу:
   ```
   https://ainsonet.github.io/smirnovlang/
   ```

### Альтернативно - отдельный репозиторий

```bash
git checkout --orphan gh-pages
git rm -rf .
# Копируйте файлы сайта сюда
git add .
git commit -m "Initial website build"
git push origin gh-pages
```

## 🎯 Что на странице

### Главная страница
- Герой-секция с примером кода
- Уникальные возможности (6 карточек)
- Примеры кода с вкладками
- Секция скачивания
- Футер

### Документация
- Введение и установка
- Синтаксис языка
- Уникальные фичи
- API reference
- Боковая навигация

## 🎨 Кастомизация

### Цветовая схема

Измените в `css/style.css`:

```css
:root {
    --bg-primary: #0d1117;       /* Основной фон */
    --accent-primary: #58a6ff;   /* Акцентный цвет */
    --gradient-start: #793af5;   /* Градиент */
    --gradient-end: #00d4aa;
}
```

### Шрифты

Подключите в `index.html`:

```html
<link href="https://fonts.googleapis.com/css2?family=Fira+Code&display=swap" rel="stylesheet">
```

## 🤝 Вклад

Принимаются правки и улучшения! Создавайте pull request.

## 📄 Лицензия

MIT License
