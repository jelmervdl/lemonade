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

class Translator {

public:
  Translator(size_t maxModels, size_t numWorkers)
      : manager_(maxModels), config_{numWorkers}, service_(config_),
        inventory_(modelsJSON(), modelsDir()) {}

  void translate(std::string input, const std::string &source,
                 const std::string &target,
                 marian::bergamot::CallbackType callback) {
    std::optional<ModelInventory::ModelInfo> modelInfo =
        inventory_.query(source, target);

    if (modelInfo) {
      ModelInventory::ModelInfo m = modelInfo.value();
      Model model = manager_.lookup(m.code);
      if (!model) {
        auto modelConfig = marian::bergamot::parseOptionsFromFilePath(
            inventory_.configFile(m));

        // FIXME
        modelConfig->set("workspace", 128);

        marian::timer::Timer timer;
        marian::bergamot::MemoryBundle memoryBundle =
            marian::bergamot::getMemoryBundleFromConfig(modelConfig);
        model = service_.createCompatibleModel(modelConfig,
                                               std::move(memoryBundle));

        manager_.cacheModel(m.code, model);

        std::cout << fmt::format("Model building took {} seconds.",
                                 timer.elapsed());
      }

      marian::bergamot::ResponseOptions responseOptions;
      service_.translate(model, std::move(input), callback, responseOptions);
    }
  }

  marian::bergamot::Response btranslate(std::string input,
                                        const std::string &source,
                                        const std::string &target) {

    using Response = marian::bergamot::Response;
    std::promise<Response> p;
    std::future<Response> f = p.get_future();
    auto callback = [&p](Response &&response) {
      p.set_value(std::move(response));
    };

    translate(std::move(input), source, target, callback);
    f.wait();
    return f.get();
  }

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
