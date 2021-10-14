#include "engine-compat.h"

typedef struct _IBusLemonadeEngine IBusLemonadeEngine;
typedef struct _IBusLemonadeEngineClass IBusLemonadeEngineClass;

struct _IBusLemonadeEngine {
  IBusEngine parent;
  GString *preedit;
  gint cursor_pos;
  IBusLookupTable *table;
};

struct _IBusLemonadeEngineClass {
  IBusEngineClass parent;
};

/* functions prototype */
static void ibus_lemonade_engine_class_init(IBusLemonadeEngineClass *klass);
static void ibus_lemonade_engine_init(IBusLemonadeEngine *engine);
static void ibus_lemonade_engine_destroy(IBusLemonadeEngine *engine);
static gboolean ibus_lemonade_engine_process_key_event(IBusEngine *engine,
                                                       guint keyval,
                                                       guint keycode,
                                                       guint modifiers);
static void ibus_lemonade_engine_focus_in(IBusEngine *engine);
static void ibus_lemonade_engine_focus_out(IBusEngine *engine);
static void ibus_lemonade_engine_reset(IBusEngine *engine);
static void ibus_lemonade_engine_enable(IBusEngine *engine);
static void ibus_lemonade_engine_disable(IBusEngine *engine);
static void ibus_engine_set_cursor_location(IBusEngine *engine, gint x, gint y,
                                            gint w, gint h);
static void ibus_lemonade_engine_set_capabilities(IBusEngine *engine,
                                                  guint caps);
static void ibus_lemonade_engine_page_up(IBusEngine *engine);
static void ibus_lemonade_engine_page_down(IBusEngine *engine);
static void ibus_lemonade_engine_cursor_up(IBusEngine *engine);
static void ibus_lemonade_engine_cursor_down(IBusEngine *engine);
static void ibus_lemonade_property_activate(IBusEngine *engine,
                                            const gchar *prop_name,
                                            gint prop_state);
static void ibus_lemonade_engine_property_show(IBusEngine *engine,
                                               const gchar *prop_name);
static void ibus_lemonade_engine_property_hide(IBusEngine *engine,
                                               const gchar *prop_name);

static void ibus_lemonade_engine_commit_string(IBusLemonadeEngine *lemonade,
                                               const gchar *string);
static void ibus_lemonade_engine_update(IBusLemonadeEngine *lemonade);

G_DEFINE_TYPE(IBusLemonadeEngine, ibus_lemonade_engine, IBUS_TYPE_ENGINE)

static void ibus_lemonade_engine_class_init(IBusLemonadeEngineClass *klass) {
  IBusObjectClass *ibus_object_class = IBUS_OBJECT_CLASS(klass);
  IBusEngineClass *engine_class = IBUS_ENGINE_CLASS(klass);

  ibus_object_class->destroy =
      (IBusObjectDestroyFunc)ibus_lemonade_engine_destroy;

  engine_class->process_key_event = ibus_lemonade_engine_process_key_event;
}

static void ibus_lemonade_engine_init(IBusLemonadeEngine *lemonade) {
  lemonade->preedit = g_string_new("");
  lemonade->cursor_pos = 0;

  lemonade->table = ibus_lookup_table_new(9, 0, TRUE, TRUE);
  g_object_ref_sink(lemonade->table);
}

static void ibus_lemonade_engine_destroy(IBusLemonadeEngine *lemonade) {
  if (lemonade->preedit) {
    g_string_free(lemonade->preedit, TRUE);
    lemonade->preedit = NULL;
  }

  if (lemonade->table) {
    g_object_unref(lemonade->table);
    lemonade->table = NULL;
  }

  ((IBusObjectClass *)ibus_lemonade_engine_parent_class)
      ->destroy((IBusObject *)lemonade);
}

static gboolean ibus_lemonade_engine_process_key_event(IBusEngine *engine,
                                                       guint keyval,
                                                       guint keycode,
                                                       guint modifiers) {
  IBusText *text;
  IBusLemonadeEngine *lemonade = (IBusLemonadeEngine *)engine;

  if (modifiers & IBUS_RELEASE_MASK)
    return FALSE;

  modifiers &= (IBUS_CONTROL_MASK | IBUS_MOD1_MASK);

  if (modifiers == IBUS_CONTROL_MASK && keyval == IBUS_s) {
    // ibus_lemonade_engine_update_lookup_table(lemonade);
    return TRUE;
  }

  if (modifiers != 0) {
    if (lemonade->preedit->len == 0)
      return FALSE;
    else
      return TRUE;
  }

  switch (keyval) {
  case IBUS_space:
    g_string_append(lemonade->preedit, " ");
    // return ibus_lemonade_engine_commit_preedit(lemonade);
    return TRUE;
  case IBUS_Return:
    // return ibus_lemonade_engine_commit_preedit(lemonade);
    return TRUE;

  case IBUS_Escape:
    if (lemonade->preedit->len == 0)
      return FALSE;

    g_string_assign(lemonade->preedit, "");
    lemonade->cursor_pos = 0;
    ibus_lemonade_engine_update(lemonade);
    return TRUE;

  case IBUS_Left:
    if (lemonade->preedit->len == 0)
      return FALSE;
    if (lemonade->cursor_pos > 0) {
      lemonade->cursor_pos--;
      ibus_lemonade_engine_update(lemonade);
    }
    return TRUE;

  case IBUS_Right:
    if (lemonade->preedit->len == 0)
      return FALSE;
    if (lemonade->cursor_pos < lemonade->preedit->len) {
      lemonade->cursor_pos++;
      ibus_lemonade_engine_update(lemonade);
    }
    return TRUE;

  case IBUS_Up:
    if (lemonade->preedit->len == 0)
      return FALSE;
    if (lemonade->cursor_pos != 0) {
      lemonade->cursor_pos = 0;
      ibus_lemonade_engine_update(lemonade);
    }
    return TRUE;

  case IBUS_Down:
    if (lemonade->preedit->len == 0)
      return FALSE;

    if (lemonade->cursor_pos != lemonade->preedit->len) {
      lemonade->cursor_pos = lemonade->preedit->len;
      ibus_lemonade_engine_update(lemonade);
    }

    return TRUE;

  case IBUS_BackSpace:
    if (lemonade->preedit->len == 0)
      return FALSE;
    if (lemonade->cursor_pos > 0) {
      lemonade->cursor_pos--;
      g_string_erase(lemonade->preedit, lemonade->cursor_pos, 1);
      ibus_lemonade_engine_update(lemonade);
    }
    return TRUE;

  case IBUS_Delete:
    if (lemonade->preedit->len == 0)
      return FALSE;
    if (lemonade->cursor_pos < lemonade->preedit->len) {
      g_string_erase(lemonade->preedit, lemonade->cursor_pos, 1);
      ibus_lemonade_engine_update(lemonade);
    }
    return TRUE;
  }

  if (is_alpha(keyval)) {
    g_string_insert_c(lemonade->preedit, lemonade->cursor_pos, keyval);

    lemonade->cursor_pos++;
    ibus_lemonade_engine_update(lemonade);

    return TRUE;
  }

  return FALSE;
}

static void ibus_lemonade_engine_update(IBusLemonadeEngine *lemonade) {}
