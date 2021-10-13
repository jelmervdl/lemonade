#pragma once
#include <optional>
#include <string>

#include "3rd_party/CLI/CLI.hpp"

#include "data.h"
#include "json_interop.h"
#include "model_inventory.h"
#include "model_manager.h"
#include "translator/definitions.h"

#include "translator/parser.h"
#include "translator/service.h"
#include "translator/translation_model.h"

class Translator {

public:
  Translator(size_t maxModels, size_t numWorkers)
      : manager_(maxModels), config_{numWorkers}, service_(config_),
        inventory_(/*modelsJSON=*/"/home/jphilip/.config/lemonade/models.json",
                   /*modelsDir=*/"/home/jphilip/.lemonade/models") {}

  void translate(std::string input, const std::string &source,
                 const std::string &target,
                 marian::bergamot::CallbackType callback) {
    std::optional<ModelInventory::ModelInfo> modelInfo =
        inventory_.query(source, target);

    Model model;
    if (modelInfo) {
      ModelInventory::ModelInfo m = modelInfo.value();
      auto modelConfig =
          marian::bergamot::parseOptionsFromFilePath(inventory_.configFile(m));
      model = service_.createCompatibleModel(modelConfig);

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
  using Model = std::shared_ptr<marian::bergamot::TranslationModel>;
  using Service = marian::bergamot::AsyncService;

  ModelManager manager_;
  ModelInventory inventory_;

  Service::Config config_;
  Service service_;
};
