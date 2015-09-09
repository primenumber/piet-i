#include <iostream>
#include "visualize.hpp"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " [PNG FILENAME]" << std::endl;
    return EXIT_FAILURE;
  }
  try {
    auto image = Image(argv[1]);
    visualize(image);
  } catch (png::error& e) {
    std::cerr << e.what() << std::endl;
  }
  return EXIT_SUCCESS;
}
