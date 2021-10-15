#pragma once
#include <optional>
#include <string>

#include "3rd_party/CLI/CLI.hpp"

#include "data.h"
#include "json_interop.h"
#include "model_inventory.h"
#include "model_manager.h"
#include "translator/definitions.h"

#include "common/timer.h"
#include "fmt/core.h"
#include "translator/byte_array_util.h"
#include "translator/parser.h"
#include "translator/service.h"
#include "translator/translation_model.h"

namespace lemonade {

class Translator {

public:
  Translator(size_t maxModels, size_t numWorkers)
      : manager_(maxModels), config_{numWorkers}, service_(config_),
        inventory_(modelsJSON(), modelsDir()) {}

  void translate(std::string input, const std::string &source,
                 const std::string &target,
                 marian::bergamot::CallbackType callback);

  marian::bergamot::Response btranslate(std::string input,
                                        const std::string &source,
                                        const std::string &target);

private:
  static std::string modelsJSON() {
    return fmt::format("{}/.config/lemonade/models.json", home());
  }

  static std::string modelsDir() {
    return fmt::format("{}/.lemonade/models", home());
  }

  static std::string home() {
    const char *envHome = std::getenv("HOME");
    return std::string(envHome);
  }

  using Model = std::shared_ptr<marian::bergamot::TranslationModel>;
  using Service = marian::bergamot::AsyncService;

  ModelManager manager_;
  ModelInventory inventory_;

  Service::Config config_;
  Service service_;
};

} // namespace lemonade
