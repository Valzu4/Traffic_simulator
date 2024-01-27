#pragma once
#ifndef Kukko
#define Kukko

#include <math.h>

#include <climits>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <iomanip>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <tuple>
#include <vector>

#include "../Square.hpp"
#include "../Utilities.hpp"
#include "Resident.hpp"

using namespace std;

/**
 * @brief Car class represents a car in the city
 *
 * The car class contains the information about the car and the functions that
 * simulate the car's actions in the simulation.
 */
class Car {
public:
  /**
   * @brief Constructor for the Car class.
   *
   * @param city Reference to the City object.
   * @param startPos Initial position of the car.
   * @param endPos Final destination of the car.
   * @param id Unique identifier for the car.
   */
  Car(City &city, const Position startPos, const Position endPos, int id);

  /**
   * @brief Destructor for the Car class.
   *
   * Moves residents in the car to their final destination building.
   */
  ~Car();

  /**
   * @brief Assignment operator for the Car class.
   *
   * @param other Another Car object for assignment.
   * @return A reference to the assigned Car object.
   */
  Car &operator=(Car &other);

  /**
   * @brief Adds a resident as a passenger to the car.
   *
   * @param KimiRaikkonen Reference to the resident to be added as a passenger.
   * @return True if the addition is successful, false otherwise.
   */
  bool addPassenger(std::unique_ptr<Resident> &KimiRaikkonen);

  /**
   * @brief Retrieves the passenger from the car.
   *
   * @return A unique pointer to the passenger resident, or nullptr if there is
   * no passenger.
   */
  std::unique_ptr<Resident> getPassenger();

  /**
   * @brief Finds the shortest path from the start position to the end position
   * using A* algorithm.
   *
   * @return The length of the found path, or -1 if no path is found.
   */
  int findPath();

  /**
   * @brief Gets the current direction the car is facing.
   *
   * @return The current direction of the car.
   */
  const Direction getDir() const;

  /**
   * @brief Peeks at the next direction in the car's movement path.
   *
   * @return The next direction in the movement path.
   */
  Direction peekDir() const;

  /**
   * @brief Updates the direction the car is facing during movement.
   *
   * @return The updated direction the car is facing.
   */
  Direction updateDir();

  /**
   * @brief Creates a custom path for the car.
   *
   * @param path_ The vector of directions representing the custom path.
   */
  void createPath(std::vector<Direction> &path);

  /**
   * @brief Simulates the daily routine and actions of residents inside the car.
   *
   * @param cur The current position of the car.
   */
  void simulate(Position cur);

private:
  const Position startPos_;
  const Position endPos_;
  City &city_;
  std::vector<Direction> path;
  std::vector<std::unique_ptr<Resident>> passList;
  int ii = 0;
  Direction currentDir;
  int id_;
};

#endif