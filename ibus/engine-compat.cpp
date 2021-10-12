#include "engine-compat.h"

typedef struct _IBusBergamotEngine IBusBergamotEngine;
typedef struct _IBusBergamotEngineClass IBusBergamotEngineClass;

struct _IBusBergamotEngine {
  IBusEngine parent;
  GString *preedit;
  gint cursor_pos;
  IBusLookupTable *table;
};

struct _IBusBergamotEngineClass {
  IBusEngineClass parent;
};

/* functions prototype */
static void ibus_bergamot_engine_class_init(IBusBergamotEngineClass *klass);
static void ibus_bergamot_engine_init(IBusBergamotEngine *engine);
static void ibus_bergamot_engine_destroy(IBusBergamotEngine *engine);
static gboolean ibus_bergamot_engine_process_key_event(IBusEngine *engine,
                                                       guint keyval,
                                                       guint keycode,
                                                       guint modifiers);
static void ibus_bergamot_engine_focus_in(IBusEngine *engine);
static void ibus_bergamot_engine_focus_out(IBusEngine *engine);
static void ibus_bergamot_engine_reset(IBusEngine *engine);
static void ibus_bergamot_engine_enable(IBusEngine *engine);
static void ibus_bergamot_engine_disable(IBusEngine *engine);
static void ibus_engine_set_cursor_location(IBusEngine *engine, gint x, gint y,
                                            gint w, gint h);
static void ibus_bergamot_engine_set_capabilities(IBusEngine *engine,
                                                  guint caps);
static void ibus_bergamot_engine_page_up(IBusEngine *engine);
static void ibus_bergamot_engine_page_down(IBusEngine *engine);
static void ibus_bergamot_engine_cursor_up(IBusEngine *engine);
static void ibus_bergamot_engine_cursor_down(IBusEngine *engine);
static void ibus_bergamot_property_activate(IBusEngine *engine,
                                            const gchar *prop_name,
                                            gint prop_state);
static void ibus_bergamot_engine_property_show(IBusEngine *engine,
                                               const gchar *prop_name);
static void ibus_bergamot_engine_property_hide(IBusEngine *engine,
                                               const gchar *prop_name);

static void ibus_bergamot_engine_commit_string(IBusBergamotEngine *bergamot,
                                               const gchar *string);
static void ibus_bergamot_engine_update(IBusBergamotEngine *bergamot);

G_DEFINE_TYPE(IBusBergamotEngine, ibus_bergamot_engine, IBUS_TYPE_ENGINE)

static void ibus_bergamot_engine_class_init(IBusBergamotEngineClass *klass) {
  IBusObjectClass *ibus_object_class = IBUS_OBJECT_CLASS(klass);
  IBusEngineClass *engine_class = IBUS_ENGINE_CLASS(klass);

  ibus_object_class->destroy =
      (IBusObjectDestroyFunc)ibus_bergamot_engine_destroy;

  engine_class->process_key_event = ibus_bergamot_engine_process_key_event;
}

static void ibus_bergamot_engine_init(IBusBergamotEngine *bergamot) {
  bergamot->preedit = g_string_new("");
  bergamot->cursor_pos = 0;

  bergamot->table = ibus_lookup_table_new(9, 0, TRUE, TRUE);
  g_object_ref_sink(bergamot->table);
}

static void ibus_bergamot_engine_destroy(IBusBergamotEngine *bergamot) {
  if (bergamot->preedit) {
    g_string_free(bergamot->preedit, TRUE);
    bergamot->preedit = NULL;
  }

  if (bergamot->table) {
    g_object_unref(bergamot->table);
    bergamot->table = NULL;
  }

  ((IBusObjectClass *)ibus_bergamot_engine_parent_class)
      ->destroy((IBusObject *)bergamot);
}

static gboolean ibus_bergamot_engine_process_key_event(IBusEngine *engine,
                                                       guint keyval,
                                                       guint keycode,
                                                       guint modifiers) {
  IBusText *text;
  IBusBergamotEngine *bergamot = (IBusBergamotEngine *)engine;

  if (modifiers & IBUS_RELEASE_MASK)
    return FALSE;

  modifiers &= (IBUS_CONTROL_MASK | IBUS_MOD1_MASK);

  if (modifiers == IBUS_CONTROL_MASK && keyval == IBUS_s) {
    // ibus_bergamot_engine_update_lookup_table(bergamot);
    return TRUE;
  }

  if (modifiers != 0) {
    if (bergamot->preedit->len == 0)
      return FALSE;
    else
      return TRUE;
  }

  switch (keyval) {
  case IBUS_space:
    g_string_append(bergamot->preedit, " ");
    // return ibus_bergamot_engine_commit_preedit(bergamot);
    return TRUE;
  case IBUS_Return:
    // return ibus_bergamot_engine_commit_preedit(bergamot);
    return TRUE;

  case IBUS_Escape:
    if (bergamot->preedit->len == 0)
      return FALSE;

    g_string_assign(bergamot->preedit, "");
    bergamot->cursor_pos = 0;
    ibus_bergamot_engine_update(bergamot);
    return TRUE;

  case IBUS_Left:
    if (bergamot->preedit->len == 0)
      return FALSE;
    if (bergamot->cursor_pos > 0) {
      bergamot->cursor_pos--;
      ibus_bergamot_engine_update(bergamot);
    }
    return TRUE;

  case IBUS_Right:
    if (bergamot->preedit->len == 0)
      return FALSE;
    if (bergamot->cursor_pos < bergamot->preedit->len) {
      bergamot->cursor_pos++;
      ibus_bergamot_engine_update(bergamot);
    }
    return TRUE;

  case IBUS_Up:
    if (bergamot->preedit->len == 0)
      return FALSE;
    if (bergamot->cursor_pos != 0) {
      bergamot->cursor_pos = 0;
      ibus_bergamot_engine_update(bergamot);
    }
    return TRUE;

  case IBUS_Down:
    if (bergamot->preedit->len == 0)
      return FALSE;

    if (bergamot->cursor_pos != bergamot->preedit->len) {
      bergamot->cursor_pos = bergamot->preedit->len;
      ibus_bergamot_engine_update(bergamot);
    }

    return TRUE;

  case IBUS_BackSpace:
    if (bergamot->preedit->len == 0)
      return FALSE;
    if (bergamot->cursor_pos > 0) {
      bergamot->cursor_pos--;
      g_string_erase(bergamot->preedit, bergamot->cursor_pos, 1);
      ibus_bergamot_engine_update(bergamot);
    }
    return TRUE;

  case IBUS_Delete:
    if (bergamot->preedit->len == 0)
      return FALSE;
    if (bergamot->cursor_pos < bergamot->preedit->len) {
      g_string_erase(bergamot->preedit, bergamot->cursor_pos, 1);
      ibus_bergamot_engine_update(bergamot);
    }
    return TRUE;
  }

  if (is_alpha(keyval)) {
    g_string_insert_c(bergamot->preedit, bergamot->cursor_pos, keyval);

    bergamot->cursor_pos++;
    ibus_bergamot_engine_update(bergamot);

    return TRUE;
  }

  return FALSE;
}

static void ibus_bergamot_engine_update(IBusBergamotEngine *bergamot) {}
