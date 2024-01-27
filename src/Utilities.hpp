#pragma once
#ifndef UTILITIES
#define UTILITIES

#include <cstddef>
#include <random>
#include <stdexcept>
#include <vector>

/**
 * @brief Enum representing cardinal directions (North, East, South, West).
 */
enum class Direction { N = 0, E = 1, S = 2, W = 3, None = 4 };

/*
std::ostream &operator<<(std::ostream &os, const Direction &dir) {
  switch (dir) {
  case Direction::N:
    os << "N";
    break;
  case Direction::E:
    os << "E";
    break;
  case Direction::S:
    os << "S";
    break;
  case Direction::W:
    os << "W";
    break;
  }
  return os;
}

*/

/**
 * @brief Enum representing different types of buildings.
 */
enum class BuildingType {
  Commercial = 0,
  Industrial = 1,
  Residential = 2,
  None = 3,
  Gym = 5,
  Restaurant = 6,
  Store = 7
};

/**
 * @brief Struct representing statistics for an intersection.
 */
struct IntersectionStats {
  std::vector<double> carCounters;
  int overallCounter;

  IntersectionStats() : carCounters(24, 0) {}

  /**
   * @brief Increments the counter for a specific hour.
   * @param index Hour index.
   */
  void incrementCounter(int index) {
    if (index >= 0 && index < carCounters.size()) {
      carCounters[index]++;
    } else {
      throw std::out_of_range("Index out of bounds");
    }
  }

  /**
   * @brief Retrieves the total cars for each hour.
   * @return A vector of double representing the total cars for each hour.
   */
  std::vector<double> getTotalCars() const { return carCounters; }
};

/**
 * @brief Struct representing stats for roads.
 */
struct RoadStats {
  std::vector<int> carCounters;
  int overallCounter;

  RoadStats() : carCounters(11, 0), overallCounter(0) {}

  /**
   * @brief Increments the counter for a specific hour and updates the overall
   * counter.
   * @param index Hour index.
   */
  void incrementCounter(int index) {
    if (index >= 0 && index < carCounters.size()) {
      carCounters[index]++;
      overallCounter++;
    } else {
      throw std::out_of_range("Index out of bounds");
    }
  }

  /**
   * @brief Calculates and retrieves the average hour for cars.
   * @return The average hour for cars as a double.
   */
  double getAverage() const {
    double sum = 0.0;
    for (size_t i = 0; i < carCounters.size(); i++) {
      if (overallCounter > 0) {
        sum += static_cast<double>(carCounters[i]) / overallCounter * i;
      }
    }
    return sum;
  }
};

/**
 * @brief Struct representing statistics for the entire city.
 */
struct CityStats {
  std::vector<int> counters;
  std::vector<double> sums;

  CityStats() : counters(24, 0), sums(24, 0) {}

  /**
   * @brief Increments the counter and sum for a specific hour.
   * @param index Hour index.
   * @param percentage Percentage value to be added to the sum.
   */
  void incrementCounter(int index, double percentage) {
    if (index >= 0 && index < 24) {
      counters[index]++;
      sums[index] += percentage;

    } else {
      throw std::out_of_range("Index out of bounds");
    }
  }
  /**
   * @brief Calculates and retrieves the hourly averages for percentages.
   * @return A vector of double representing the hourly averages for
   * percentages.
   */
  std::vector<double> getHourlyAverages() const {
    std::vector<double> averages;
    for (size_t i = 0; i < 24; i++) {
      if (counters[i] > 0) {
        averages.push_back(sums[i] / counters[i]);
      } else {
        averages.push_back(0);
      }
    }
    return averages;
  }
};

/**
 * @brief Struct representing a position with x and y coordinates.
 */
struct Position {
  Position(size_t xa, size_t ya) : x(xa), y(ya) {}
  Position() : x(0), y(0) {}
  size_t x;
  size_t y;

  Position &operator=(const Position &other) {
    if (this != &other) {
      x = other.x;
      y = other.y;
    }
    return *this;
  }

  bool operator==(const Position &a) const { return a.x == x && a.y == y; }
  bool operator!=(const Position &a) const { return a.x != x || a.y != y; }
};

#endif
