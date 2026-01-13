# WorkoutServer

WorkoutServer — серверный проект на C++, разработанный в рамках самостоятельного изучения backend-разработки.
Проект представляет собой серверное приложение для работы с данными о тренировках, сетевым взаимодействием и базой данных.
- обработка HTTP-запросов
- серверная логика на C++
- хранение данных о тренировках в базе SQLite
- базовая архитектура серверного приложения

Использует такие технологие как:
- C++
- CMake
- SQLite
- STL
- DRAGON

Сборка и запуск:
```bash
git clone https://github.com/Tempelvil/WorkoutServer.git
cd WorkoutServer
mkdir build
cd build
cmake ..
cmake --build .
## Структура проекта
WorkoutServer/
├── src/ # исходный код серверной логики
├── public/ # публичные ресурсы
├── vcpkg/ # зависимости (vcpkg)
├── CMakeLists.txt # конфигурация сборки
├── vcpkg.json # описание зависимостей
├── build.bat # скрипт сборки под Windows
