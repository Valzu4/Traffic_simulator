#include "../Cars/Car.hpp"
#include "../City.hpp"
#include "Road.hpp"

Road::Road(Position pos, City &city, int speed) : Square(pos, city), speed_(speed) {
  cars1_.resize(ROAD_LENGTH);
  cars2_.resize(ROAD_LENGTH);
  for (size_t i = 0; i < ROAD_LENGTH; i++) {
    status_[i] = std::make_pair(0, 0);
  }
}

void Road::initSquare() {
  cars1_.clear();
  cars2_.clear();
  peds_.clear();
  cars1_.resize(ROAD_LENGTH);
  cars2_.resize(ROAD_LENGTH);
}

int Road::getCarCount() const {
  int count = 0;
  for (auto &car : cars1_) {
    if (car) {
      count++;
    }
  }
  for (auto &car : cars2_) {
    if (car) {
      count++;
    }
  }
  return count;
}

int Road::addPed(std::unique_ptr<Resident> &&ped) {
  if (!ped)
    return 0;
  ped->updateDir();

  peds_.push_back(std::make_pair(std::move(ped), PED_TIMER));
  return 1;
}

int Road::addCar(std::unique_ptr<Car> &&car) {
  if (!car) {
    return 0;
  }

  if ((car->getDir() == Direction::N) ||
      ((car->getDir() == Direction::E && isEW()) || (car->peekDir() == Direction::N && isNS()))) {
    if (!cars1_[0]) {
      cars1_[0] = std::move(car);
      cars1_[0]->updateDir();
      status_[0].first = DEFAULT_SPEED + speed_;
      return 1;
    }
    return 0;

  } else if (car->getDir() == Direction::S || ((car->getDir() == Direction::W && isEW()) ||
                                               (car->peekDir() == Direction::S && isNS()))) {
    if (!cars2_[0]) {
      cars2_[0] = std::move(car);
      cars2_[0]->updateDir();
      status_[0].second = DEFAULT_SPEED + speed_;
      return 1;
    }
    return 0;
  } else {
    car->updateDir();
    getNeighbour(car->getDir())->addCar(std::move(car));
  }
  return 0;
}

int Road::joinCar(std::unique_ptr<Car> &&car) {
  if (!car) {
    return 0;
  }

  if (car->peekDir() == Direction::N || car->peekDir() == Direction::E) {
    if (!cars1_[0] && !cars1_[1]) {
      cars1_[1] = std::move(car);
      cars1_[1]->updateDir();
      status_[1].first = DEFAULT_SPEED + speed_;
      return 1;
    }
    return 0;
  } else if (car->peekDir() == Direction::S || car->peekDir() == Direction::W) {
    if (!cars2_[0] && !cars2_[1]) {
      cars2_[1] = std::move(car);
      cars2_[1]->updateDir();
      status_[1].second = DEFAULT_SPEED + speed_;
      return 1;
    }
    return 0;
  }
  return 0;
}

