#include "Intersection.hpp"

#include "../Cars/Car.hpp"
#include "../Cars/Resident.hpp"
#include "../City.hpp"

Intersection::Intersection(Position pos, City &city,
                           std::vector<Direction> dirs, int type)
    : Square(pos, city), type_(type) {
  for (auto d : dirs) {
    cars_[d].resize(2);
    status_[d] = 1;
  }

  lastLightChange_ = 0;
  if (type_ == 1) {
    status_[dirs[0]] = 0;
  }
}

void Intersection::initSquare() {
  std::vector<Direction> dirs;
  for (auto &c : cars_) {
    dirs.push_back(c.first);
  }

  cars_.clear();
  peds_.clear();
  for (auto d : dirs) {
    cars_[d].resize(2);
    status_[d] = 1;
  }
  lastLightChange_ = 0;
  if (type_ == 1) {
    status_[dirs[0]] = 0;
  }
}

int Intersection::getCarCount() const {
  int count = 0;
  for (auto &c : cars_) {
    if (c.second[0]) {
      count++;
    }
    if (c.second[1]) {
      count++;
    }
  }
  return count;
}

int Intersection::getPedCount() const {
  int count = 0;
  for (auto &d : peds_) {
    count += d.second.first.size();
    count += d.second.second.size();
  }
  return count;
}

int Intersection::addCar(std::unique_ptr<Car> &&car) {
  if (!car)
    return 0;

  // suunta johon auto sijoitetaan risteyksessä
  Direction dir =
      static_cast<Direction>((static_cast<int>(car->getDir()) + 2) % 4);

  if (!cars_[dir][0]) {
    cars_[dir][0] = std::move(car);
    cars_[dir][0]->updateDir();
    if (type_ == 0) {
      status_[dir] = REACTION_TIME;
    }
    stats_.incrementCounter(static_cast<int>(city_.getTime()));
    return 1;
  }

  return 0;
}

int Intersection::addPed(std::unique_ptr<Resident> &&ped) {
  if (!ped)
    return 0;

  // suunta johon jalankulkija sijoitetaan risteyksessä
  Direction dir =
      static_cast<Direction>((static_cast<int>(ped->getDir()) + 2) % 4);

  peds_[dir].first.push_back(std::move(ped));
  pedStatus_[dir].first.push_back(PED_CROSSING_TIME);
  peds_[dir].first[peds_[dir].first.size() - 1]->updateDir();
  // jeejee stats increment counter TÄNNE jee jee jee

  return 1;
}

/* TODO
- Väistämisvelvolisuusmerkit
*/

void Intersection::simulate() {
  pushOut();

  for (auto &c : cars_) {
    if (c.second[0]) {
      c.second[0]->simulate(coordinates_);
    }
  }

  switch (type_) {
  case 0:
    simulateTasaArvo();
    break;
  case 1:
    simulateLiikennevalo();
    break;
  default:
    break;
  }

  // Light change
  if (type_ == 1) {
    if (lastLightChange_ >= LIGHT_LENGTH) {
      Direction dir = getActiveDir();
      Direction newDir =
          static_cast<Direction>((static_cast<int>(dir) + 1) % 5);

      if (newDir == Direction::None) {
        status_[dir] = 1;
        lastLightChange_ = 0;
      } else {
        status_[dir] = 1;
        while (status_.count(newDir) == 0) {
          newDir = static_cast<Direction>((static_cast<int>(newDir) + 1) % 5);
        }
        status_[newDir] = 0;
        lastLightChange_ = 0;
      }
    } else {
      lastLightChange_++;
    }
  }

  // statistics[static_cast<int>(city_.getTime())].incrementCounter(getCarCount());
}

void Intersection::simulatePeds() {
  for (auto &d : peds_) {
    for (int i = 0; i < d.second.first.size(); i++) {
      if (!peds_[d.first].first[i])
        continue;
      peds_[d.first].first[i]->simulate(coordinates_);

      if (pedStatus_[d.first].first[i] != 0) {
        pedStatus_[d.first].first[i]--;
      } else if (pedStatus_[d.first].first[i] == 0) {
        Direction dir = peds_[d.first].first[i]->getDir();
        peds_[dir].second.push_back(std::move(peds_[d.first].first[i]));
        pedStatus_[dir].second.push_back(PED_OUT_TIME);
        removePedFromI(i, d.first, true);
        i--;
      }
    }
  }
}

