#ifndef WCS_H
#define WCS_H

#include "common.h"

typedef CLASS WCS WCS;


// Constructor and destructor
PUBLIC WCS  *WCS_new(char *header, const int n_keys, const int n_axes, const int *dim_axes);
PUBLIC void  WCS_delete(WCS *self);

// Public methods
PUBLIC bool  WCS_is_valid(const WCS *self);
PUBLIC void  WCS_convertToWorld(const WCS *self, const double x, const double y, const double z, double *longitude, double *latitude, double *spectral);
PUBLIC void  WCS_convertToPixel(const WCS *self, const double longitude, const double latitude, const double spectral, double *x, double *y, double *z);

#endif
