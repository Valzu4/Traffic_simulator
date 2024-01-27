#ifndef RESIDENTIAL_HPP
#define RESIDENTIAL_HPP

#include "../City.hpp"
#include "Building.hpp"

/**
 * @brief Residential building represents an residential building in the traffic simulation
 *
 * Residential building is one type of building where the residents are implemented. The capacity is set to 100 by the program
 * but user can also change it.
 */

class Residential : public Building {
public:
  /**
   * @brief Construct a new Residential building object
   *
   * @param pos Position of the building
   * @param city Reference to the city
   * @param capacity The maximum number of people in the building
   */
  Residential(Position pos, City &city, int capacity)
      : Building(pos, city, capacity) {}

  /**
   * @brief Adds residents to the building
   */
  void initSquare();

  /**
   * @brief Checks the type of building
   *
   * @return BuildingType Residential
   */
  BuildingType buildingType() const { return BuildingType::Residential; }

private:
};

#endif