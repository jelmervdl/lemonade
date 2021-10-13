#include "model_inventory.h"
#include "fmt/core.h"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <string>
#include <utility>

ModelInventory::ModelInventory(const std::string &modelsJSON,
                               const std::string &modelsDir)
    : modelsJSON_(modelsJSON), modelsDir_(modelsDir) {
  // std::cout << modelsJSON_ << std::endl;

  inventory_ = readInventoryFromDisk();

  LEMONADE_ABORT_IF(!inventory_.HasMember("models"), "No models found");
  const rapidjson::Value &models = inventory_["models"];

  for (size_t i = 0; i < models.Size(); i++) {
    const rapidjson::Value &entry = models[i];
    // std::cout << i << " " << entry.IsObject() << std::endl;
    // std::cout << entry.HasMember("shortName") << std::endl;
    // std::cout << "Parsing " << entry["name"].GetString() << std::endl;
    std::string type = entry["type"].GetString();
    if (type == "tiny") {
      // std::cout << "Inserting " << entry["src"].GetString() << "-"
      //           << entry["trg"].GetString() << std::endl;
      LanguageDirection direction =
          std::make_pair(entry["src"].GetString(), entry["trg"].GetString());

      languageDirections_[direction] =
          ModelInfo{/*name=*/
                    entry["name"].GetString(),
                    /*type=*/entry["type"].GetString(),
                    /*code=*/entry["code"].GetString(),
                    /*direction=*/direction};
    }
  }
}

std::optional<ModelInventory::ModelInfo>
ModelInventory::query(const std::string &source,
                      const std::string &target) const {
  auto query = languageDirections_.find(LanguageDirection{source, target});
  if (query != languageDirections_.end()) {
    return query->second;
  }
  return std::nullopt;
}

size_t
ModelInventory::Hash::operator()(const LanguageDirection &direction) const {
  auto hash_combine = [](size_t &seed, size_t next) {
    seed ^= std::hash<size_t>{}(next) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  };

  size_t seed = std::hash<std::string>{}(direction.first);
  hash_combine(seed, std::hash<std::string>{}(direction.second));
  return seed;
}

rapidjson::Document ModelInventory::readInventoryFromDisk() {
  FILE *fp = fopen(modelsJSON_.c_str(), "r"); // non-Windows use "r"
  char readBuffer[65536];
  rapidjson::FileReadStream fReadStream(fp, readBuffer, sizeof(readBuffer));
  rapidjson::Document d;
  d.ParseStream(fReadStream);
  fclose(fp);
  return d;
}

std::string ModelInventory::configFile(const ModelInfo &modelInfo) {
  std::string configFilePath =
      fmt::format("{}/{}/config.bergamot.yml", modelsDir_, modelInfo.code);
  return configFilePath;
}