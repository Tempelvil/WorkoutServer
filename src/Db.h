#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>

struct Workout {
	int id{};
	std::string name{};
	std::string date{};

};
struct Exercise{
	int id{};
	std::string  name{};
};

struct Sets{
	int id{};
	int workout_id{};
	int exercise_id{};
	double weight{};
	int repetitions{};
	int aproaches{};
	std::string exercise_name{};
};

bool myBD(sqlite3* db);

bool addWorkout(sqlite3* db, const std::string& name, const std::string& date);
std::vector<Workout> selectWorkouts(sqlite3* db);

bool addExercise(sqlite3* db, const std::string& name);
std::vector<Exercise> selectExercises(sqlite3* db);

bool addSet(sqlite3* db, int workout_id, int exercise_id, double weight, int repetitions, int aproaches);
std::vector<Sets> selectSetsJoined(sqlite3* db);