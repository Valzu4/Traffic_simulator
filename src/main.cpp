#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Buildings/Building.hpp"
#include "Buildings/Commercial/Commercial.hpp"
#include "Buildings/Industrial.hpp"
#include "Buildings/Residential.hpp"
#include "Cars/Car.hpp"
#include "Cars/Resident.hpp"
#include "City.hpp"
#include "GUI.hpp"
#include "Roads/Intersection.hpp"
#include "Roads/Road.hpp"
#include "Utilities.hpp"

int main() {
  int size = 50;
  City city(size);

  GUI gui(city, size);

  gui.init();

  while (gui.isOpen()) {
    gui.handleEvent();

    gui.drawAndDisplay();
    int simCount; // How many simulation steps per GUI update
    if (gui.currentToolType() == Tool::FastForward) {
      simCount = 100;
    } else if (gui.currentToolType() == Tool::FastForward10x) {
      simCount = 1000;
    } else {
      simCount = 1;
    }

    if (!gui.isPaused()) {
      if (gui.hasCityChanged()) {
        // Update intersections
        for (size_t x = 0; x < size; x++) {
          for (size_t y = 0; y < size; y++) {
            Position pos(x, y);
            if (city.getSquare(pos)->isRoad() && !city.getSquare(pos)->isIntersection()) {
              Road *road = static_cast<Road *>(city.getSquare(pos));
              int roadCount = 0;
              std::vector<Direction> dirs;
              // Check neighbors in all four directions (N, S, E, W)
              if (road->getNeighbour(Direction::N) && road->getNeighbour(Direction::N)->isRoad()) {
                roadCount++;
                dirs.push_back(Direction::N);
              } else if(road->getNeighbour(Direction::N) && road->getNeighbour(Direction::N)->isBuilding()){
                dirs.push_back(Direction::N);
              }

              if (road->getNeighbour(Direction::S) && road->getNeighbour(Direction::S)->isRoad()) {
                roadCount++;
                dirs.push_back(Direction::S);
              } else if(road->getNeighbour(Direction::S) &&road->getNeighbour(Direction::S)->isBuilding()){
                dirs.push_back(Direction::S);
              }

              if (road->getNeighbour(Direction::E) && road->getNeighbour(Direction::E)->isRoad()) {
                roadCount++;
                dirs.push_back(Direction::E);
              } else if(road->getNeighbour(Direction::E) && road->getNeighbour(Direction::E)->isBuilding()){
                dirs.push_back(Direction::E);
              }

              if (road->getNeighbour(Direction::W) && road->getNeighbour(Direction::W)->isRoad()) {
                roadCount++;
                dirs.push_back(Direction::W);
              } else if(road->getNeighbour(Direction::W) && road->getNeighbour(Direction::W)->isBuilding()){
                dirs.push_back(Direction::W);
              }

              if (roadCount > 2) {
                city.add(pos, new Intersection(pos, city, dirs));
              }
            } else if (city.getSquare(pos)->isIntersection()) {
              Intersection *intersection = static_cast<Intersection *>(city.getSquare(pos));
              std::vector<Direction> dirs;
              int roadCount = 0;
              // Check neighbors in all four directions (N, S, E, W)
              if (intersection->getNeighbour(Direction::N) && intersection->getNeighbour(Direction::N)->isRoad()) {
                roadCount++;
                dirs.push_back(Direction::N);
              } else if(intersection->getNeighbour(Direction::N) && intersection->getNeighbour(Direction::N)->isBuilding()){
                dirs.push_back(Direction::N);
              }

              if (intersection->getNeighbour(Direction::S) && intersection->getNeighbour(Direction::S)->isRoad()) {
                roadCount++;
                dirs.push_back(Direction::S);
              } else if(intersection->getNeighbour(Direction::S) && intersection->getNeighbour(Direction::S)->isBuilding()){
                dirs.push_back(Direction::S);
              }

              if (intersection->getNeighbour(Direction::E) && intersection->getNeighbour(Direction::E)->isRoad()) {
                roadCount++;
                dirs.push_back(Direction::E);
              } else if(intersection->getNeighbour(Direction::E) && intersection->getNeighbour(Direction::E)->isBuilding()){
                dirs.push_back(Direction::E);
              }

              if (intersection->getNeighbour(Direction::W) && intersection->getNeighbour(Direction::W)->isRoad()) {
                roadCount++;
                dirs.push_back(Direction::W);
              } else if(intersection->getNeighbour(Direction::W) && intersection->getNeighbour(Direction::W)->isBuilding()){
                dirs.push_back(Direction::W);
              }
              if(roadCount > 2){
                city.add(pos, new Intersection(pos, city, dirs, intersection->getType()));
              } else {
                city.add(pos, new Road(pos, city, 1));
              }
            }
          }
        }
        // restart simulation here
        gui.setChanged(false);
        city.init();
      }
      for (int i = 0; i < simCount; i++) {
        city.simulate();
      }
    }
  }
  return 0;
}