void Road::simulate() {
  for (int i = 0; i < peds_.size(); i++) {
    if (!peds_[i].first) {
      continue;
    }

    if (peds_[i].second) {
      peds_[i].first->simulate(coordinates_);
      peds_[i].second--;
    } else {
      peds_[i].first->simulate(coordinates_);
      if (getNeighbour(peds_[i].first->getDir())->addPed(std::move(peds_[i].first))) {
        peds_.erase(peds_.begin() + i);
        i--;
      }
    }
  }

  for (size_t i = 0; i < cars1_.size() - 1; i++) {
    if (!cars1_[i])
      continue;
    if (cars1_[i] && !cars1_[i + 1] && !status_[i].first) {
      cars1_[i + 1] = std::move(cars1_[i]);
      status_[i + 1].first = DEFAULT_SPEED + speed_;
      if (i + 1 == cars1_.size() - 1) {
        status_[i + 1].first++;
      }
      status_[i].first = 0;
      cars1_[i].reset();
      i++;
    } else if (cars1_[i] && status_[i].first) {
      status_[i].first--;
    }
    if (cars1_[i]) {
      cars1_[i]->simulate(coordinates_);
    }
  }

  if (cars1_[cars1_.size() - 1] && !status_[cars1_.size() - 1].first) {
    Square *next = getNeighbour(cars1_[cars1_.size() - 1]->getDir());
    next->addCar(std::move(cars1_[cars1_.size() - 1]));
  } else if (status_[cars1_.size() - 1].first) {
    status_[cars1_.size() - 1].first--;
  }

  for (size_t i = 0; i < cars2_.size() - 1; i++) {
    if (!cars2_[i])
      continue;
    if (cars2_[i] && !cars2_[i + 1] && !status_[i].second) {
      cars2_[i + 1] = std::move(cars2_[i]);
      status_[i + 1].second = DEFAULT_SPEED + speed_;
      status_[i].second = 0;
      if (i + 1 == cars2_.size() - 1) {
        status_[i + 1].second++;
      }
      cars2_[i].reset();
      i++;
    } else if (cars2_[i] && status_[i].second) {
      status_[i].second--;
    }
    if (cars2_[i]) {
      cars2_[i]->simulate(coordinates_);
    }
  }
  if (cars2_[cars2_.size() - 1] && !status_[cars2_.size() - 1].second) {
    Direction dir = cars2_[cars2_.size() - 1]->getDir();
    getNeighbour(dir)->addCar(std::move(cars2_[cars2_.size() - 1]));
  } else if (status_[cars2_.size() - 1].second) {
    status_[cars2_.size() - 1].second--;
  }

  statistics[static_cast<int>(city_.getTime())].incrementCounter(getCarCount());
}

std::vector<double> Road::getStats() const {
  std::vector<double> averages;
  for (auto &stat : statistics) {
    averages.push_back(stat.getAverage());
  }
  return averages;
}

bool Road::isNS() {
  if (getNeighbour(Direction::N) && getNeighbour(Direction::N)->isRoad() &&
      getNeighbour(Direction::S) && getNeighbour(Direction::S)->isRoad()) {
    return true;
  } else if (getNeighbour(Direction::N) && getNeighbour(Direction::N)->isRoad() &&
             getNeighbour(Direction::W) && getNeighbour(Direction::W)->isRoad()) {
    return true;
  } else if (getNeighbour(Direction::N) && getNeighbour(Direction::N)->isRoad() &&
             getNeighbour(Direction::E) && getNeighbour(Direction::E)->isRoad()) {
    return true;
  } else if (getNeighbour(Direction::S) && getNeighbour(Direction::S)->isRoad() &&
             getNeighbour(Direction::W) && getNeighbour(Direction::W)->isRoad()) {
    return true;
  } else if (getNeighbour(Direction::S) && getNeighbour(Direction::S)->isRoad() &&
             getNeighbour(Direction::E) && getNeighbour(Direction::E)->isRoad()) {
    return true;
  } else if (getNeighbour(Direction::N) && getNeighbour(Direction::N)->isRoad() ||
             getNeighbour(Direction::S) && getNeighbour(Direction::S)->isRoad()) {
    return true;
  } else {
    return false;
  }
}

bool Road::isEW() {
  if (getNeighbour(Direction::E) && getNeighbour(Direction::E)->isRoad() &&
      getNeighbour(Direction::W) && getNeighbour(Direction::W)->isRoad()) {
    return true;
  } else if (getNeighbour(Direction::W) && getNeighbour(Direction::W)->isRoad() ||
             getNeighbour(Direction::E) && getNeighbour(Direction::E)->isRoad()) {
    return true;
  } else {
    return false;
  }
}

int Road::getNECarCount() const {
  int count = 0;
  for (auto &car : cars1_) {
    if (car) {
      count++;
    }
  }
  return count;
}

int Road::getSWCarCount() const {
  int count = 0;
  for (auto &car : cars2_) {
    if (car) {
      count++;
    }
  }
  return count;
}

double Road::averageTrafficPercent() const {
  double sum = 0;
  for (auto stat : getStats()) {
    sum += stat;
  }
  sum = sum / 24;
  return sum / 10; // 10 is max number of cars on a road
}