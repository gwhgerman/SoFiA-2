/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (Source.h) - Source Finding Application             ///
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

#ifndef SOURCE_H
#define SOURCE_H

#include <stdint.h>
#include "common.h"

#define MAX_STR_LENGTH 63
#define MAX_ARR_LENGTH 64

#define SOURCE_TYPE_INT 0
#define SOURCE_TYPE_FLT 1


// ----------------------------------------------------------------- //
// Class 'Source'                                                    //
// ----------------------------------------------------------------- //
// The purpose of this class is to provide a structure for storing   //
// and handling the measured parameters of a source. Parameters are  //
// composed of a name, value, type and unit. Both 64-bit integer and //
// 64-bit double-precision floating-point values are supported. In   //
// addition, a source can be assigned an identifier in the form of a //
// string, e.g. a source name.                                       //
// ----------------------------------------------------------------- //

typedef class Source Source;

// Constructor and destructor
public Source     *Source_new                 (void);
public void        Source_delete              (Source *this);

// Public member functions
public void        Source_set_identifier      (Source *this, const char *name);
public const char *Source_get_identifier      (const Source *this);

public size_t      Source_get_num_par         (const Source *this);

public void        Source_add_par_flt         (Source *this, const char *name, const double value, const char *unit, const char *ucd);
public void        Source_add_par_int         (Source *this, const char *name, const int64_t value, const char *unit, const char *ucd);
public double      Source_get_par_flt         (const Source *this, const size_t index);
public int64_t     Source_get_par_int         (const Source *this, const size_t index);
public double      Source_get_par_by_name_flt (const Source *this, const char *name);
public int64_t     Source_get_par_by_name_int (const Source *this, const char *name);
public void        Source_set_par_flt         (Source *this, const char *name, const double value, const char *unit, const char *ucd);
public void        Source_set_par_int         (Source *this, const char *name, const int64_t value, const char *unit, const char *ucd);

public bool        Source_par_exists          (const Source *this, const char *name, size_t *index);

public char       *Source_get_name            (const Source *this, const size_t index);
public char       *Source_get_unit            (const Source *this, const size_t index);
public uint8_t     Source_get_type            (const Source *this, const size_t index);
public char       *Source_get_ucd             (const Source *this, const size_t index);

#endif
