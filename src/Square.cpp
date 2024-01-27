#include "Square.hpp"
#include "City.hpp"

Square *Square::getNeighbour(Direction dir) {
  size_t x = coordinates_.x;
  size_t y = coordinates_.y;
  try {
    switch (dir) {
    case Direction::N:
      return city_.getSquare(Position(x, y - 1));
    case Direction::E:
      return city_.getSquare(Position(x + 1, y));
    case Direction::S:
      return city_.getSquare(Position(x, y + 1));
    case Direction::W:
      return city_.getSquare(Position(x - 1, y));
    case Direction::None:
      return nullptr;
    }
  } catch (std::out_of_range &e) {
    std::cout << "Out of range" << std::endl;
    return nullptr;
  }
  return this;
}

Square &Square::operator=(const Square &other) {
  coordinates_ = other.coordinates_;
  city_ = other.city_;
  return *this;
}