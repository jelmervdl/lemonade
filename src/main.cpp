
#include "CLI/CLI11.hpp"
#include "rapidjson/document.h"
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

class TranslationModel;

/// Manages models
class ModelManager {
  using Container = std::list;
  using Ptr = std::shared_ptr;

public:
  ModelManager(size_t maxModels) : maxModels_(maxModels) {}

  bool loadModel(const TranslationModel::config &config) {
    if (models_.size() == maxModels_) {
      return false;
    }
    models_.emplace(models_.end(), config);
    return true;
  };

private:
  size_t maxModels_{0};
  Container<Ptr<TranslationModel>> models_;

  struct Usage {
    const TranslationModel *model{nullptr};
    size_t usage{0};
  };

  std::priority_queue<Usage, decltype([](const Usage &a, const Usage &b) {
                        return a.usage < b.usage;
                      })>
      accessLog_;
};

std::ostream &operator<<(std::ostream &out, const Config &config) {
  out << config.source << "->" << config.target;
  return out;
}

int main(int argc, char **argv) {
  CLI::App app;
  Config config;
  app.add_option("-s,--source", config.source, "Source language")->required();
  app.add_option("-t,--target", config.target, "Target language")->required();
  app.parse(argc, argv);
  std::cout << config << std::endl;
  return 0;
}
