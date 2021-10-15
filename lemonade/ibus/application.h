#pragma once
#include "engine_compat.h"
#include <ibus.h>

static g::Pointer<IBusFactory> factory = NULL;

namespace lemonade::ibus {
class Application {
public:
  Application(gboolean ibus) {
    ibus_init();

    bus = ibus_bus_new();
    g_object_ref_sink(bus);

    auto callback = +[](IBusBus *bus, gpointer user_data) { ibus_quit(); };
    g_signal_connect(bus, "disconnected", G_CALLBACK(callback), NULL);

    factory = ibus_factory_new(ibus_bus_get_connection(bus));

    ibus_factory_add_engine(factory, "",
                            IBUS_TYPE_LEMONADE_ENGINE);

    if (ibus) {
      ibus_bus_request_name(bus, "org.freedesktop.IBus.Lemonade", 0);
    } else {

      g::Pointer<IBusComponent> component;

      component = ibus_component_new(
          /*name=*/"org.freedesktop.IBus.Lemonade", /*description=*/"Lemonade provides client-side translation on the local *nix machine.",
          /*version=*/"", /*license=*/"GPL",
          /*uthor=*/"",
          /*homepage=*/"https://github.com/jerinphilip/lemonade",
          /*commandline=*/"", /*textdomain=*/"lemonade-ime");
      ibus_component_add_engine(component,
                                ibus_engine_desc_new(
                                    /*shortname=*/"lemonade",
                                    /*longname=*/"Lemonade",
                                    /*description=*/"Lemonade provides client-side translation on the local *nix machine.",
                                    /*language=*/"en",
                                    /*license=*/"GPL",
                                    /*author=*/"Jerin Philip <jerinphilip@live.in>",
                                    /*icon=*/"/usr/share/icons/lemonade.svg",
                                    /*layout=*/"us"));
      ibus_bus_register_component(bus, component);
    }
  }

private:
  IBusBus *bus = NULL;
};
} // namespace lemonade::ibus
