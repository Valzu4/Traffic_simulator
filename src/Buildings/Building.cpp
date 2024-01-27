#include "Building.hpp"

#include "../Cars/Car.hpp"
#include "../Cars/Resident.hpp"
#include "../City.hpp"

int Building::newResident(City &city, Position currentPos, int id) {
  std::unique_ptr<Resident> newR =
      std::make_unique<Resident>(city, currentPos, id);
  residents_.push_back(std::move(newR));

  if (residents_.back() != nullptr) {
    return 1;
  } else {
    return 0;
  }
}

int Building::addResident(std::unique_ptr<Resident> &resident) {
  residents_.push_back(std::move(resident));

  if (residents_.back() != nullptr) {
    return 1;
  } else {
    return 0;
  }
}

void Building::initSquare() {
  residents_.clear();
  cars_.clear();
}

int Building::removeResident(int residentId, const Position endPos, int carId) {
  for (auto &r : residents_) {
    if (!r) {
      continue;
    }
    if (r->getId() == residentId) {
      if (carId == 0) {
        pedestrians_.push_back(std::move(r));
        return 1;
      } else {
        std::unique_ptr<Car> newC = std::make_unique<Car>(
            this->city_, this->coordinates_, endPos, carId);
        if (newC->addPassenger(r)) {
          cars_.push_back(std::move(newC));
          return 1;
        }
      }
    }
  }
  return 0;
}

int Building::addCar(std::unique_ptr<Car> &&car) {
  car.reset();
  return 1;
}

int Building::addPed(std::unique_ptr<Resident> &&ped) {
  if (!ped)
    return 0;

  ped->enter();
  residents_.push_back(std::move(ped));
  return 1;
}

void Building::simulate() {
  for (auto &r : residents_) {
    if (!r)
      continue;
    r->simulate(coordinates_);
  }

  auto isNullPtr = [](const auto &r) { return !r; };
  auto nullPtrIterator =
      std::remove_if(residents_.begin(), residents_.end(), isNullPtr);
  residents_.erase(nullPtrIterator, residents_.end());

  for (size_t i = 0; i < pedestrians_.size(); i++) {
    if (pedestrians_[i]) {
      Direction dir = pedestrians_[i]->getDir();
      Square *targetSquare = this->getNeighbour(dir);
      Road *targetRoad = dynamic_cast<Road *>(targetSquare);
      if (targetSquare) {
        if (targetSquare->addPed(std::move(pedestrians_[i]))) {
          pedestrians_.erase(pedestrians_.begin() + i);
        }
      }
    }
  }

  for (size_t i = 0; i < cars_.size(); i++) {
    if (cars_[i]) {
      Direction dir = cars_[i]->getDir();
      Square *targetSquare = this->getNeighbour(dir);
      Road *targetRoad = dynamic_cast<Road *>(targetSquare);
      if (targetSquare) {
        if (targetSquare->joinCar(std::move(cars_[i]))) {
          cars_.erase(cars_.begin() + i);
          break;
        } else {
          break;
        }
      }
    }
  }
}

int Building::getCarCount() const {
  int count = 0;
  for (auto &car : cars_) {
    if (car) {
      count++;
    }
  }
  return count;
}

int Building::joinCar(std::unique_ptr<Car> &&car, Direction dir) {
  return addCar(std::move(car));
}
