#include "Car.hpp"

#include "../Buildings/Building.hpp"
#include "../City.hpp"

using namespace std;

Car::Car(City &city, const Position startPos, const Position endPos, int id)
    : city_(city), startPos_(startPos), endPos_(endPos), id_(id) {
  findPath();
  passList.resize(1);
}

Car &Car::operator=(Car &other) { return other; }

const Direction Car::getDir() const { return currentDir; }

Direction Car::peekDir() const { return path[ii + 1]; }

bool Car::addPassenger(std::unique_ptr<Resident> &KimiRaikkonen) {
  if (KimiRaikkonen->getPos().x == this->startPos_.x &&
      KimiRaikkonen->getPos().y == this->startPos_.y) {
    passList[0] = std::move(KimiRaikkonen);
    return true;
  }
  return false;
}

int Car::findPath() {
  // Tarvittavat alustukset
  const int nStartX = startPos_.x;
  const int nStartY = startPos_.y;

  const int nTargetX = endPos_.x;
  const int nTargetY = endPos_.y;

  const int nMapWidth = (int)city_.getCitySize();
  const int nMapHeight = (int)city_.getCitySize();

  City &ci = city_;
  // idx funktio tekee kaksiulotteisista koordinaateista yksiuloitteisen
  // indeksin, nopeuttaa
  auto idx = [nMapWidth](int x, int y) { return x + y * nMapWidth; };

  // alin etäisyys arvio käyttäen manhattan-etäisyyttä
  auto h = [=](int u) -> int {
    int x = u % nMapWidth, y = u / nMapWidth;
    return abs(x - nTargetX) + abs(y - nTargetY);
  };

  // alustetaan lisää
  const int n = nMapWidth * nMapHeight;
  const int startPos = idx(startPos_.x, startPos_.y),
            targetPos = idx(endPos_.x, endPos_.y);

  int discovered = 0;
  int ExploredNodes = 0;
  vector<int> p(n), d(n, INT_MAX);
  priority_queue<tuple<int, int, int>, vector<tuple<int, int, int>>,
                 greater<tuple<int, int, int>>>
      pq;  // A* with tie breaking
  d[startPos] = 0;
  pq.push(make_tuple(0 + h(startPos), 0, startPos));
  // päälooppi, katsoo lähimmät squaret käyttäen priority queueta.
  while (!pq.empty()) {
    int u = get<2>(pq.top());
    pq.pop();
    ExploredNodes++;
    for (auto e : {+1, -1, +nMapWidth, -nMapWidth}) {
      int v = u + e;
      if ((e == 1 && (v % nMapWidth == 0)) || (e == -1 && (u % nMapWidth == 0)))
        continue;
      if (0 <= v && v < n && d[v] > d[u] + 1 &&
          (ci.getSquare(Position(v % nMapWidth, v / nMapWidth))->isRoad() ||
           Position(v % nMapWidth, v / nMapWidth) ==
               (Position(nTargetX, nTargetY)) ||
           Position(v % nMapWidth, v / nMapWidth) ==
               (Position(nStartX, nStartY)))) {
        p[v] = u;
        d[v] = d[u] + 1;
        if (v == targetPos) goto end;
        pq.push(make_tuple(d[v] + h(v), ++discovered, v));
      }
    }
  }
end:

  // laitetaan arvot path-vektoriin
  if (d[targetPos] == INT_MAX) {
    return -1;
  } else {
    int curr = targetPos;
    for (int i = d[targetPos] - 1; i >= 0; i--) {
      int prev = p[curr];
      int dx = (curr % nMapWidth) - (prev % nMapWidth);
      int dy = (curr / nMapWidth) - (prev / nMapWidth);

      if (dx == 1) {
        path.push_back(Direction::E);
      } else if (dx == -1) {
        path.push_back(Direction::W);
      } else if (dy == 1) {
        path.push_back(Direction::S);
      } else if (dy == -1) {
        path.push_back(Direction::N);
      }

      curr = prev;
    }
    size_t size = path.size();
    for (size_t i = 0; i < size / 2; ++i) {
      std::swap(path[i], path[size - 1 - i]);
    }
    /*for (int i = 0; i < path.size(); i++) {
      switch (path[i]) {
        case Direction::N:
          std::cout << "N";
          break;
        case Direction::E:
          std::cout << "E";
          break;
        case Direction::S:
          std::cout << "S";
          break;
        case Direction::W:
          std::cout << "W";
          break;
      }
    }
    std::cout << std::endl;*/
    currentDir = path[0];  // LISÄYS t.Valtteri iik

    return d[targetPos];
  }
}

std::unique_ptr<Resident> Car::getPassenger() {
  if (!passList.empty()) {
    return std::move(passList[0]);
  } else {
    return nullptr;
  }
}

void Car::createPath(std::vector<Direction> &path_) {
  path = path_;
  currentDir = path[0];
}

Direction Car::updateDir() {
  ii++;
  currentDir = path[ii];
  return currentDir;
}

Car::~Car() {
  Building *currentBuilding =
      dynamic_cast<Building *>(city_.getSquare(endPos_));
  if (currentBuilding) {
    for (int i = 0; i < passList.size(); i++) {
      passList[i]->enter();
      currentBuilding->addResident(passList[i]);
    }
  }
}

void Car::simulate(Position cur) {
  for (int i = 0; i < passList.size(); i++) {
    passList[i]->simulate(cur);
  }
}