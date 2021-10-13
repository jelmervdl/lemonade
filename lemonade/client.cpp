#include "3rd_party/CLI/CLI.hpp"
#include "translator.h"

int main(int argc, char **argv) {
  using App = CLI::App;
  App app;
  std::string url;
  app.add_option("-u,--url", url, "URL server is listening to")->required();
  app.parse(argc, argv);
  TranslationClient client(url);
  client.run();
  return 0;
}
