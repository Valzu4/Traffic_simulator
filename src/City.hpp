#pragma once
#ifndef CITY_HPP
#define CITY_HPP

#define TIMEDIVIDER 3600

#include "Buildings/Building.hpp"
#include "Roads/Intersection.hpp"
#include "Roads/Road.hpp"
#include "Utilities.hpp"

#include <stdexcept>
#include <tuple>
#include <vector>

class Square;

/**
 * @brief CityStats class is used to store the city as a grid of squares
 *
 * City is a grid of squares. City also tracs many statistics, e.g. time, and
 * the ammount of total traffic in the city.
 */
class City {

public:
  /**
   * @brief Construct a new City object
   *
   * @param size The city is going to be size x size grid
   */
  City(size_t size);

  /** @brief destroys the city object */
  ~City();

  /**
   * @brief add adds (or replaces) a square in the city
   *
   * @param pos Position where to add the square
   * @param square Pointer to the square to be added
   */
  void add(Position pos, Square *square);

  /**
   * @brief getSquare returns the square in the given position
   *
   * @param pos Position of the square
   * @return Square* Pointer to the square
   */
  Square *getSquare(Position pos);

  /**
   * @brief getCity returns the grid of squares
   *
   * @return std::vector<std::vector<Square*>> The grid of squares
   */
  const std::vector<std::vector<Square *>> &getCity() const { return city_; }

  /**
   * @brief getCitySize returns the size of the city
   *
   * @return size_t size of the city
   */
  size_t getCitySize() const { return city_[0].size(); }

  /**
   * @brief getResidentCount returns the amount of residents in the city
   *
   * @return int amount of residents in the city
   */
  int getResidentCount() const { return residentCount_; }

  /**
   * @brief getIndustrials returns the vector of industrial squares
   *
   * @return std::vector<Square*> vector of industrial squares
   */
  std::vector<Square *> getIndustrials() const;
  /**
   * @brief getCommercials returns the vector of commercial squares
   *
   * @return std::vector<Square*> vector of commercial squares
   */
  std::vector<Square *> getCommercials() const;
  /**
   * @brief getResidentials returns the vector of residential squares
   *
   * @return std::vector<Square*> vector of residential squares
   */
  std::vector<Square *> getResidentials() const;

  /**
   * @brief getRoads returns the vector of roads
   *
   * @return std::vector<Road*> vector of roads
   */
  std::vector<Road *> getRoads() const;

  /** @brief simulates everything in the city */
  void simulate();
  /** @brief initializes everything in the city for simulation */
  void init();

  /**
   * @brief getTime returns the time in the city in 24h format
   *
   * @return double time in the city
   */

  double getTime() const { return ((double)time_ / TIMEDIVIDER); }

  /**
   * @brief Retrieves the current average road traffic percentage from all
   * roads.
   * @return The current road traffic percentage as a double.
   */
  double currentRoadTrafficPercentage() const;

  /**
   * @brief Calculates and returns the average road traffic percentage from all
   * roads from the entire simulation.
   * @return The average road traffic percentage as a double.
   */
  double averageRoadTrafficPercentage() const;

  /**
   * @brief Retrieves a vector of pointers to intersections.
   * @return A vector of Intersection pointers representing the intersections.
   */
  std::vector<Intersection *> getIntersections() const;

  /**
   * @brief Retrieves the amount of residents in the city.
   * @return The number of residents as an integer.
   */
  int getResidentAmount() const;

  /**
   * @brief carsOnRoads returns the amount of cars on the roads
   *
   * @return int amount of cars on the roads
   */
  int carsOnRoads() const;

  /**
   * @brief Retrieves the hourly averages from the statistics and returns them.
   * @return A vector of doubles representing the hourly averages.
   */
  std::vector<double> getStats() const { return stats_.getHourlyAverages(); }

  /**
   * @brief Clears the existing data and resizes the container to the specified
   * size, this is needed for importing different size cities.
   * @param size The new size of the container.
   */
  void clearAndResize(size_t size);

  /**
   * @brief Returns the "hour counters", meaning how many times has a specific
   * hour been simulated.
   * @return A vector of int representing the hour counters.
   */
  std::vector<int> getHourCounters() const { return hourCounters_; }

  /**
   * @brief Exports the statistics data to a file with the specified filename.
   * @param filename The name of the file to which the statistics will be
   * exported. This is "TrafficData.csv" in the program.
   */
  void exportStats(const std::string &filename);

  /**
   * @brief Returns the amount of pedestrians currently traveling on roads.
   */
  int getPedCount() const;

  /**
   * @brief Returns new ID for new resident.
   */
  int newResidentId() { return residentCount_++; }

private:
  /** @brief incrementTime increments the time in the city by 1 second */
  void incrementTime();

  /** @brief city_ contains the square grid in double vector */
  std::vector<std::vector<Square *>> city_;
  /** @brief containsSomething_ contains the squares that have something in them
   */
  std::vector<Square *> containsSomething_;
  /** @brief residentCount_ contains the amount of residents in the city */
  int residentCount_;
  /** @brief time_ contains the time in the city in simulate steps */
  int time_;

  std::vector<int> hourCounters_;

  CityStats stats_;
};

#endif
