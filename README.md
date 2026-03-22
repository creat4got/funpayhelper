# FunPay Helper

<p align="center">
  <strong>🇬🇧 English</strong> | <a href="#russian">🇷🇺 Русский</a>
</p>

---

**FunPay Helper** is a desktop GUI bot for the [FunPay](https://funpay.com) trading platform, written in C++ with Qt 6. It runs locally on Linux with no need for Telegram or any third-party services.

---

## Features

- **Auto-Responder** — automatically replies to buyers' messages based on configurable rules
- **Auto-Delivery** — sends product text to the buyer upon receiving a new order
- **Auto-Raise** — automatically raises your listings at a configurable interval
- **Built-in Chat** — view conversations and send messages directly from the app
- **Event Log** — color-coded log with INF / OK / WRN / ERR levels
- **Lot Manager** — displays all your sections and lots with prices

---

## Screenshots

> *(add screenshots here)*

---

## Requirements

| Dependency | Version | Notes |
|------------|---------|-------|
| Qt | 6.5+ | Modules: Widgets |
| Boost | 1.80+ | Header-only (Beast, Asio) |
| OpenSSL | 1.1+ | System package |
| CMake | 3.16+ | |
| GCC / Clang | C++17 | |

### Install dependencies (Ubuntu / Debian)

```bash
sudo apt update
sudo apt install -y build-essential cmake libssl-dev libboost-dev
```

**Qt 6** is installed via the [Qt Online Installer](https://www.qt.io/download-qt-installer).
Select the component **Qt 6.x → Desktop gcc 64-bit**.

---

## Build

```bash
git clone https://github.com/creat4got/funpayhelper.git
cd funpayhelper

mkdir build && cd build

cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64 \
  -DBOOST_ROOT=/path/to/boost

make -j$(nproc)
```

**Run:**

```bash
LD_LIBRARY_PATH=/path/to/Qt/6.x.x/gcc_64/lib ./botfunpay
```

> Replace `/path/to/Qt/6.x.x/gcc_64` and `/path/to/boost` with the actual paths on your system.

---

## Getting Started

### 1. Export your cookie file

The bot needs your FunPay account cookies to authenticate. Use a browser extension to export them:

- [Cookie-Editor](https://cookie-editor.com/) *(recommended)*
- EditThisCookie

**Steps:**
1. Log in to your account at [funpay.com](https://funpay.com)
2. Open the extension and click **Export → Export as JSON**
3. Save the file (e.g. `cookies.json`)

Both export formats are supported:

```json
[{"name": "golden_key", "value": "...", "domain": ".funpay.com", ...}]
```
```json
{"cookies": [{"name": "golden_key", "value": "...", ...}]}
```

> **Important:** the key cookies are `golden_key` (long-term auth token) and `PHPSESSID`. Without them, login will fail.

### 2. Log in to the app

1. Launch **FunPay Helper**
2. Click **"Choose file"** and select your `cookies.json`
3. Click **"Login"** — the app will validate the cookies and open the main window

---

## Interface

### Toolbar

Start / Stop the bot and view the current account status.

---

### Tab: Log

Displays all events in real time with color coding:

| Color | Level | Description |
|-------|-------|-------------|
| White | INF | Informational message |
| Green | OK | Successful operation |
| Orange | WRN | Warning (e.g. raise cooldown) |
| Red | ERR | Error |

---

### Tab: Lots

Shows all your sections and lots as a tree with columns:

| Column | Content |
|--------|---------|
| Section / Description | Section name or lot description |
| Price | Lot price |
| Lot ID | Listing identifier |
| Node ID | Section ID |
| Game ID | Game identifier (used for raising) |

**Buttons:**
- **"Refresh"** — fetch the current lot list from FunPay
- **"Raise Now"** — immediately raise all sections

---

### Tab: Auto-Responder

Configure automatic replies to buyers' messages.

**Rule fields:**

| Field | Description |
|-------|-------------|
| Trigger | Text that activates the rule |
| Response | Text that will be sent to the buyer |
| Enabled | Whether the rule is active |
| Exact Match | Match full message or just substring |

**Placeholder:** `%username%` in the response text is replaced with the buyer's username.

**Example:**
- Trigger: `hello`
- Response: `Hi, %username%! How can I help you?`

---

### Tab: Auto-Delivery

Automatically sends product text to the buyer when a new order is received.

**Fields:**

| Field | Description |
|-------|-------------|
| Lot ID | ID of the lot this delivery is linked to |
| Title | Lot name (for reference) |
| Delivery Text | Text the buyer will receive |
| Enabled | Whether this entry is active |

The Lot ID can be found in the **Lots** tab.

---

### Tab: Settings

| Setting | Description |
|---------|-------------|
| Auto-Raise | Enable / disable + interval in minutes |
| Auto-Responder | Enable / disable the module |
| Auto-Delivery | Enable / disable the module |

---

### Tab: Chats

Built-in messenger for communicating with buyers:

- **Contact list** — all chats with last message preview
- **Message history** — full conversation with the selected contact
- **Send messages** — input field + Send button
- **"Refresh"** — reload the chat list

FunPay system messages are displayed in italics.

---

## Architecture

```
┌─────────────────────────────────────────────┐
│               DashboardWindow               │  ← Qt GUI (main window)
│   Log │ Lots │ Responder │ Delivery │ Chats │
└────────────────────┬────────────────────────┘
                     │ Qt signals
              ┌──────▼──────┐
              │  BotEngine  │  ← thread manager
              └──────┬──────┘
                     │ QThread
              ┌──────▼──────┐
              │  BotWorker  │  ← bot logic (polling, auto-actions)
              └──────┬──────┘
                     │
              ┌──────▼──────┐
              │  FunPayAPI  │  ← FunPay API wrapper
              └──────┬──────┘
                     │
              ┌──────▼──────┐
              │  HttpClient │  ← Boost.Beast HTTPS client
              └─────────────┘
```

| Component | File | Purpose |
|-----------|------|---------|
| `HttpClient` | `httpclient.h/cpp` | Low-level HTTPS client using Boost.Beast, no Qt |
| `FunPayAPI` | `funpayapi.h/cpp` | Auth, polling, messaging, lot raising |
| `BotWorker` | `botworker.h/cpp` | Bot business logic in a separate thread |
| `BotEngine` | `botengine.h/cpp` | `QThread` management, signal bridge to UI |
| `DashboardWindow` | `dashboardwindow.h/cpp` | Main window, fully programmatic UI |
| `CookieLoader` | `cookieloader.h/cpp` | JSON cookie file parser |
| `SettingsManager` | `settingsmanager.h/cpp` | Persistent settings via `QSettings` |
| `Logger` | `logger.h/cpp` | Singleton logger with `newLogEntry` signal |

---

## Tech Stack

- **Language:** C++17
- **UI Framework:** Qt 6 (Widgets)
- **HTTP:** Boost.Beast + Boost.Asio over OpenSSL (native HTTPS, no Qt Network)
- **Build System:** CMake
- **Platform:** Linux (x86\_64)

---

## Roadmap

- [ ] Detailed order info for auto-delivery
- [ ] Lot caching (avoid re-fetching on every auto-raise cycle)
- [ ] Add / edit lot dialog
- [ ] Desktop notifications
- [ ] Dark theme
- [ ] Multi-account support

---

## License

MIT License — see [LICENSE](LICENSE).

---

## Contributing

Found a bug or want to suggest a feature? Open an [Issue](https://github.com/creat4got/funpayhelper/issues).

---

---

<a name="russian"></a>

# FunPay Helper

<p align="center">
  <a href="#top">🇬🇧 English</a> | <strong>🇷🇺 Русский</strong>
</p>

---

**FunPay Helper** — десктопный GUI-бот для торговой площадки [FunPay](https://funpay.com), написанный на C++ с использованием Qt 6. Работает локально под Linux без необходимости в Telegram или сторонних сервисах.

---

## Возможности

- **Авто-ответчик** — автоматически отвечает на сообщения покупателей по заданным правилам
- **Авто-выдача** — отправляет товар (текст) покупателю при получении нового заказа
- **Авто-поднятие лотов** — автоматически поднимает ваши объявления с заданным интервалом
- **Встроенный чат** — просмотр переписки и отправка сообщений прямо из приложения
- **Журнал событий** — цветной лог с уровнями INF / OK / WRN / ERR
- **Управление лотами** — отображение всех ваших секций и лотов с ценами

---

## Скриншоты

> *(добавьте скриншоты приложения)*

---

## Требования

| Зависимость | Версия | Примечание |
|-------------|--------|------------|
| Qt | 6.5+ | Модули: Widgets |
| Boost | 1.80+ | Только заголовки (Beast, Asio) |
| OpenSSL | 1.1+ | Системный пакет |
| CMake | 3.16+ | |
| GCC / Clang | C++17 | |

### Установка зависимостей (Ubuntu / Debian)

```bash
sudo apt update
sudo apt install -y build-essential cmake libssl-dev libboost-dev
```

**Qt 6** устанавливается через [Qt Online Installer](https://www.qt.io/download-qt-installer).
Выберите компонент **Qt 6.x → Desktop gcc 64-bit**.

---

## Сборка

```bash
git clone https://github.com/creat4got/funpayhelper.git
cd funpayhelper

mkdir build && cd build

cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64 \
  -DBOOST_ROOT=/path/to/boost

make -j$(nproc)
```

**Запуск:**

```bash
LD_LIBRARY_PATH=/path/to/Qt/6.x.x/gcc_64/lib ./botfunpay
```

> Замените `/path/to/Qt/6.x.x/gcc_64` и `/path/to/boost` на реальные пути в вашей системе.

---

## Первый запуск

### 1. Получение cookie-файла

Для работы бота нужны cookies вашего аккаунта FunPay. Используйте браузерное расширение для экспорта:

- [Cookie-Editor](https://cookie-editor.com/) *(рекомендуется)*
- EditThisCookie

**Шаги:**
1. Войдите в аккаунт на [funpay.com](https://funpay.com)
2. Откройте расширение и нажмите **Export → Export as JSON**
3. Сохраните файл (например, `cookies.json`)

Поддерживаются оба формата экспорта:

```json
[{"name": "golden_key", "value": "...", "domain": ".funpay.com", ...}]
```
```json
{"cookies": [{"name": "golden_key", "value": "...", ...}]}
```

> **Важно:** ключевые cookies — `golden_key` (долгосрочный токен авторизации) и `PHPSESSID`. Без них авторизация невозможна.

### 2. Вход в приложение

1. Запустите **FunPay Helper**
2. Нажмите **«Выбрать файл»** и укажите сохранённый `cookies.json`
3. Нажмите **«Войти»** — приложение проверит cookies и откроет главное окно

---

## Интерфейс

### Панель управления

Кнопки **«Запустить»** / **«Остановить»** бота и статус текущего аккаунта.

---

### Вкладка «Журнал»

Отображает все события в реальном времени с цветовой индикацией:

| Цвет | Уровень | Описание |
|------|---------|----------|
| Белый | INF | Информационное сообщение |
| Зелёный | OK | Успешная операция |
| Оранжевый | WRN | Предупреждение (например, кулдаун поднятия) |
| Красный | ERR | Ошибка |

---

### Вкладка «Лоты»

Показывает все ваши секции и лоты в виде дерева с колонками:

| Колонка | Содержимое |
|---------|------------|
| Секция / Описание | Название секции или описание лота |
| Цена | Цена лота |
| Lot ID | Идентификатор объявления |
| Node ID | ID секции |
| Game ID | ID игры (используется для поднятия) |

**Кнопки:**
- **«Обновить»** — загрузить актуальный список лотов с FunPay
- **«Поднять сейчас»** — немедленно поднять все секции

---

### Вкладка «Авто-ответчик»

Настройка автоматических ответов на сообщения покупателей.

**Поля правила:**

| Поле | Описание |
|------|----------|
| Триггер | Текст, при обнаружении которого срабатывает правило |
| Ответ | Текст, который будет отправлен покупателю |
| Включено | Активность правила |
| Точное совпадение | Полное совпадение или вхождение в текст |

**Плейсхолдер:** `%username%` в тексте ответа заменяется на ник покупателя.

**Пример:**
- Триггер: `привет`
- Ответ: `Привет, %username%! Чем могу помочь?`

---

### Вкладка «Авто-выдача»

Автоматическая отправка товара (текста) покупателю при получении нового заказа.

**Поля:**

| Поле | Описание |
|------|----------|
| Lot ID | ID лота, к которому привязана выдача |
| Название | Название лота (для удобства) |
| Текст выдачи | Текст, который получит покупатель |
| Включено | Активность позиции |

Lot ID можно найти во вкладке «Лоты».

---

### Вкладка «Настройки»

| Параметр | Описание |
|----------|----------|
| Авто-поднятие | Включить / выключить + интервал в минутах |
| Авто-ответчик | Включить / выключить модуль |
| Авто-выдача | Включить / выключить модуль |

---

### Вкладка «Чаты»

Встроенный мессенджер для общения с покупателями:

- **Список контактов** — все чаты с предпросмотром последнего сообщения
- **История сообщений** — полная переписка с выбранным контактом
- **Отправка сообщений** — поле ввода + кнопка «Отправить»
- **«Обновить»** — перезагрузить список чатов

Системные сообщения FunPay отображаются курсивом.

---

## Архитектура

```
┌─────────────────────────────────────────────┐
│               DashboardWindow               │  ← Qt GUI (главное окно)
│  Журнал │ Лоты │ Ответчик │ Выдача │ Чаты  │
└────────────────────┬────────────────────────┘
                     │ сигналы Qt
              ┌──────▼──────┐
              │  BotEngine  │  ← управление потоком
              └──────┬──────┘
                     │ QThread
              ┌──────▼──────┐
              │  BotWorker  │  ← логика бота (poll, авто-действия)
              └──────┬──────┘
                     │
              ┌──────▼──────┐
              │  FunPayAPI  │  ← обёртка над FunPay API
              └──────┬──────┘
                     │
              ┌──────▼──────┐
              │  HttpClient │  ← Boost.Beast HTTPS-клиент
              └─────────────┘
```

| Компонент | Файл | Назначение |
|-----------|------|------------|
| `HttpClient` | `httpclient.h/cpp` | Низкоуровневый HTTPS-клиент на Boost.Beast без Qt |
| `FunPayAPI` | `funpayapi.h/cpp` | Инициализация, polling, отправка сообщений, поднятие лотов |
| `BotWorker` | `botworker.h/cpp` | Бизнес-логика бота в отдельном потоке |
| `BotEngine` | `botengine.h/cpp` | Управление `QThread`, мост сигналов между потоком и UI |
| `DashboardWindow` | `dashboardwindow.h/cpp` | Главное окно, весь UI строится программно |
| `CookieLoader` | `cookieloader.h/cpp` | Парсинг JSON-файла с cookies |
| `SettingsManager` | `settingsmanager.h/cpp` | Хранение настроек через `QSettings` |
| `Logger` | `logger.h/cpp` | Singleton-логгер с сигналом `newLogEntry` |

---

## Технический стек

- **Язык:** C++17
- **UI фреймворк:** Qt 6 (Widgets)
- **HTTP:** Boost.Beast + Boost.Asio поверх OpenSSL (нативный HTTPS без Qt Network)
- **Сборка:** CMake
- **Платформа:** Linux (x86\_64)

---

## Планируемые улучшения

- [ ] Детальная информация о заказе для авто-выдачи
- [ ] Кэширование лотов (не загружать каждый раз при авто-поднятии)
- [ ] Диалог добавления/редактирования лота
- [ ] Уведомления на рабочем столе
- [ ] Тёмная тема
- [ ] Поддержка нескольких аккаунтов

---

## Лицензия

MIT License — см. файл [LICENSE](LICENSE).

---

## Участие в разработке

Нашли баг или хотите предложить улучшение? Открывайте [Issue](https://github.com/creat4got/funpayhelper/issues).
