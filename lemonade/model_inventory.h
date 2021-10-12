#pragma once
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include <cstdio>
#include <cstdlib>
#include <optional>
#include <string>
#include <utility>

#define LEMONADE_ABORT_IF(cond, message)                                       \
  do {                                                                         \
    if ((cond)) {                                                              \
      std::cerr << message << std::endl;                                       \
      std::abort();                                                            \
    }                                                                          \
  } while (0)

class ModelInventory {
public:
  using LanguageDirection = std::pair<std::string, std::string>;

  struct ModelInfo {
    std::string name;
    std::string type;
    LanguageDirection direction;
  };

  ModelInventory(const std::string &modelsJSON, const std::string &modelsDir)
      : modelsJSON_(modelsJSON), modelsDir_(modelsDir) {
    std::cout << modelsJSON_ << std::endl;

    inventory_ = readInventoryFromDisk();

    LEMONADE_ABORT_IF(!inventory_.HasMember("models"), "No models found");
    const rapidjson::Value &models = inventory_["models"];

    for (size_t i = 0; i < models.Size(); i++) {
      const rapidjson::Value &entry = models[i];
      // std::cout << i << " " << entry.IsObject() << std::endl;
      // std::cout << entry.HasMember("shortName") << std::endl;
      std::cout << "Parsing " << entry["name"].GetString() << std::endl;
      std::string type = entry["type"].GetString();
      if (type == "tiny") {
        std::cout << "Inserting " << entry["src"].GetString() << "-"
                  << entry["trg"].GetString() << std::endl;
        LanguageDirection direction =
            std::make_pair(entry["src"].GetString(), entry["trg"].GetString());

        languageDirections_[direction] =
            ModelInfo{/*name=*/
                      entry["name"].GetString(),
                      /*type=*/entry["type"].GetString(),
                      /*direction=*/direction};
      }
    }
  }

  std::optional<ModelInfo> query(const std::string &source,
                                 const std::string &target) const {
    auto query = languageDirections_.find(LanguageDirection{source, target});
    if (query != languageDirections_.end()) {
      return query->second;
    }
    return std::nullopt;
  }

private:
  struct Hash {
    size_t operator()(const LanguageDirection &direction) const {
      auto hash_combine = [](size_t &seed, size_t next) {
        seed ^=
            std::hash<size_t>{}(next) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      };

      size_t seed = std::hash<std::string>{}(direction.first);
      hash_combine(seed, std::hash<std::string>{}(direction.second));
      return seed;
    }
  };

  std::unordered_map<LanguageDirection, ModelInfo, Hash> languageDirections_;
  rapidjson::Document inventory_;
  std::string modelsDir_;
  std::string modelsJSON_;

  rapidjson::Document readInventoryFromDisk() {
    FILE *fp = fopen(modelsJSON_.c_str(), "r"); // non-Windows use "r"
    char readBuffer[65536];
    rapidjson::FileReadStream fReadStream(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document d;
    d.ParseStream(fReadStream);
    fclose(fp);
    return d;
  }
};
