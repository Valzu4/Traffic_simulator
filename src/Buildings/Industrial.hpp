#ifndef INDUSTRIAL_HPP
#define INDUSTRIAL_HPP

#include "Building.hpp"

/**
 * @brief Industrial building represents an industrial building in the traffic simulation
 *
 * Industrial building is one type of building for the residents who are going to work. The capacity is randomized by the program
 * so the user can't affect it.
 */

class Industrial : public Building {
public:
  /**
   * @brief Construct a new Industrial building object
   *
   * @param pos Position of the building
   * @param city Reference to the city
   * @param capacity The maximum number of people in the building
  */
    Industrial(Position pos, City &city, int capacity)
      : Building(pos, city, capacity) {
      }

  /**
   * @brief Checks the type of building
   * 
   * @return BuildingType Industrial
  */
    BuildingType buildingType() const { return BuildingType::Industrial; }

};

#endif