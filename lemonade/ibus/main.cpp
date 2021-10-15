#include "application.h"
#include "engine_compat.h"
#include <ibus.h>

int main(int argc, char **argv) {
  /* command line options */
  gboolean ibus = FALSE;
  gboolean verbose = FALSE;

  const GOptionEntry entries[] = {
      {"ibus", 'i', 0, G_OPTION_ARG_NONE, &ibus,
       "component is executed by ibus", NULL},
      {"verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "verbose", NULL},
      {NULL},
  };

  GError *error = NULL;
  GOptionContext *context;

  /* Parse the command line */
  context = g_option_context_new("- ibus template engine");
  g_option_context_add_main_entries(context, entries, "ibus-tmpl");

  if (!g_option_context_parse(context, &argc, &argv, &error)) {
    g_print("Option parsing failed: %s\n", error->message);
    g_error_free(error);
    return (-1);
  }

  lemonade::ibus::Application application(ibus);
  ibus_main();
  return 0;
}
