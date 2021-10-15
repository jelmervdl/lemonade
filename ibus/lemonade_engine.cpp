#include "lemonade_engine.h"
#include "engine_compat.h"

/* constructor */
LemonadeEngine::LemonadeEngine(IBusEngine *engine) : Engine(engine) { gint i; }

/* destructor */
LemonadeEngine::~LemonadeEngine(void) {}

gboolean LemonadeEngine::processKeyEvent(guint keyval, guint keycode,
                                         guint modifiers) {
  gboolean retval = FALSE;

  if (contentIsPassword())
    return retval;

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
