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
	int m_workout_id{};
	int m_exercise_id{};
	double m_weight{};
	int m_repititions{};
	int m_aproaches{};
};

bool myBD(sqlite3* db);

bool addWorkout(sqlite3* db, const std::string& name, const std::string& date);
std::vector<Workout> selectWorkouts(sqlite3* db);

bool addExercise(sqlite3* db, const std::string& name);
std::vector<Exercise> selectExercises(sqlite3* db);