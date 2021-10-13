#pragma once
#include "3rd_party/Simple-WebSocket-Server/client_ws.hpp"
#include "3rd_party/Simple-WebSocket-Server/server_ws.hpp"

class TranslationClient {
public:
  TranslationClient(const std::string &addr) : client_(addr) {}
  void run();

private:
  // Warning, inheritance is not meant for code-reuse. But well, I don't follow
  // rules.
  class WsClient : public SimpleWeb::SocketClient<SimpleWeb::WS> {
  public:
    WsClient(const std::string &addr)
        : SimpleWeb::SocketClient<SimpleWeb::WS>(addr) {}
    // This will bite later. We'll fix when it does.
    // https://gitlab.com/eidheim/Simple-WebSocket-Server/-/issues/94#note_324545902
    void send_message(std::string msg_to_send) {
      SimpleWeb::LockGuard lock(connection_mutex);
      connection->send(msg_to_send);
    }
  };

  WsClient client_;
};
