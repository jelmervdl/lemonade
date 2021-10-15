#pragma once

#include "engine_compat.h"

class LemonadeEngine : public Engine {
public:
  LemonadeEngine(IBusEngine *engine);
  ~LemonadeEngine(void);

  // virtual functions
  gboolean processKeyEvent(guint keyval, guint keycode, guint modifiers);
  void focusIn(void);
  void focusOut(void);
  void reset(void);
  void enable(void);
  void disable(void);
  void pageUp(void);
  void pageDown(void);
  void cursorUp(void);
  void cursorDown(void);
  gboolean propertyActivate(const gchar *prop_name, guint prop_state);
  void candidateClicked(guint index, guint button, guint state);

private:
  void showSetupDialog(void);
};
