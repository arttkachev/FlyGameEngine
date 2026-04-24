#include <iostream>
#include "Core/Launch.h"
#include <stdexcept>
#include "cstdlib"

int main() {
  Engine::Core::Launch engineLoop{};

  try {
    engineLoop.run();
  }
  catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}