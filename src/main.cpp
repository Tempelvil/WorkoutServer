#include <drogon/drogon.h>
#include <sqlite3.h>
#include <mutex>
#include <iostream>
#include "Db.h"

using namespace drogon;

static sqlite3* g_db = nullptr;
static std::mutex g_db_mutex;


void getWorkoutHandler(const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
    std::vector<Workout> list;
    {
        std::lock_guard<std::mutex> lock(g_db_mutex);
        list = selectWorkouts(g_db);
    }

    Json::Value arr(Json::arrayValue);
    for (const auto& w : list)
    {
        Json::Value j;
        j["id"] = w.id;
        j["name"] = w.name;
        j["date"] = w.date;
        arr.append(j);
    }

    auto res = HttpResponse::newHttpJsonResponse(arr);
    callback(res);
}

void addWorkoutHandler(const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
    auto json = req->getJsonObject();
    if (!json || !json->isMember("name") || !json->isMember("date"))
    {
        Json::Value err;
        err["status"] = "error";
        err["message"] = "name and date required";
        auto res = HttpResponse::newHttpJsonResponse(err);
        res->setStatusCode(k400BadRequest);
        callback(res);
        return;
    }

    std::string name = (*json)["name"].asString();
    std::string date = (*json)["date"].asString();

    bool ok;
    {
        std::lock_guard<std::mutex> lk(g_db_mutex);
        ok = addWorkout(g_db, name, date);
    }

    Json::Value result;
    if (!ok)
    {
        result["status"] = "error";
        result["message"] = "db error";
        auto res = HttpResponse::newHttpJsonResponse(result);
        res->setStatusCode(k500InternalServerError);
        callback(res);
        return;
    }

    result["status"] = "ok";
    auto res = HttpResponse::newHttpJsonResponse(result);
    callback(res);
}

void getExercisesHandler(const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {
    std::vector<Exercise> list;
    {
        std::lock_guard<std::mutex> lock(g_db_mutex);
        list = selectExercises(g_db);
    }
    Json::Value arr(Json::arrayValue);
    for (const auto& a : list) {
        Json::Value j;
        j["id"] = a.id;
        j["name"] = a.name;
        arr.append(j);
    }
    auto res = HttpResponse::newHttpJsonResponse(arr);
    callback(res);
}
void addExercisesHandler(const HttpRequestPtr& req,
    std::function<void( const HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json || !json->isMember("name")) {
        Json::Value err;
        err["status"] = "Error";
        err["message"] = "failed name required";
        auto res = HttpResponse::newHttpJsonResponse(err);
        res->setStatusCode(k400BadRequest);
        callback(res);
        return;
    }
    std::string name = (*json)["name"].asString();
    bool ok;
    {
        std::lock_guard<std::mutex>lock(g_db_mutex);
        ok = addExercise(g_db, name);
    }
    Json::Value result;
    result["status"] = ok ? "ok" : "Error";
    auto res = HttpResponse::newHttpJsonResponse(result);
    if (!ok) { res->setStatusCode(k500InternalServerError); }
    callback(res);

}


void listSetsHandler(const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {
    (void)req;
    std::vector<Sets> rows;
    {
        std::lock_guard<std::mutex>lock(g_db_mutex);
        rows = selectSetsJoined(g_db);
    }
    Json::Value arr(Json::arrayValue);
    for (const auto& s : rows) {
        Json::Value j;
        j["id"] = s.id;
        j["workout_id"] = s.workout_id;
        j["exercise_id"] = s.exercise_id;
        j["exercise_name"] = s.exercise_name;
        j["repetitions"] = s.repetitions;
        j["aproaches"] = s.aproaches;
        j["weight"] = s.weight;
        arr.append(j);
    }
    auto res = HttpResponse::newHttpJsonResponse(arr);
    callback(res);
}

void addSetHandler(const HttpRequestPtr& req,
std::function<void (const HttpResponsePtr&) > && callback){
    auto json = req->getJsonObject();
    if (!json || !json->isMember("workout_id") || !json->isMember("exercise_id") || !json->isMember("weight") 
        || !json->isMember("repetitions") || !json->isMember("aproaches"))
    {
        Json::Value err;
        err["status"] = "error";
        err["message"] = "workout_id, exercise_id, weight, repetitions, aproaches required";

        auto res = HttpResponse::newHttpJsonResponse(err);
        res->setStatusCode(k400BadRequest);
        callback(res);
        return;
    }
    int workout_id = (*json)["workout_id"].asInt();
    int exercise_id = (*json)["exercise_id"].asInt();
    double weight = (*json)["weight"].asDouble();
    int repetitions = (*json)["repetitions"].asInt();
    int aproaches = (*json)["aproaches"].asInt();

    bool ok;
    {
        std::lock_guard<std::mutex> ld(g_db_mutex);
        ok = addSet(g_db, workout_id, exercise_id, weight, repetitions, aproaches);
    }

    Json::Value ans;
    if (!ok) {
        ans["status"] = "error";
        ans["message"] = "db error";
        auto res = HttpResponse::newHttpJsonResponse(ans);
        res->setStatusCode(k500InternalServerError);
        callback(res);
        return;
    }
    ans["status"] = "ok";
    auto res = HttpResponse::newHttpJsonResponse(ans);
    callback(res);
}

void healthHandler(const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
    auto resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_TEXT_PLAIN);
    resp->setBody("ok");
    callback(resp);
}

int main()
{
    sqlite3* db = nullptr;
    int rc = sqlite3_open("WorkOuts.db", &db);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Cannot open DB: " << (db ? sqlite3_errmsg(db) : "unknown") << "\n";
        if (db) sqlite3_close(db);
        return 1;
    }
    g_db = db;
    if (!myBD(db))
    {
        std::cerr << "Failed to initialize DB schema\n";
        sqlite3_close(db);
        return 1;
    }

    // Статика (папка для index.html и т.д.)
    app().setDocumentRoot("D:/project/WorkoutServer/public");

    // Роуты
    // Workouts
    app().registerHandler("/api/workouts", &getWorkoutHandler, { Get });
    app().registerHandler("/api/workouts", &addWorkoutHandler, { Post });

    // Exercises
    app().registerHandler("/api/exercises", &getExercisesHandler, { Get });
    app().registerHandler("/api/exercises", &addExercisesHandler, { Post });

    app().registerHandler("/api/sets", &listSetsHandler, { Get });
    app().registerHandler("/api/sets", &addSetHandler, {Post});

    app().registerHandler("/health", &healthHandler, { Get });

    // Запуск
    app().addListener("0.0.0.0", 8080);
    std::cout << "Starting server on 0.0.0.0:8080\n";
    app().run();

    if (g_db)
    {
        sqlite3_close(g_db);
        g_db = nullptr;
    }

    return 0;
}
