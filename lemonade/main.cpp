#include <iostream>
#include <list>
#include <optional>
#include <sstream>
#include <string>

#include "3rd_party/CLI/CLI.hpp"

#include "model_inventory.h"
#include "model_manager.h"

#include "translator/service.h"
#include "translator/translation_model.h"

struct Config {
  std::string source;
  std::string target;
};

int main(int argc, char **argv) {
  using App = CLI::App;
  App app;
  Config config;
  app.add_option("-s,--source", config.source, "Source language")->required();
  app.add_option("-t,--target", config.target, "Target language")->required();
  app.parse(argc, argv);

  ModelManager manager(/*maxModels=*/2);

  using Service = marian::bergamot::AsyncService;
  Service::Config serviceConfig;
  serviceConfig.numWorkers = 4;

  Service service(serviceConfig);

  ModelInventory inventory(
      /*modelsJSON=*/"/home/jphilip/.config/lemonade/models.json",
      /*modelsDir=*/"/home/jphilip/.lemonade/models");

  std::optional<ModelInventory::ModelInfo> modelInfo =
      inventory.query(config.source, config.target);

  if (modelInfo) {
    ModelInventory::ModelInfo m = modelInfo.value();
    std::cout << m.name << std::endl;
  }

  std::stringstream readStream;
  readStream << std::cin.rdbuf();

  std::cout << readStream.str();

  return 0;
}
