#include <ibus.h>

#define IBUS_TYPE_LEMONADE_ENGINE (ibus_lemonade_engine_get_type())
#define is_alpha(c)                                                            \
  (((c) >= IBUS_a && (c) <= IBUS_z) || ((c) >= IBUS_A && (c) <= IBUS_Z))

GType ibus_lemonade_engine_get_type(void);
