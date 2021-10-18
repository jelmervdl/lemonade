#include "lemonade_engine.h"
#include "engine_compat.h"
#include "lemonade/lib/logging.h"
#include "lemonade/lib/utils.h"
#include <string>
#include <vector>

namespace lemonade::ibus {

namespace {
std::string getIBUSLoggingDirectory() {
  return fmt::format("{}/.lemonade/ibus.log", home());
}
} // namespace

/* constructor */
LemonadeEngine::LemonadeEngine(IBusEngine *engine)
    : Engine(engine), logger_("ibus-engine", {getIBUSLoggingDirectory()}) {
  logger_.log("Lemonade engine started");
  gint i;
}

/* destructor */
LemonadeEngine::~LemonadeEngine(void) {}

gboolean LemonadeEngine::processKeyEvent(guint keyval, guint keycode,
                                         guint modifiers) {
  gboolean retval = FALSE;

  if (contentIsPassword())
    return retval;

  if (modifiers & IBUS_RELEASE_MASK) {
    return FALSE;
  }

  std::string translation = "Translation";
  std::vector<std::string> entries = {translation, buffer_};
  g::LookupTable table = generateLookupTable(entries);
  updateLookupTable(table, /*visible=*/!entries.empty());
  retval = TRUE;

  showLookupTable();

  return retval;
}

void LemonadeEngine::focusIn(void) {}

void LemonadeEngine::focusOut(void) { Engine::focusOut(); }

void LemonadeEngine::reset(void) {}

void LemonadeEngine::enable(void) {}

void LemonadeEngine::disable(void) {}

void LemonadeEngine::pageUp(void) {}

void LemonadeEngine::pageDown(void) {}

void LemonadeEngine::cursorUp(void) {}

void LemonadeEngine::cursorDown(void) {}

inline void LemonadeEngine::showSetupDialog(void) {
  // g_spawn_command_line_async(LIBEXECDIR "/ibus-setup-libzhuyin zhuyin",
  // NULL);
}

gboolean LemonadeEngine::propertyActivate(const char *prop_name,
                                          guint prop_state) {
  return FALSE;
}

void LemonadeEngine::candidateClicked(guint index, guint button, guint state) {}

g::LookupTable
LemonadeEngine::generateLookupTable(const std::vector<std::string> &entries) {
  g::LookupTable lookupTable;
  for (auto &entry : entries) {
    lookupTable.appendCandidate(g::Text(entry));
  }
  return lookupTable;
}

} // namespace lemonade::ibus
