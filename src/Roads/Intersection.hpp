#pragma once
#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#define LIGHT_LENGTH 50
#define REACTION_TIME 7
#define REACTION_TIME2 5
#define JAM_LENGTH 7
#define PED_CROSSING_TIME 30 // HUOM LIGHT_LENGTH
#define PED_OUT_TIME 10

#include "../Square.hpp"
#include "../Utilities.hpp"

#include <map>
#include <memory>
#include <optional>
#include <vector>

/**
 * @brief Intersection represents an intersection in the traffic simulation
 *
 * Intersection contains output and input spots for cars and pedestrians and
 * simulates them according to the traffic laws. They move the cars and
 * pedestrians where they need to go.
 */
class Intersection : public Square {
public:
  /**
   * @brief Construct a new Intersection object
   *
   * @param pos Position of the intersection
   * @param city Reference to the city
   * @param dirs Directions of the roads
   * @param type Type of the intersection
   */
  Intersection(Position pos, City &city, std::vector<Direction> dirs,
               int type = 0);

  /** @brief Destroy the Intersection object */
  ~Intersection() = default;

  /** @brief initSquare initializes the intersection */
  virtual void initSquare();

  /**
   * @brief addCar adds a car to the intersection
   *
   * The function adds a car to the intersection. The car is added to the
   * input spot of the intersection depending on the direction of the car.
   *
   * @param car the car to add
   * @return int 1 if succesfull, 0 if not
   */
  int addCar(std::unique_ptr<Car> &&car);

  int joinCar(std::unique_ptr<Car> &&car) { return addCar(std::move(car)); }

  /**
   * @brief addPed adds a pedestrian to the intersection
   *
   * The function adds a pedestrian to the intersection. The pedestrian is added
   * to the input vector of the intersection depending on the direction of the
   * pedestrian.
   *
   * @param ped the pedestrian to add
   * @return int 1 if succesfull, 0 if not
   */
  int addPed(std::unique_ptr<Resident> &&ped);

  /**
   * @brief simulate simulates the intersection
   *
   * The function simulates the intersection by moving the cars in the
   * intersection according to the traffic laws. The function also takes into
   * account the reaction time of the drivers.
   */
  virtual void simulate();

  /**
   * @brief getCarCount returns the number of cars in the intersection
   *
   * @return int number of cars in the intersection
   */
  virtual int getCarCount() const;

  virtual int getPedCount() const;

  /**
   * @brief isRoad returns true for intersections
   *
   * @return true
   */
  bool isRoad() const { return true; }
  /**
   * @brief isIntersection returns true for intersections
   *
   * @return true
   */
  bool isIntersection() const { return true; }

  /**
   * @brief getType returns the type of the intersection
   *
   * @return int type of the intersection, 0 = equal, 1 = traffic light
   */
  int getType() const { return type_; }

  /**
   * @brief getStats returns the statistics of the intersection
   *
   * @return std::vector<double> statistics of the intersection. These are the
   * average hourly number of cars passing through the intersection.
   */
  std::vector<double> getStats() const;

  int maxCarAmount() const { return cars_.size() * 2; }

private:
  /** @brief pushOut tries to push out all cars in the output spots */
  void pushOut();

  /**
   * @brief jalanEnsin simulates the traffic law of giving way to the
   * pedestrians
   *
   * @param dirs Directions of the cars that may be able to go
   * @param finalDirs Directions of the cars definately able to go
   * @return std::vector<Direction> Directions of the cars that may be able to
   * go after the simulation
   */
  std::vector<Direction> jalanEnsin(std::vector<Direction> dirs,
                                    std::vector<Direction> &finalDirs);

  /**
   * @brief vasenVaistaa simulates the traffic law of giving way to the right
   *
   * @param dirs Directions of the cars that may be able to go
   * @param finalDirs Directions of the cars definately able to go
   * @return std::vector<Direction> Directions of the cars that may be able to
   * go after the simulation
   */
  std::vector<Direction> vasenVaistaa(std::vector<Direction> dirs,
                                      std::vector<Direction> &finalDirs);

  /**
   * @brief ristiinVaistaa simulates the traffic law of giving way if you turn
   * to the left and the car in front of you goes straight
   *
   * @param dirs Directions of the cars that may be able to go
   * @param finalDirs Directions of the cars definately able to go
   * @return std::vector<Direction> Directions of the cars that may be able to
   * go after the simulation
   */
  std::vector<Direction> ristiinVaistaa(std::vector<Direction> dirs,
                                        std::vector<Direction> &finalDirs);

  /** @brief simulatePeds simulates the pedestrians in the intersection */
  void simulatePeds();

  /** @brief simulateTasaArvo simulates the equal intersection */
  void simulateTasaArvo();
  /** @brief simulateLiikennevalo simulates the intersection with traffic lights
   */
  void simulateLiikennevalo();
  /** @brief getActiveDir returns the direction from where cars can go in a
   * light intersection */
  Direction getActiveDir();

  /**
   * @brief removeCarFromI removes a pedestrian from place at the the
   * intersection
   *
   * @param i the index of the ped in the intersection
   * @param dir the direction of the ped
   * @param io is the ped in the input or output spot: true = input
   */
  void removePedFromI(int i, Direction dir, bool input);

  /**
   * @brief cars_ cars in the intersection
   *
   * The cars are stored in a map with the direction of the car as the key and a
   * size 2 vector as the value. The value represents the input and output spots
   * at the direction. The first spot is the input spot and the second spot is
   * the output spot.
   */
  std::map<Direction, std::vector<std::unique_ptr<Car>>> cars_;

  /**
   * @brief peds_ pedestrians in the intersection
   *
   * The pedestrians are stored in a map with the direction of the pedestrian as
   * the key and a pair of vectors as the value. The first vector represents the
   * input spots and the second vector represents the output spots. the vectors
   * can be infinitely long, to represent people gathering in front of sidewalks
   * at light intersections
   */
  std::map<Direction, std::pair<std::vector<std::unique_ptr<Resident>>,
                                std::vector<std::unique_ptr<Resident>>>>
      peds_;

  /**
   * @brief type_ the type of the intersection
   * 0 = equal intersection
   * 1 = traffic light intersection
   */
  int type_;

  /** @brief status_ the status of the cars or lights. 0 means that the car can
   * move */
  std::map<Direction, int> status_;

  /** @brief pedStatus_ the status of the pedestrians */
  std::map<Direction, std::pair<std::vector<int>, std::vector<int>>> pedStatus_;

  /** @brief lastLightChange_ is the last time the light has changed */
  int lastLightChange_; // Viimeisin valonvaihto

  /** @brief Statistics for the intersection. Keeps track of the number of cars
   * passed through the intersection hourly. */
  IntersectionStats stats_;
};

#endif