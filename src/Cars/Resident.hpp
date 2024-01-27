#pragma once
#ifndef Kana
#define Kana

#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>

#include "../Utilities.hpp"

// toivomuksena getId Done :)

class City;

/**
 * @brief Resident class represents a resident in the city
 *
 * The resident class contains the information about the resident and the
 * functions that simulate the resident's actions in the simulation.
 */
class Resident {
public:
  /**
   * @brief Constructor for the Resident class.
   *
   * @param city Reference to the City object.
   * @param currentPos Initial position of the resident.
   * @param id Unique identifier for the resident.
   */
  Resident(City &city, Position currentPos, int id);

  /**
   * @brief Destroy the Resident object
   *
   */
  ~Resident() = default;

  /**
   * @brief Overloaded equality operator for comparing residents based on their
   * IDs.
   *
   * @param r Another Resident object for comparison.
   * @return True if the residents have the same ID, false otherwise.
   */
  bool operator==(Resident &r);

  /**
   * @brief Overloaded inequality operator for comparing residents based on
   * their IDs.
   *
   * @param r Another Resident object for comparison.
   * @return True if the residents have different IDs, false otherwise.
   */
  bool operator!=(Resident &r);

  /**
   * @brief Initiates the process of a resident leaving their current position
   * for a new destination.
   *
   * @param nextPos The position to which the resident intends to move.
   * @return True if the resident successfully initiates the move, false
   * otherwise.
   */
  bool leave(const Position nextPos);

  /**
   * @brief Finds the shortest path from the current position to the destination
   * using A* algorithm.
   *
   * @return The length of the found path, or -1 if no path is found.
   */
  int findPath();

  /**
   * @brief Resets the resident's movement state and clears the movement path.
   */
  void enter() {
    onTheMove = false;
    ii = 0;
    path.clear();
  }

  /**
   * @brief Gets the unique identifier of the resident.
   *
   * @return The ID of the resident.
   */
  int getId() const { return id_; }

  /**
   * @brief Gets the current position of the resident.
   *
   * @return The current position of the resident.
   */
  const Position getPos() const { return currentPos_; }

  /**
   * @brief Simulates the daily routine and actions of the resident.
   *
   * @param cur The current position of the resident.
   */
  void simulate(Position cur);

  /**
   * @brief Provides information about the resident.
   *
   * @return A string containing information about the resident.
   */
  std::string info() const;

  /**
   * @brief Gets the current direction the resident is facing.
   *
   * @return The current direction of the resident.
   */
  const Direction getDir() const;

  /**
   * @brief Peeks at the next direction in the resident's movement path.
   *
   * @return The next direction in the movement path.
   */
  Direction peekDir() const;

  /**
   * @brief Updates the direction the resident is facing during movement.
   *
   * @return The updated direction the resident is facing.
   */
  Direction updateDir();

private:
  Position home_;
  Position work_;
  Position currentPos_;
  Position favShop_;
  Position randShop_;
  Position shop_;
  Position destination_;
  City &city_;
  int id_;
  int shopCommuteTime;
  int favShopCommuteTime;
  bool onTheMove;
  bool siesta;
  bool party;
  double night;
  double workBegin;
  double workEnd;
  double workBeginAdd;
  double workEndAdd;
  std::vector<Direction> path;
  Direction currentDir;
  int ii = 0;
};
#endif