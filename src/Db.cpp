#include "Db.h"
#include <iostream>

bool myBD(sqlite3* db) {
    const std::string sql = "PRAGMA foreign_keys = ON; "
        "CREATE TABLE IF NOT EXISTS workouts ("
        " id   INTEGER PRIMARY KEY AUTOINCREMENT,"
        " name TEXT NOT NULL,"
        " date TEXT NOT NULL);"

        "CREATE TABLE IF NOT EXISTS exercises ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL);"

        "CREATE TABLE IF NOT EXISTS sets ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "workout_id INTEGER NOT NULL,"
        "exercise_id INTEGER NOT NULL,"
        "weight REAL,"
        "repetitions INTEGER NOT NULL,"
        "aproaches INTEGER NOT NULL,"
        "FOREIGN KEY(workout_id) REFERENCES workouts(id) ON DELETE CASCADE,"
        "FOREIGN KEY(exercise_id) REFERENCES exercises(id) ON DELETE CASCADE);";

    char* errMsg = nullptr;
    int exit = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (exit != SQLITE_OK) {
        std::cerr << "Error " << (errMsg ? errMsg : "unkwonw") << '\n';
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool addWorkout(sqlite3* db,const std::string& name, const std::string& date) {
    const char* sql = "INSERT INTO workouts (name,date) VALUES (?1,?2);";
    sqlite3_stmt* stm = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stm, nullptr) != SQLITE_OK) {
        std::cerr << " Preapare addWorkout falied: " << sqlite3_errmsg(db) << '\n';
        return false;
    }
    sqlite3_bind_text(stm, 1, name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stm, 2, date.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = (sqlite3_step(stm) == SQLITE_DONE);
    if (!ok) std::cerr << "addWorkout step error: " << sqlite3_errmsg(db) << "\n";
    sqlite3_finalize(stm);
    return ok;
}

std::vector<Workout> selectWorkouts(sqlite3* db) {
    std::vector<Workout> out;
    const std::string sql = "SELECT id, name, date FROM workouts ORDER BY date,id;";
    sqlite3_stmt* stm = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stm, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare Select Error: " << sqlite3_errmsg(db) << '\n';
        sqlite3_finalize(stm);
        return out;
    }
    while (sqlite3_step(stm) == SQLITE_ROW) {
        Workout w;

        w.id = sqlite3_column_int(stm, 0);
        const unsigned char* n = sqlite3_column_text(stm, 1);// для примера как защитится от вывода nullptr(программа сломается если его попробовать вывести*)
        const unsigned char* d = sqlite3_column_text(stm, 2); // В таблице у нас нет возможности записать пустое выражение, стоит NOT NULL, как во всех примерах
        w.name = n ? reinterpret_cast<const char*>(n) : "";
        w.date = d ? reinterpret_cast<const char*>(d) : "";
        out.push_back(std::move(w));
    }
    
    sqlite3_finalize(stm);
    return out;
}

bool addExercise(sqlite3* db, const std::string& name)
{
    const char* sql = "INSERT INTO exercises (name) VALUES (?1);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "Prepare addExercise failed: " << sqlite3_errmsg(db) << '\n';
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!ok)
        std::cerr << "SQLite insert exercise failed: " << sqlite3_errmsg(db) << '\n';

    sqlite3_finalize(stmt);
    return ok;
}

std::vector<Exercise> selectExercises(sqlite3* db)
{
    std::vector<Exercise> list;
    const std::string sql = "SELECT id, name FROM exercises ORDER BY id;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "Prepare selectExercises failed: " << sqlite3_errmsg(db) << '\n';
        return list;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Exercise e;
        e.id = sqlite3_column_int(stmt, 0);
        const unsigned char* n = sqlite3_column_text(stmt, 1);
        e.name = n ? reinterpret_cast<const char*>(n) : "";
        list.push_back(std::move(e));
    }

    sqlite3_finalize(stmt);
    return list;
}

bool addSet(sqlite3* db, int workout_id, int exercise_id, double weight, int repetitions, int aproaches) {
    const char* sql = R"sql(INSERT INTO sets (workout_id,exercise_id,weight,repetitions,aproaches)
    VALUES (?1,?2,?3,?4,?5);)sql";
    sqlite3_stmt* stm = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stm, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare addSets failed: " << sqlite3_errmsg(db) << '\n';
        return false;
    }
    sqlite3_bind_int(stm, 1, workout_id);
    sqlite3_bind_int(stm, 2, exercise_id);
    sqlite3_bind_double(stm, 3, weight);
    sqlite3_bind_int(stm, 4, repetitions);
    sqlite3_bind_int(stm, 5, aproaches);

    bool ok = (sqlite3_step(stm) == SQLITE_DONE);
    if(!ok){
        std::cerr << "insertSet step error: " << sqlite3_errmsg(db) << "\n";
    }
    sqlite3_finalize(stm);
    return ok;
}

std::vector<Sets> selectSetsJoined(sqlite3* db)
{
    std::vector<Sets> out;
    const char* sql = R"sql( SELECT s.id, s.workout_id, s.exercise_id, ex.name, s.repetitions, s.aproaches, s.weight
    FROM sets AS s
    JOIN workouts  AS w  ON w.id  = s.workout_id
    JOIN exercises AS ex ON ex.id = s.exercise_id
    ORDER BY s.id;)sql";

    sqlite3_stmt* stm = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stm, nullptr) != SQLITE_OK)
    {
        std::cerr << "Prepare selectSetsJoined failed: " << sqlite3_errmsg(db) << "\n";
        sqlite3_finalize(stm);
        return out;
    }

    while (sqlite3_step(stm) == SQLITE_ROW) {
        Sets s;
        s.id = sqlite3_column_int(stm, 0);
        s.workout_id = sqlite3_column_int(stm, 1);
        s.exercise_id = sqlite3_column_int(stm, 2);
        const unsigned char* exName = sqlite3_column_text(stm, 3);
        s.exercise_name = exName ? reinterpret_cast<const char*>(exName) : "";
        s.repetitions = sqlite3_column_int(stm, 4);
        s.aproaches = sqlite3_column_int(stm, 5);
        s.weight = sqlite3_column_double(stm, 6);

        out.push_back(std::move(s));
    }

    sqlite3_finalize(stm);
    return out;
}