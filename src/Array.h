/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (Array.h) - Source Finding Application              ///
/// Copyright (C) 2019 Tobias Westmeier                                  ///
/// ____________________________________________________________________ ///
///                                                                      ///
/// Address:  Tobias Westmeier                                           ///
///           ICRAR M468                                                 ///
///           The University of Western Australia                        ///
///           35 Stirling Highway                                        ///
///           Crawley WA 6009                                            ///
///           Australia                                                  ///
///                                                                      ///
/// E-mail:   tobias.westmeier [at] uwa.edu.au                           ///
/// ____________________________________________________________________ ///
///                                                                      ///
/// This program is free software: you can redistribute it and/or modify ///
/// it under the terms of the GNU General Public License as published by ///
/// the Free Software Foundation, either version 3 of the License, or    ///
/// (at your option) any later version.                                  ///
///                                                                      ///
/// This program is distributed in the hope that it will be useful,      ///
/// but WITHOUT ANY WARRANTY; without even the implied warranty of       ///
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the         ///
/// GNU General Public License for more details.                         ///
///                                                                      ///
/// You should have received a copy of the GNU General Public License    ///
/// along with this program. If not, see http://www.gnu.org/licenses/.   ///
/// ____________________________________________________________________ ///
///                                                                      ///

#ifndef ARRAY_H
#define ARRAY_H

#include "common.h"

#define ARRAY_TYPE_FLT 0
#define ARRAY_TYPE_INT 1


// ----------------------------------------------------------------- //
// Class 'Array'                                                     //
// ----------------------------------------------------------------- //
// The purpose of this class is to provide a convenient way to store //
// multiple values of type double or long in an array-like struc-    //
// ture. A new array can either be of a given size and empty (using  //
// the standard constructor) or provided with a list of comma-sepa-  //
// rated values that will be stored in the array and used to deter-  //
// mine its size using the alternative constructor).                 //
// ----------------------------------------------------------------- //

typedef CLASS Array Array;

// Constructor and destructor
PUBLIC Array            *Array_new      (const size_t size, const int type);
PUBLIC Array            *Array_new_str  (char *string, const int type);
PUBLIC void              Array_delete   (Array *self);

// Methods
PUBLIC const void       *Array_get_ptr  (const Array *self);
PUBLIC size_t            Array_get_size (const Array *self);
PUBLIC int               Array_get_type (const Array *self);

PUBLIC void              Array_push_flt (Array *self, const double value);
PUBLIC void              Array_push_int (Array *self, const long int value);

PUBLIC double            Array_get_flt  (const Array *self, const size_t index);
PUBLIC long int          Array_get_int  (const Array *self, const size_t index);
PUBLIC unsigned long int Array_get_uint (const Array *self, const size_t index);

PUBLIC void              Array_set_flt  (Array *self, const size_t index, const double value);
PUBLIC void              Array_set_int  (Array *self, const size_t index, const long int value);
PUBLIC void              Array_add_flt  (Array *self, const size_t index, const double value);
PUBLIC void              Array_add_int  (Array *self, const size_t index, const long int value);

#endif
