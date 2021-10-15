#pragma once
#include "3rd_party/Simple-WebSocket-Server/client_ws.hpp"
#include "3rd_party/Simple-WebSocket-Server/server_ws.hpp"
#include "data.h"
#include "json_interop.h"

namespace lemonade {

class TranslationClient {
public:
  TranslationClient(const std::string &addr, const std::string &source,
                    const std::string &target)
      : client_(addr), source_(source), target_(target) {}
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
    void translate(const std::string &source, const std::string &target,
                   const std::string &query) {
      Payload payload{source, target, query};
      std::string payloadAsString = toJSON<Payload>(payload);
      SimpleWeb::LockGuard lock(connection_mutex);
      connection->send(payloadAsString);
    }
  };

  WsClient client_;

  const std::string source_;
  const std::string target_;
};

} // namespace lemonade
