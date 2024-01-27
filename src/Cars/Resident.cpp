#include "Resident.hpp"

#include <time.h>

#include "../Buildings/Building.hpp"
#include "../City.hpp"
// Random numero generaattori. Ottaa aloitusluvun (offset), sekä lukumäärän
// (range) ja antaa random numeron siltä väliltä. Esim jos offset on 2 ja range
// 9 niin antaa jonkun numeron 2 ja 10 välillä..
int random(int offset, int range) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(offset, offset + range - 1);
  return dis(gen);
}

int normalRandom(int offset, int range) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::normal_distribution<double> distribution(offset, range);
  double randomValue = distribution(gen);

  return randomValue;
}

// constructori
Resident::Resident(City &city, Position currentPos, int id)
    : city_(city),
      currentPos_(currentPos),
      home_(currentPos),
      work_(currentPos),
      favShop_(currentPos),
      randShop_(currentPos),
      shop_(currentPos),
      destination_(currentPos),
      id_(id),
      shopCommuteTime(0),
      favShopCommuteTime(0),
      onTheMove(false),
      siesta(!random(0, 2)),
      party(!random(0, 2)),
      workBegin((double)normalRandom(8, 1)),
      workEnd(workBegin + 8),
      workBeginAdd((50 - (double)random(0, 100)) / 100.0),
      workEndAdd((50 - (double)random(0, 100)) / 100.0),
      night(random(0, 4) ? 0.0 : 12.0) {
  // tää arpoo työpaikan
  work_ =
      city_.getIndustrials().empty()
          ? currentPos
          : city_
                .getIndustrials()[random(
                    0, (int)city_.getIndustrials().size())]  // otettu -1 pois
                ->getCoordinates();
  // tää arpoo lempparikaupan
  favShop_ =
      city_.getCommercials().empty()
          ? currentPos
          : city_
                .getCommercials()[random(
                    0, (int)city_.getCommercials().size())]  // otettu -1 pois
                ->getCoordinates();
  // tää arpoo jonkun random kaupan
  randShop_ =
      city_.getCommercials().empty()
          ? currentPos
          : city_
                .getCommercials()[random(
                    0, (int)city_.getCommercials().size())]  // otettu -1 pois
                ->getCoordinates();
  shop_ = randShop_;
}

// vertailufunktiot vertaa vaan id:tä koska kaikilla on ikioma id :)
bool Resident::operator==(Resident &r) {
  return id_ == r.getId() ? true : false;
}

bool Resident::operator!=(Resident &r) { return !(*this == r); }

// poistumisfunktio. Kun kaveri haluu lähteä jonnekkin se kutsuu tätä.
// Jos seuraava määränpää on joku muu kun nykyinen nii kutsuu nykysen
// rakennuksen poistu-funktiota
bool Resident::leave(const Position nextPos) {
  if (nextPos != currentPos_) {
    Building *currentBuilding =
        dynamic_cast<Building *>(city_.getSquare(currentPos_));
    if (currentBuilding) {
      destination_ = nextPos;
      findPath();
      if (path.empty()) {
        return false;
      }
      int rng = (int)normalRandom(20, 3);
      if (path.size() < rng) {
        currentBuilding->removeResident(id_, nextPos, 0);
      } else {
        currentBuilding->removeResident(id_, nextPos, 1);
      }
      onTheMove = true;
      return true;
    }
  }
  return false;
}

Direction Resident::updateDir() {
  ii++;
  currentDir = path[ii];
  return currentDir;
}

const Direction Resident::getDir() const { return currentDir; }

Direction Resident::peekDir() const { return path[ii + 1]; }

int Resident::findPath() {
  // Tarvittavat alustukset

  const int nStartX = currentPos_.x;
  const int nStartY = currentPos_.y;

  const int nTargetX = destination_.x;
  const int nTargetY = destination_.y;

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
  const int startPos = idx(currentPos_.x, currentPos_.y),
            targetPos = idx(destination_.x, destination_.y);

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
        case Direction::None:
          std::cout << "-";
          break;
      }
    }
    std::cout << std::endl;*/
    currentDir = path[0];  // LISÄYS t.Valtteri iik

    return d[targetPos];
  }
}

/*
Päättää mitä tyyppi aikoo seuraavaksi tehdä. Aina kello 00 arpoo uudet jutut,
joiden perusteella seuraava päivä hoidetaan. Atm jokainen päivä on tylsä hima -
työ - kauppa - hima sykli. Työ ja Hima pysyy samana, mutta lähtöajat vaihtelee
tunnilla kaverin laiskuudesta (rng) riippuen. Kaupan kanssa on 2/3 chäänssi et
äijänkäppänä menee lempikauppaan ja 1/3 et kokeilee uutta. Lojaaliksi tämän
kylän asukkia ei kuitenkaan voi kutsua, sillä jos matka random kauppaan
vierähtää nopeammin kuin lempparikauppaan, tulee kyseisestä random kaupasta uusi
lemppari.
*/
void Resident::simulate(Position cur) {
  currentPos_ = cur;
  if (city_.getTime() == 0.0) {
    workBeginAdd = (50 - (double)random(0, 100)) / 100.0;
    workEndAdd = (50 - (double)random(0, 100)) / 100.0;
    if (favShopCommuteTime > shopCommuteTime) {
      favShop_ = randShop_;
    }
    randShop_ =
        city_.getCommercials().empty()
            ? currentPos_
            : city_
                  .getCommercials()[random(
                      0, (int)city_.getCommercials().size())]  // otettu -1 pois
                  ->getCoordinates();
    if (random(0, 2)) {
      shop_ = favShop_;
      favShopCommuteTime = 0;
    } else {
      shop_ = randShop_;
      shopCommuteTime = 0;
    }
    siesta = !random(0, 2);
    party = !random(0, 2);
  }
  if (!onTheMove) {
    if (city_.getTime() == workBegin + workBeginAdd + night) {
      leave(work_);
    } else if (city_.getTime() == workBegin + workBeginAdd + 3 && siesta) {
      leave(random(0, 2) ? shop_ : home_);
    } else if (city_.getTime() == workBegin + workBeginAdd + 5 && siesta) {
      leave(work_);
    } else if (city_.getTime() == workEnd + workEndAdd - night) {
      leave(shop_);
    } else if (city_.getTime() == workEnd + workEndAdd - night + 3) {
      leave(home_);
    } else if (city_.getTime() == ((workBeginAdd < 0)? 21 - (workBeginAdd*6): workBeginAdd*6) && party){
      leave(favShop_);
    } else if (city_.getTime() == (3 + (workBeginAdd*6))  && party){
      leave(home_);
    }
  } else if (destination_ == randShop_) {
    shopCommuteTime++;
  } else if (destination_ == favShop_) {
    favShopCommuteTime++;
  }
}

// infofunktio kivaa infoa stringin muodossa :)
std::string Resident::info() const {
  std::stringstream o;
  o << "Resident id: " << id_ << "\nCurrent position: " << currentPos_.x << ", "
    << currentPos_.y << "\nHome at: " << home_.x << ", " << home_.y
    << "\nWork at: " << work_.x << ", " << work_.y
    << "\nPreferred shop at: " << favShop_.x << ", " << favShop_.y;
  std::string str = o.str();
  return str;
}
