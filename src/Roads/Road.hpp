#pragma once
#ifndef ROAD_HPP
#define ROAD_HPP

#define ROAD_LENGTH 5
#define PED_TIMER 60 // HUOM liikkuu joka askeleella
#define DEFAULT_SPEED 1

#include "../Cars/Car.hpp"
#include "../Square.hpp"
#include "../Utilities.hpp"

#include <map>
#include <memory>
#include <vector>

/**
 * @brief Road class represents a road in the traffic simulation
 *
 * The road class contains the vectors that stores cars and pedestrians that are
 * on the move. Roads move the cars and pedestrians in the simulation
 */
class Road : public Square {
public:
  /**
   * @brief Construct a new Road object
   *
   * @param pos Position of thD the city
   * @param speed Speed of the cars on the road
   */
  Road(Position pos, City &city, int speed = 0);

  /**
   * @brief Destroy the Road object
   *
   */
  ~Road() = default;

  virtual int addPed(std::unique_ptr<Resident> &&ped);

  virtual int addCar(std::unique_ptr<Car> &&car);

  int joinCar(std::unique_ptr<Car> &&car);

  /**
   * @brief getCarCount returns the number of cars on the road
   *
   * @return int number of cars on the road
   */
  virtual int getCarCount() const;

  virtual int getPedCount() const { return peds_.size(); }

  /**
   * @brief getNECarCount returns the number of cars on the road in the
   * directions North and East
   *
   * @return int number of cars on the road in the directions North and East
   */
  virtual int getNECarCount() const;

  /**
   * @brief getSWCarCount returns the number of cars on the road in the
   * directions South and West
   *
   * @return int number of cars on the road in the directions South and West
   */
  virtual int getSWCarCount() const;

  /**
   * @brief simulate simulates the movement of the cars on the road
   *
   * The function simulates the movement of the cars on the road. The function
   * moves the cars if there is space in front of them, and they have waited for
   * the time that is defined by the speed of the road.
   *
   * The function also calls the simuate functions for the Residents inside the
   * cars.
   */
  virtual void simulate();

  /**
   * @brief initSquare initializes the vectors and maps of the road
   *
   * The function initializes the vectors and maps of the road. The function is
   * called when the simulation is reset.
   */
  virtual void initSquare();

  /**
   * @brief Checks if the Square object is a road.
   * @return True.
   */
  bool isRoad() const { return true; }

  /**
   * @brief Retrieves the statistics from the object.
   * @return A vector of double representing the average hourly car counts on
   * the road.
   */
  std::vector<double> getStats() const;

  /**
   * @brief Checks if the road has a north-south orientation. This is needed for
   * the heatmap.
   * @return True if the road is oriented north-south, north-east, north-west,
   * south-west or south-east. The heatmap shows all these variations as two up
   * to down lanes.
   */
  bool isNS();

  /**
   * @brief Checks if the road has an east-west orientation.
   * @return True if the road is oriented east-west, false otherwise.
   */
  bool isEW();

  /**
   * @brief Calculates and returns the average traffic percentage on the road
   * from all hours of the entire simulation period.
   * @return The average traffic percentage as a double.
   */
  double averageTrafficPercent() const;

  /**
   * @brief Get the Speed object
   *
   * @return int speed of the cars on the road
   */
  int getSpeed() { return speed_; }

private:
  /**
   * @brief Vector containing the Car pointers for the directions North and
   * East.
   *
   * One of the vectors that contain the Car pointers. The vector is set to a
   * certain length, to simulate the space within the road. When there is no car
   * in the space, the pointer is set to nullptr and when there is a car in a
   * certain spot of the road, the pointer points to the car.
   */
  std::vector<std::unique_ptr<Car>> cars1_;

  /**
   * @brief Vector containing the Car pointers for the directions South and
   * West.
   *
   * One of the vectors that contain the Car pointers. The vector is set to a
   * certain length, to simulate the space within the road. When there is no car
   * in the space, the pointer is set to nullptr and when there is a car in a
   * certain spot of the road, the pointer points to the car.
   */
  std::vector<std::unique_ptr<Car>> cars2_;

  /**
   * @brief Vector containing the Resident pointers for the directions North and
   * East.
   *
   * One of the vectors that contain the Resident pointers. The vector is set to
   * a certain length, to simulate the space within the road. When there is no
   * Resident in the space, the pointer is set to nullptr and when there is a
   * Resident in a certain spot of the road, the pointer points to the Resident.
   */
  std::vector<std::pair<std::unique_ptr<Resident>, int>> peds_;

  /**
   * @brief Map containing the status of the objects on the road.
   *
   * The map contains the status of the road. The key is the position of the
   * car, and the value is a pair of ints. The first int indicates the states of
   * the cars1_ vector and the second int indicates the states of the cars2_
   * vector.
   */
  std::map<int, std::pair<int, int>> status_;

  /**
   * @brief The speed of the cars on the road
   *
   * The speed of the cars on the road. The speed is implemented with how long
   * the cars wait until they move one step forward.
   *
   * The value indicates the speed: 0 = 60km/h, 1 = 40km/h, 2 = 30km/h.
   *
   */
  int speed_;

  /**
   * @brief The Stats objects for each hour of the day. These keep track of
   * average car counts on the road for example.
   */
  RoadStats statistics[24];
};

#endif