#ifndef STRING_H
#define STRING_H

#include "common.h"

typedef CLASS String String;

// Constructor and destructor
PUBLIC String    *String_new        (const char *string);
PUBLIC String    *String_copy       (const String *string);
PUBLIC void       String_delete     (String *self);

// Public methods
PUBLIC size_t      String_size      (const String *self);
PUBLIC const char *String_get       (const String *self);
PUBLIC char        String_at        (const String *self, const size_t index);
PUBLIC bool        String_compare   (const String *self, const char *string);

PUBLIC String     *String_set       (String *self, const char *string);
PUBLIC String     *String_set_int   (String *self, char *format, const long int value);
PUBLIC String     *String_set_delim (String *self, const char *string, const char delimiter, const bool first, const bool until);
PUBLIC String     *String_append    (String *self, const char *string);
PUBLIC String     *String_append_int(String *self, char *format, const long int value);
PUBLIC String     *String_append_flt(String *self, char *format, const double value);
PUBLIC String     *String_prepend   (String *self, const char *string);

PUBLIC String     *String_clear     (String *self);
PUBLIC String     *String_trim      (String *self);

#endif
