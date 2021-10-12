#include "3rd_party/CLI/CLI.hpp"
#include "rapidjson/document.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "translator/translation_model.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <list>
#include <string>

std::filesystem::path getConfigPath() {
  return std::filesystem::path("/home/jphilip/.config/lemonade/models.json");
  std::filesystem::path configpath;
  if (std::getenv("HOME")) {
    return (std::filesystem::path(std::string(std::getenv("HOME"))) /
            ".config" / "lemonade" / "models.json");
  } else {
    return std::filesystem::path("/home/jphilip");
  }
}

class ModelInventory {
public:
  using Path = std::filesystem::path;

  ModelInventory() : modelJSON_(getConfigPath()) {}

  void load() {
    std::cout << "ModelInventory: Constructor" << std::endl;
    std::cout << modelJSON_.string() << std::endl;

    std::ifstream jsonFstream(modelJSON_.string());

    if (!jsonFstream.is_open()) {
      std::cerr << "Could not open file for reading!\n";
    }

    rapidjson::IStreamWrapper jsonFstreamWrapper{jsonFstream};

    rapidjson::Document document;
    document.ParseStream(jsonFstreamWrapper);

    // 3. Stringify the DOM
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    if (document.HasParseError()) {
      std::cout << "Error  : " << document.GetParseError() << '\n'
                << "Offset : " << document.GetErrorOffset() << '\n';
    }

    // Output {"project":"rapidjson","stars":11}
    std::cout << buffer.GetString() << std::endl;
  }

private:
  Path modelDir_;  /// < Where models are stored.
  Path modelJSON_; /// < A JSON file listing the inventory of models.
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
  ModelInventory inventory;
  inventory.load();
  std::cout << "End of program! " << std::endl;
  return 0;
}
