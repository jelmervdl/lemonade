#pragma once
#include "engine_compat.h"
#include "lemonade/lib/logging.h"
#include <ibus.h>

static g::Pointer<IBusFactory> factory = NULL;

namespace lemonade::ibus {
class Application {
public:
  Application(gboolean ibus) : logger_("application") {
    ibus_init();

    // TODO: Bus can be g::Object derived.
    bus = ibus_bus_new();

    if (!ibus_bus_is_connected(bus)) {
      logger_.log("Cannot connect to ibus!");
      g_warning("Can not connect to ibus!");
      std::abort();
    }

    if (!ibus_bus_get_config(bus)) {
      g_warning("IBus config component is not ready!");
      std::abort();
    }

    g_object_ref_sink(bus);

    auto callback = +[](IBusBus *bus, gpointer user_data) { ibus_quit(); };
    g_signal_connect(bus, "disconnected", G_CALLBACK(callback), NULL);

    logger_.log("Adding factory");
    factory = ibus_factory_new(ibus_bus_get_connection(bus));

    ibus_factory_add_engine(factory, "", IBUS_TYPE_LEMONADE_ENGINE);

    if (ibus) {
      logger_.log("ibus = true, requesting bus");
      ibus_bus_request_name(bus, "org.freedesktop.IBus.Lemonade", 0);
    } else {
      logger_.log("ibus = false, trying something else?");

      g::Pointer<IBusComponent> component;

      component = ibus_component_new(
          /*name=*/"org.freedesktop.IBus.Lemonade", /*description=*/
          "Lemonade provides client-side translation on the local *nix "
          "machine.",
          /*version=*/"", /*license=*/"GPL",
          /*uthor=*/"",
          /*homepage=*/"https://github.com/jerinphilip/lemonade",
          /*commandline=*/"", /*textdomain=*/"lemonade-ime");
      ibus_component_add_engine(
          component, ibus_engine_desc_new(
                         /*shortname=*/"lemonade",
                         /*longname=*/"Lemonade",
                         /*description=*/
                         "Lemonade provides client-side translation on the "
                         "local *nix machine.",
                         /*language=*/"en",
                         /*license=*/"GPL",
                         /*author=*/"Jerin Philip <jerinphilip@live.in>",
                         /*icon=*/"/usr/share/icons/lemonade.svg",
                         /*layout=*/"us"));
      ibus_bus_register_component(bus, component);
    }

    logger_.log("Calling ibus main");
    ibus_main();
    logger_.log("Out of ibus main");
  }

private:
  IBusBus *bus = NULL;
  Logger logger_;
};
} // namespace lemonade::ibus