void Intersection::pushOut() {
  for (auto &c : cars_) {
    if (c.second[1]) {
      c.second[1]->simulate(coordinates_);
      if (getNeighbour(c.second[1]->getDir())->addCar(std::move(c.second[1])) ==
          1)
        c.second[1].reset();
    }
  }

  for (auto &d : peds_) {

    for (int i = 0; i < d.second.second.size(); i++) {
      if (!peds_[d.first].second[i])
        continue;
      peds_[d.first].second[i]->simulate(coordinates_);

      if (pedStatus_[d.first].second[i] != 0) {
        pedStatus_[d.first].second[i]--;
      } else if (pedStatus_[d.first].second[i] == 0) {
        if (getNeighbour(peds_[d.first].second[i]->getDir())
                ->addPed(std::move(peds_[d.first].second[i])) == 1) {
          removePedFromI(i, d.first, false);
          i--;
        }
        break;
      }
    }
  }
}

std::vector<Direction>
Intersection::jalanEnsin(std::vector<Direction> dirs,
                         std::vector<Direction> &finalDirs) {
  std::vector<Direction> newDirs;
  for (auto &d : dirs) {
    if (!cars_[d][0])
      continue;
    if (peds_[d].first.size() == 0 &&
        peds_[cars_[d][0]->getDir()].second.size() == 0) {
      newDirs.push_back(d);
      continue;
    } else if (peds_[d].first.size() > 0 ||
               peds_[cars_[d][0]->getDir()].second.size() > 0) {
      status_[d] = REACTION_TIME2;
      continue;
    }
  }
  return newDirs;
}

// Oikealta tulevaa väistetään
std::vector<Direction>
Intersection::vasenVaistaa(std::vector<Direction> goDirs,
                           std::vector<Direction> &finalDirs) {
  std::vector<Direction> newGoDirs;
  for (auto &d : goDirs) {
    if (!cars_[d][0])
      continue;
    Direction dir = static_cast<Direction>((static_cast<int>(d) + 3) % 4);
    if (cars_[d][0]->getDir() == dir && cars_.count(dir) && !cars_[dir][1]) {
      finalDirs.push_back(d);
      continue;
    } else if (cars_.count(dir) && !cars_[dir][0] &&
               !cars_[cars_[d][0]->getDir()][1]) {
      newGoDirs.push_back(d);
      continue;
    } else if (cars_.count(dir) && cars_[dir][0]) {
      status_[d] = REACTION_TIME2;
      continue;
    } else if (!cars_.count(dir) && !cars_[cars_[d][0]->getDir()][1]) {
      newGoDirs.push_back(d);
    }
  }
  return newGoDirs;
}

// Vasemmalle kääntyvät pois mikäli joku menee suoraan
std::vector<Direction>
Intersection::ristiinVaistaa(std::vector<Direction> goDirs,
                             std::vector<Direction> &finalDirs) {
  std::vector<Direction> newGoDirs;
  Direction dir1 =
      static_cast<Direction>((static_cast<int>(goDirs[1]) + 3) % 4);
  Direction dir2 =
      static_cast<Direction>((static_cast<int>(goDirs[0]) + 3) % 4);

  if (cars_[goDirs[1]][0] && (goDirs[0] == cars_[goDirs[1]][0]->getDir()) &&
      (!cars_[cars_[goDirs[1]][0]->getDir()][1])) {
    finalDirs.push_back(goDirs[1]);
  } else {
    status_[goDirs[1]] = REACTION_TIME2;
  }

  if (cars_[goDirs[0]][0] && (goDirs[1] == cars_[goDirs[0]][0]->getDir()) &&
      (!cars_[cars_[goDirs[0]][0]->getDir()][1])) {
    finalDirs.push_back(goDirs[0]);
  } else {
    status_[goDirs[0]] = REACTION_TIME2;
  }

  if (finalDirs.size() == 0 && getCarCount() == 2) {
    finalDirs.push_back(goDirs[0]);
    finalDirs.push_back(goDirs[1]);
    return finalDirs;
  }
  return newGoDirs;
}

