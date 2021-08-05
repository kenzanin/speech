#include <iostream>

#include "speech.hpp"

int main(int, char** argc) {
  std::cout << PitchAnalyzer2(argc[1]) << "\n";
  return {};
}
