#ifndef COMMERCIAL_HPP
#define COMMERCIAL_HPP

#include "../Building.hpp"

/**
 * @brief Commercial building represents an commercial building in the traffic simulation
 *
 * Commercial building is one type of building for the residents who are going to stores, gyms, restaurants etc. The capacity
 * is randomized by the program so the user can't affect it.
 */

class Commercial : public Building {
public:
  /**
   * @brief Construct a new Commercial building object
   *
   * @param pos Position of the building
   * @param city Reference to the city
   * @param capacity The maximum number of people in the building
  */
    Commercial(Position pos, City &city, int capacity)
      : Building(pos, city, capacity) {
      }

  /**
   * @brief Checks the type of building
   * 
   * @return BuildingType Commercial
  */
    BuildingType buildingType() const { return BuildingType::Commercial; }


private:
        
};

#endif