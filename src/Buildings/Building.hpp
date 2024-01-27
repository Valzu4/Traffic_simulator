#pragma once
#ifndef BUILDING_HPP
#define BUILDING_HPP

#include <vector>

#include "../Cars/Car.hpp"
#include "../Cars/Resident.hpp"
#include "../Roads/Road.hpp"
#include "../Square.hpp"
#include "../Utilities.hpp"
#include <algorithm>

class Resident;

/**
 * @brief Building represents an building in the traffic simulation
 *
 * Building contains a vector for residents. When the resident is about to leave, it is moved to either pedestrians vector,
 * or if going by car, it is moved to a car and cars have their own vector. There are also different type of buildings and different type
 * of buildings have different capacities for residents.
 */

class Building : public Square {
public:
  /**
   * @brief Construct a new Building object
   *
   * @param pos Position of the building
   * @param city Reference to the city
   * @param capacity The maximum number of people in the building
   */
  Building(Position pos, City &city, int capacity)
      : Square(pos, city), capacity_(capacity){};

  /**
   * @brief Destroy the Building object
   *
   */
  ~Building() = default;

  /**
   * @brief Removes all residents and cars
   */
  virtual void initSquare();

  /**
   * @brief Create a new resident and add it to the building
   *
   * @param city Reference to the city
   * @param currentPos Position where the resident is created
   * @param id Id for the new resident
   *
   * @return int 1 if successful, 0 if not
   */
  int newResident(City &city, Position currentPos, int id);

  /**
   * @brief Adds resident to the building
   *
   * @param resident Resident to be added
   *
   * @return int 1 if successful, 0 if not
   */
  int addResident(std::unique_ptr<Resident> &resident);

  /**
   * @brief Creates a car and moves the resident from building to the car
   *
   * @param residentId Id of the resident
   * @param endPos The position where the car is supposed to go
   * @param carId Id for the new car
   *
   * @return int 1 if successful, 0 if not
   */
  int removeResident(int residentId, const Position endPos, int carId);

  /**
   * @brief Removes the car. This function is called when resident is moved from
   * car to building.
   *
   * @param car Car object which is going to be removed
   *
   * @return int 1
   */
  virtual int addCar(std::unique_ptr<Car> &&car);

  /**
   * @brief Adds a pedestrian to the building
   *
   * @param ped Resident to be added
   *
   * @return int 1 if successful, 0 if not
   */
  virtual int addPed(std::unique_ptr<Resident> &&ped);

  /**
   * @brief simulate moves a car inside the building to the road
   *
   * If there are cars inside the building this function moves one car to the
   * road. If there doesn't exists a road the car can't move. If the first car
   * can't join to the road the the function tries the next one.
   *
   * If there are residents in the building who are about to walk to a new
   * destination, they are moved from pedestrians vector the road
   */
  virtual void simulate();

  /**
   * @brief Get how many cars are in the building
   *
   * @return int number of cars
   */
  virtual int getCarCount() const;

  /**
   * @brief Get the capacity of the building
   *
   * @return int returns the capacity number
   */
  int getCapacity() const { return capacity_; }

  /**
   * @brief Get the number of residents in the building
   *
   * @return int number of residents
   */
  int getResidents() const {
    int count =
        std::count_if(residents_.begin(), residents_.end(),
                      [](const auto &resident) { return resident != nullptr; });
    count += std::count_if(cars_.begin(), cars_.end(),
                           [](const auto &car) { return car != nullptr; });
    return count;
  }

  /**
   * @brief Checks if the square is building or not
   *
   * @return bool true
   */
  bool isBuilding() const { return true; }

  /**
   * @brief Checks the type of building
   *
   * @return BuildingType None
   */
  virtual BuildingType buildingType() const { return BuildingType::None; }

  /**
   * @brief Calls the addCar function to remove the car
   *
   * @param car The car to remove
   * @param dir the direction of the car
   *
   * @return int 1
   */
  int joinCar(std::unique_ptr<Car> &&car, Direction dir);

protected:
  /**
   * @brief Vector containing the Resident pointers
   *
   * When a resident enters the building, it is moved to residents vector. When
   * the resident leaves the building, it is removed from the vector
   */
  std::vector<std::unique_ptr<Resident>> residents_;

  /**
   * @brief Vector containing the Car pointers
   *
   * Car is added to cars_ vector when the car is created for a resident and
   * removed when the car leaves the building. Car never enters a building but
   * is removed when the resident gets to its destination.
   */
  std::vector<std::unique_ptr<Car>> cars_;

  /**
   * @brief Vector containing the Resident pointers that are going to walk
   *
   * When the resident is going to leave the building by walking, it is added to
   * pedestrians_ vector. Pedestrian is moved to a road from the simulate
   * function
   */
  std::vector<std::unique_ptr<Resident>> pedestrians_;

  /**
   * @brief capacity is the number of residents the building can take
   * The user can affect only on the number of residential building capacity. It is set to 500 but can be changed.
   */
  int capacity_;
};

#endif