void Intersection::simulateTasaArvo() {
  std::vector<Direction> goDirs; // Suunnat joista voi liikkua
  std::vector<Direction> finalDirs;
  for (auto &c : cars_) {
    if (c.second[0]) {
      goDirs.push_back(c.first);
    }
  }

  simulatePeds();

  // Poistetaan suunnat väistämisvelvollisuuksien mukaan

  goDirs = jalanEnsin(goDirs, finalDirs);
  if (goDirs.size() > 1) {
    goDirs = vasenVaistaa(goDirs, finalDirs);
  }
  if (goDirs.size() > 1) {
    goDirs = ristiinVaistaa(goDirs, finalDirs);
  }
  if (goDirs.size() == 1) {
    finalDirs.push_back(goDirs[0]);
  }
  if (finalDirs.size() == 0) {
    // joku liikkuu
    for (auto &d : cars_) {
      if (d.second[0] && !cars_[d.second[0]->getDir()][1]) {
        finalDirs.push_back(d.first);
      }
    }
    if (finalDirs.size() == 0) {
      finalDirs.push_back(cars_.begin()->first);
    }
    lastLightChange_++;
    if (lastLightChange_ >= JAM_LENGTH) {
      for (auto d : finalDirs) {
        status_[d] = 0;
      }
      lastLightChange_ = 0;
    }
  } else {
    lastLightChange_ = 0;
  }

  // Liikuta autot jotka voi liikkua
  for (auto d : finalDirs) {
    if (cars_[d][0] && !status_[d]) {
      if (cars_.count(cars_[d][0]->getDir()) &&
          !cars_[cars_[d][0]->getDir()][1]) {
        cars_[cars_[d][0]->getDir()][1] = std::move(cars_[d][0]);
        cars_[d][0].reset();
      } else if (!cars_.count(cars_[d][0]->getDir())) {
        getNeighbour(cars_[d][0]->getDir())->addCar(std::move(cars_[d][0]));
      }
    } else if (cars_[d][0]) {
      status_[d]--;
    } else {
      status_[d] = REACTION_TIME;
    }
  }
}

void Intersection::simulateLiikennevalo() {
  for (auto &c : cars_) {
    if (c.second[0]) {
      c.second[0]->simulate(coordinates_);
    }
  }

  if (cars_.count(getActiveDir()) > 0 && cars_[getActiveDir()][0]) {
    Direction dest = cars_[getActiveDir()][0]->getDir();
    if (!cars_[dest][1]) {
      cars_[dest][1] = std::move(cars_[getActiveDir()][0]);
    }
  } else if (getActiveDir() == Direction::None) {
    simulatePeds();
  }
}

Direction Intersection::getActiveDir() {
  for (auto &d : status_) {
    if (!d.second) {
      return d.first;
    }
  }
  return Direction::None;
}

void Intersection::removePedFromI(int i, Direction dir, bool input) {
  if (input) {
    auto it = peds_[dir].first.begin();
    std::advance(it, i);
    peds_[dir].first.erase(it);
    auto it2 = pedStatus_[dir].first.begin();
    std::advance(it2, i);
    pedStatus_[dir].first.erase(it2);
  } else {
    auto it = peds_[dir].second.begin();
    std::advance(it, i);
    peds_[dir].second.erase(it);
    auto it2 = pedStatus_[dir].second.begin();
    std::advance(it2, i);
    pedStatus_[dir].second.erase(it2);
  }
}

std::vector<double> Intersection::getStats() const {
  std::vector<double> dailyStats;
  std::vector<double> stats = stats_.getTotalCars();
  std::vector<int> hourCounters = city_.getHourCounters();
  for (int i = 0; i < 24; i++) {
    if (hourCounters[i] > 0) {
      dailyStats.push_back(stats[i] / hourCounters[i]);
    } else {
      dailyStats.push_back(0);
    }
  }
  return dailyStats;
}
