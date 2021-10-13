#pragma once

#include "3rd_party/Simple-WebSocket-Server/server_ws.hpp"
#include "translator.h"

class TranslationServer {
public:
  using WSServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
  TranslationServer(size_t port);

  // Start server thread
  void run();

private:
  WSServer server_;
  Translator translator_;
};
