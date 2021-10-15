#include "3rd_party/CLI/CLI.hpp"
#include "lemonade/lib/translation_server.h"

int main(int argc, char **argv) {
  using App = CLI::App;
  size_t port;
  App app;
  app.add_option("-p,--port", port, "Port to run websocket server on")
      ->required();
  app.parse(argc, argv);

  lemonade::TranslationServer server(port);
  server.run();
  return 0;
}
