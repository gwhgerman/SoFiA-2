#ifndef STRING_H
#define STRING_H

#include "common.h"

typedef CLASS String String;

// Constructor and destructor
PUBLIC String    *String_new     (const char *string);
PUBLIC void       String_delete  (String *self);

// Public methods
PUBLIC size_t      String_size   (const String *self);
PUBLIC const char *String_get    (const String *self);
PUBLIC char        String_at     (const String *self, const size_t index);
PUBLIC bool        String_compare(const String *self, const char *string);

PUBLIC void        String_set    (String *self, const char *string);
PUBLIC void        String_append (String *self, const char *string);

PUBLIC void        String_clear  (String *self);
PUBLIC void        String_trim   (String *self);

#endif
