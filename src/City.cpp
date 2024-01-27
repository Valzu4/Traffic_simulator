#include "City.hpp"
#include <fstream>

City::City(size_t size) {
  city_.resize(size);
  for (size_t i = 0; i < size; i++) {
    city_[i].resize(size);
    for (size_t j = 0; j < size; j++) {
      city_[i][j] = new Square(Position(i, j), *this);
    }
  }
  time_ = 0;
  hourCounters_ = std::vector<int>(24, 0);
}

void City::clearAndResize(size_t size) {
  for (auto column : city_) {
    for (auto square : column) {
      delete square;
    }
    column.clear();
  }
  city_.clear();
  city_.resize(size);
  for (size_t i = 0; i < size; i++) {
    city_[i].resize(size);
    for (size_t j = 0; j < size; j++) {
      city_[i][j] = new Square(Position(i, j), *this);
    }
  }
  time_ = 0;
  stats_ = CityStats();
}

void City::add(Position pos, Square *square) {
  if (pos.x < getCitySize() && pos.y < getCitySize()) {
    Square *old = city_[pos.x][pos.y];
    city_[pos.x][pos.y] = nullptr;
    delete old; // Delete the existing square
    city_[pos.x][pos.y] = square;
  } else {
    throw std::out_of_range("Coordinates out of bounds");
  }
}

Square *City::getSquare(Position pos) {
  if (pos.x < getCitySize() && pos.y < getCitySize()) {
    return city_[pos.x][pos.y];
  } else {
    throw std::out_of_range("Coordinates out of bounds");
  }
}

void City::simulate() {
  stats_.incrementCounter(static_cast<int>(getTime()), currentRoadTrafficPercentage());
  int prevTime = static_cast<int>(getTime());
  incrementTime();
  int newTime = static_cast<int>(getTime());
  if (newTime != prevTime) { // tunti vaihtunut
    hourCounters_[prevTime]++;
  }
  for (auto square : containsSomething_) {
    square->simulate();
  }
}

std::vector<Square *> City::getIndustrials() const {
  std::vector<Square *> industrials;
  for (auto &column : city_) {
    for (auto &square : column) {
      if (square->buildingType() == BuildingType::Industrial) {
        industrials.push_back(square);
      }
    }
  }
  return industrials;
}

std::vector<Square *> City::getCommercials() const {
  std::vector<Square *> commercials;
  for (auto &column : city_) {
    for (auto &square : column) {
      if (square->buildingType() == BuildingType::Commercial) {
        commercials.push_back(square);
      }
    }
  }
  return commercials;
}

std::vector<Square *> City::getResidentials() const {
  std::vector<Square *> residentials;
  for (auto &column : city_) {
    for (auto &square : column) {
      if (square->buildingType() == BuildingType::Residential) {
        residentials.push_back(square);
      }
    }
  }
  return residentials;
}

void City::incrementTime() {
  time_++;
  if ((double)time_ / TIMEDIVIDER >= 24) {
    time_ = 0;
  }
}

void City::init() {
  containsSomething_.clear();
  for (auto &column : city_) {
    for (auto square : column) {
      if (!square->isEmpty()) {
        containsSomething_.push_back(square);
      }
      square->initSquare();
    }
  }
}

std::vector<Road *> City::getRoads() const {
  std::vector<Road *> roads;
  for (auto column : city_) {
    for (auto square : column) {
      if (square->isRoad() && !square->isIntersection()) {
        Road *road = static_cast<Road *>(square);
        roads.push_back(road);
      }
    }
  }
  return roads;
}

std::vector<Intersection *> City::getIntersections() const {
  std::vector<Intersection *> intersections;
  for (auto column : city_) {
    for (auto square : column) {
      if (square->isIntersection()) {
        Intersection *intersection = static_cast<Intersection *>(square);
        intersections.push_back(intersection);
      }
    }
  }
  return intersections;
}

double City::currentRoadTrafficPercentage() const {
  const std::vector<Road *> roads = getRoads();
  double sum = 0;
  for (auto road : roads) {
    sum += road->getCarCount() / 10.0;
  }
  if (getRoads().size() > 0) {
    return sum / roads.size();
  } else {
    return 0;
  }
}

int City::getResidentAmount() const {
  int sum = 0;
  for (auto square : getResidentials()) {
    Building *building = static_cast<Building *>(square);
    sum += building->getCapacity();
  }
  return sum;
}

int City::carsOnRoads() const {
  int sum = 0;
  for (auto road : getRoads()) {
    sum += road->getCarCount();
  }
  return sum;
}

int City::getPedCount() const{
  int count = 0;
  for (auto road : getRoads()) {
    count += road->getPedCount();
  }
  for (auto intersection : getIntersections()) {
    count += intersection->getPedCount();
  }
  return count;
}

double City::averageRoadTrafficPercentage() const {
  double sum = 0;
  for (auto stat : getStats()) {
    sum += stat;
  }
  return sum / 24;
}

void City::exportStats(const std::string &filename) {
  std::ofstream csvFile(filename);

  if (!csvFile.is_open()) {
    std::cerr << "Failed to open the CSV file for writing." << std::endl;
    return;
  }
  std::vector<Intersection *> intersections = getIntersections();
  std::vector<Road *> roads = getRoads();
  // Write a header row with column names
  csvFile << "X,Y,SquareType,Data (Average for roads and car amounts for intersections)\n";
  // Roads first
  for (auto road : roads) {
    int x = road->getCoordinates().x;
    int y = road->getCoordinates().y;
    csvFile << x << "," << y << ","<<"Road";

    std::vector<double> stats = road->getStats();
    for(auto stat : stats){
      csvFile << "," << std::to_string(stat);
    }
    csvFile << "\n";
  }
  for(auto intersection : intersections){
    int x = intersection->getCoordinates().x;
    int y = intersection->getCoordinates().y;
    csvFile << x << "," << y << ","<<"Intersection";
    std::vector<double> stats = intersection->getStats();
    for(auto stat : stats){
      csvFile << "," << std::to_string(stat);
    }
    csvFile << "\n";
  }
  csvFile.close();
  std::cout << "Analysis data has been written to: " << filename << std::endl;
}

City::~City() {
  // Clean up the dynamically allocated memory
  for (auto column : city_) {
    for (auto square : column) {
      delete square;
    }
  }
}
