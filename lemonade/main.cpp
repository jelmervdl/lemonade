#include "3rd_party/CLI/CLI.hpp"
#include "translator/translation_model.h"
#include <list>
#include <string>

#include "model_inventory.h"
#include "model_manager.h"

struct Config {
  std::string source;
  std::string target;
};

std::ostream &operator<<(std::ostream &out, const Config &config) {
  out << config.source << "->" << config.target;
  return out;
}

int main(int argc, char **argv) {
  using App = CLI::App;
  App app;
  Config config;
  app.add_option("-s,--source", config.source, "Source language")->required();
  app.add_option("-t,--target", config.target, "Target language")->required();
  app.parse(argc, argv);

  std::cout << config << std::endl;
  ModelInventory inventory(
      /*modelsJSON=*/"/home/jphilip/.config/lemonade/models.json",
      /*modelsDir=*/"/home/jphilip/.lemonade/models");

  std::cout << "End of program! " << std::endl;
  return 0;
}
