#ifndef HEADER_H
#define HEADER_H

#include "common.h"
#include "String.h"

#define FITS_HEADER_BLOCK_SIZE   2880
#define FITS_HEADER_LINE_SIZE      80
#define FITS_HEADER_LINES          36
#define FITS_HEADER_KEYWORD_SIZE    8
#define FITS_HEADER_KEY_SIZE       10
#define FITS_HEADER_VALUE_SIZE     70
#define FITS_HEADER_FIXED_WIDTH    20


// ----------------------------------------------------------------- //
// Class 'Header'                                                    //
// ----------------------------------------------------------------- //
// The purpose of this class is to provide a structure for storing   //
// header information from a FITS data file and read and manipulate  //
// individual header entries.                                        //
// ----------------------------------------------------------------- //

typedef CLASS Header Header;

// Constructor and destructor
PUBLIC Header     *Header_new        (const char *header, const size_t size, const bool verbosity);
PUBLIC Header     *Header_copy       (const Header *source);
PUBLIC Header     *Header_blank      (const bool verbosity);
PUBLIC void        Header_delete     (Header *self);

// Public methods
PUBLIC const char *Header_get        (const Header *self);
PUBLIC size_t      Header_get_size   (const Header *self);

// Extract header entries
PUBLIC long int    Header_get_int    (const Header *self, const char *key);
PUBLIC double      Header_get_flt    (const Header *self, const char *key);
PUBLIC bool        Header_get_bool   (const Header *self, const char *key);
PUBLIC int         Header_get_str    (const Header *self, const char *key, char *value);
PUBLIC String     *Header_get_string (const Header *self, const char *key);

// Manipulate header entries
PUBLIC int         Header_set_int    (Header *self, const char *key, const long int value);
PUBLIC int         Header_set_flt    (Header *self, const char *key, const double value);
PUBLIC int         Header_set_bool   (Header *self, const char *key, const bool value);
PUBLIC int         Header_set_str    (Header *self, const char *key, const char *value);

// Miscellaneous header operations
PUBLIC size_t      Header_check      (const Header *self, const char *key);
PUBLIC bool        Header_compare    (const Header *self, const char *key, const char *value, const size_t n);
PUBLIC int         Header_remove     (Header *self, const char *key);
PUBLIC void        Header_copy_wcs   (const Header *source, Header *target);
PUBLIC void        Header_copy_misc  (const Header *source, Header *target, const bool copy_bunit, const bool copy_beam);
PUBLIC void        Header_adjust_wcs_to_subregion(Header *self, const size_t x_min, const size_t x_max, const size_t y_min, const size_t y_max, const size_t z_min, const size_t z_max);

#endif
