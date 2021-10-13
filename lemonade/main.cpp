#include <iostream>
#include <list>
#include <optional>
#include <sstream>
#include <string>

#include "3rd_party/CLI/CLI.hpp"

#include "model_inventory.h"
#include "model_manager.h"

#include "translator/parser.h"
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

  using Model = std::shared_ptr<marian::bergamot::TranslationModel>;

  Model model;
  if (modelInfo) {
    ModelInventory::ModelInfo m = modelInfo.value();
    auto modelConfig =
        marian::bergamot::parseOptionsFromFilePath(inventory.configFile(m));
    model = service.createCompatibleModel(modelConfig);

    std::stringstream readStream;
    readStream << std::cin.rdbuf();
    std::string source = readStream.str();

    using marian::bergamot::Response;
    using marian::bergamot::ResponseOptions;

    std::promise<Response> responsePromise;
    auto responseFuture = responsePromise.get_future();

    auto callback = [&responsePromise](Response &&response) {
      responsePromise.set_value(std::move(response));
    };

    ResponseOptions responseOptions;
    service.translate(model, std::move(source), callback, responseOptions);

    responseFuture.wait();
    Response response = std::move(responseFuture.get());

    std::cout << response.target.text << "\n";
  }

  return 0;
}
