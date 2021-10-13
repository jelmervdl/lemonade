#include <iostream>
#include <list>
#include <optional>
#include <sstream>
#include <string>

#include "3rd_party/CLI/CLI.hpp"
#include "3rd_party/Simple-WebSocket-Server/client_ws.hpp"
#include "3rd_party/Simple-WebSocket-Server/server_ws.hpp"

#include "data.h"
#include "json_interop.h"
#include "model_inventory.h"
#include "model_manager.h"
#include "translator/definitions.h"

#include "translator/parser.h"
#include "translator/service.h"
#include "translator/translation_model.h"

#include "translator.h"

std::string readFromStdin() {
  std::stringstream readStream;
  readStream << std::cin.rdbuf();
  std::string source = readStream.str();
  return source;
}

int main(int argc, char **argv) {
  using App = CLI::App;
  App app;
  std::string source, target;
  app.add_option("-s,--source", source, "Source language")->required();
  app.add_option("-t,--target", target, "Target language")->required();
  app.parse(argc, argv);
  Translator translator(/*maxModels=*/4, /*numWorkers=*/40);
  std::string input = readFromStdin();
  Response response = translator.btranslate(std::move(input), source, target);

  std::cout << toJSON<Response>(response) << std::endl;
  return 0;
}
