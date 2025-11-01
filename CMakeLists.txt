cmake_minimum_required(VERSION 3.20)

project(ServerWorkout CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Drogon CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

add_executable(WorkoutServer
    src/main.cpp
    src/Db.cpp
)

target_link_libraries(WorkoutServer PRIVATE
    Drogon::Drogon
    unofficial::sqlite3::sqlite3
)
