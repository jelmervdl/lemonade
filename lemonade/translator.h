#pragma once
#include <optional>
#include <string>

#include "3rd_party/CLI/CLI.hpp"
#include "3rd_party/Simple-WebSocket-Server/server_ws.hpp"

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

class TranslationServer {
public:
  using WSServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
  TranslationServer(size_t port)
      : translator_(/*maxModels=*/4, /*numWorkers=*/4) {
    server_.config.port = 1618;

    auto &endpoint = server_.endpoint["^/?$"];

    endpoint.on_message = [this](
                              std::shared_ptr<WSServer::Connection> connection,
                              std::shared_ptr<WSServer::InMessage> message) {
      // Get input text
      std::cerr << "Message received" << std::endl;
      std::string inputText = message->string();
      std::cerr << "Message: " << inputText << std::endl;
      auto callback = [connection](marian::bergamot::Response &&response) {
        std::string outputText = response.target.text;

        auto sendStream = std::make_shared<WSServer::OutMessage>();
        // Translate
        // timer::Timer timer;
        *sendStream << outputText << std::endl;
        // if (!quiet)
        //   LOG(info, "Translation took: {:.5f}s", timer.elapsed());

        // Send translation back
        connection->send(sendStream, [](const SimpleWeb::error_code &error) {
          if (error)
            std::cerr << fmt::format("Error sending message: ({}) {}\n",
                                     error.value(), error.message());
        });
      };

      translator_.translate(inputText, "English", "German", callback);
    };

    // Error Codes for error code meanings
    // http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/reference.html
    endpoint.on_error = [](std::shared_ptr<WSServer::Connection> /*connection*/,
                           const SimpleWeb::error_code &error) {
      std::cerr << fmt::format("Connection error: ({}) {}\n", error.value(),
                               error.message());
    };
  }

  // Start server thread
  void run() {
    server_.start([](unsigned short port) {
      LOG(info, "TranslationServer is listening on port {}", port);
    });
  }

private:
  WSServer server_;
  Translator translator_;
};