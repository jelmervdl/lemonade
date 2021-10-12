
#include "3rd_party/CLI/CLI.hpp"
#include "rapidjson/document.h"
#include "translator/translation_model.h"
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>

struct Config {
  std::string source;
  std::string target;
};

class ModelInventory {
public:
  ModelInventory() {
    const char *configDir = std::getenv("XDG_CONFIG_HOME");
    const char *modelDir = std::getenv("HOME");
  }

  ~ModelInventory() {}

private:
  std::string modelDir_;  /// < Where models are stored.
  std::string modelJSON_; /// < A JSON file listing the inventory of models.
};

/// Manages models
class ModelManager {
  using Model = marian::bergamot::TranslationModel;

public:
  ModelManager(size_t maxModels) : maxModels_(maxModels) {}

  bool loadModel(const Model::Config &config) {
    if (models_.size() == maxModels_) {
      return false;
    }
    models_.emplace(models_.end(), std::make_unique<Model>(config));
    return true;
  };

private:
  size_t maxModels_{0};
  std::list<std::shared_ptr<Model>> models_;

  struct Usage {
    const Model *model{nullptr};
    size_t usage{0};
  };
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

  inventory = ModelInventory();

  std::cout << config << std::endl;
  return 0;
}
