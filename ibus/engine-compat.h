#include <ibus.h>

#define IBUS_TYPE_BERGAMOT_ENGINE (ibus_bergamot_engine_get_type())
#define is_alpha(c)                                                            \
  (((c) >= IBUS_a && (c) <= IBUS_z) || ((c) >= IBUS_A && (c) <= IBUS_Z))

GType ibus_bergamot_engine_get_type(void);
