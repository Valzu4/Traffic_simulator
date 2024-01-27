#include "Residential.hpp"

void Residential::initSquare() {
  residents_.clear();
  for (int i = 0; i < capacity_; i++) {
    // Id ei pitäisi olla i vaan jotain muuta
    newResident(city_, coordinates_, city_.newResidentId());
  }
}
