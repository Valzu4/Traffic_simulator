#pragma once
#ifndef SQUARE_HPP
#define SQUARE_HPP

/* HUOM!!!
  Mikäli haluat luokalle lisää ominaisuuksia, niin joko:
  1. Kirjoita toiveet alle
  2. Tee vittu ite

  Toiveet:


*/

#include "Utilities.hpp"

#include <iostream>
#include <memory>
#include <typeinfo>
#include <utility>

class Car;
class Resident;
class City;
class Road;

/**
 * @brief Square class is a building block of city, and might contain buildings
 * or roads.
 *
 * Square is a building block of the city. It can contain buildings, roads or be
 * empty. Empty squares simulate empty spaces in the city, and don't interract
 * with other squares.
 */
class Square {
public:
  /**
   * @brief Construct a new Square object
   *
   * @param pos Position of the square
   * @param city Reference to the city
   */
  Square(Position pos, City &city) : coordinates_(pos.x, pos.y), city_(city) {}

  /** @brief Destroy the Square object */
  ~Square() = default;

  /** @brief getCoordinates returns the coordinates of the square
   *
   * @return Position coordinates of the square
   */
  Position getCoordinates() const { return coordinates_; }

  /** @brief getNeighbour returns the neighbour of the square in the given
   * direction
   *
   * @param dir Direction of the neighbour to get
   * @return Square* Pointer to the neighbour
   */
  Square *getNeighbour(Direction dir);

  /**
   * @brief simulate simulates the functionalities of the square
   *
   * Each different square type has its own implementation of the function.
   * Empty square does nothing.
   */
  virtual void simulate() { return; }

  /**
   * @brief getCarCount returns the ammount of cars in the square
   *
   * @return the ammount of cars in the square. Default implementation returns
   * 0.
   */
  virtual int getCarCount() const { return 0; }

  /**
   * @brief getPedCount returns the ammount of pedestrians in the square
   *
   * @return the ammount of pedestrians in the square. Default implementation
   * returns 0.
   */
  virtual int getPedCount() const { return 0; }

  /**
   * @brief addPed adds a pedestrian to the square
   *
   * The function adds a pedestrian to the road. The pedestrian is added to the
   * beginning of the road vector, which is defined by the direction of the
   * pedestrian.
   *
   * @param ped the pedestrian to add
   * @return int 1 if succesfull, 0 if not
   */
  virtual int addPed(std::unique_ptr<Resident> &&ped) { return 0; }

  /**
   * @brief addCar adds a car to the square.
   *
   * Every subclass has its own implementation of the function. Default
   * implementation does nothing and returns 0 for failure.
   *
   * @param car the car to add
   * @return int 1 if succesfull, 0 if not. (Default 0)
   */
  virtual int addCar(std::unique_ptr<Car> &&car) { return 0; }

  /**
   * @brief joinCar adds a car from a building to the road or intersection
   *
   * The function adds a car to the road. The car is added to the middle of the
   * road, to simulate the car driving out of a building to the middle of the
   * road.
   *
   * @param car the car to add
   *
   * @return 1 if successfull, 0 if not (default is 0)
   */
  virtual int joinCar(std::unique_ptr<Car> &&car) { return 0; }

  /**
   * @brief operator= copy assignment operator for square
   *
   *
   * @param other the square to copy
   * @return Square& reference to the square
   *
   */
  Square &operator=(const Square &other);

  // Onko jotain tyyppiä
  /** @brief isRoad returns is the square type road */
  virtual bool isRoad() const { return false; }
  /** @brief isBuilding returns is the square type building */
  virtual bool isBuilding() const { return false; }
  /** @brief buildingType returns the building type in the square, Returns None
   * if it is not building */
  virtual BuildingType buildingType() const { return BuildingType::None; }
  /** @brief isEmpty returns is the square type empty */
  virtual bool isEmpty() const { return (!isRoad() && !isBuilding()); }
  /** @brief isIntersection returns is the square type intersection */
  virtual bool isIntersection() const { return false; }
  /**
   * @brief initSquare initializes the square for simulation
   */
  virtual void initSquare() { return; }

protected:
  /** @brief coordinates_ The coordinates of the square */
  Position coordinates_;
  /** @brief city_ Reference to the city where the square resides */
  City &city_;
};

#endif
