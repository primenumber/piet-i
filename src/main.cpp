#include <fstream>
#include <iostream>
#include "visualize.hpp"
#include "interpret.hpp"
#include "color_blocks.hpp"
#include "basic_blocks.hpp"

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "usage: " << argv[0] << " [PNG FILENAME] [CODEL SIZE]" << std::endl;
    return EXIT_FAILURE;
  }
  try {
    Image image(argv[1]);
    CodelTable table(image, std::stoi(argv[2]));
    ColorBlockGraph graph(table);
    CommandGraph cg(graph);
    BasicBlockGraph bbg(cg);
    std::cerr << "Compile Completed" << std::endl;
    std::cout << bbg << std::flush;
  } catch (png::error& e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
