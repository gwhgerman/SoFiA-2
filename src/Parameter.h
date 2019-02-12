/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (Parameter.h) - Source Finding Application          ///
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

#ifndef PARAMETER_H
#define PARAMETER_H

#include <stdbool.h>
#include "common.h"

#define MAX_LINE_SIZE 1024

enum {PARAMETER_APPEND, PARAMETER_UPDATE};


// ----------------------------------------------------------------- //
// Class 'Parameter'                                                 //
// ----------------------------------------------------------------- //
// The purpose of this class is to provide a structure for handling  //
// SoFiA parameter settings. Settings can be loaded from a file and  //
// then read or updated as needed. All parameter settings are treat- //
// ed as strings, and several methods are available for extracting   //
// the parameter value as a specific data type.                      //
// ----------------------------------------------------------------- //

typedef class Parameter Parameter;

class Parameter
{
	size_t   n_par;
	char   **keys;
	char   **values;
};

// Constructor and destructor
public Parameter *Parameter_new     (void);
public void       Parameter_delete  (Parameter *this);

// Public methods
public void       Parameter_set     (Parameter *this, const char *key, const char *value);
public int        Parameter_exists  (const Parameter *this, const char *key);
public double     Parameter_get_flt (const Parameter *this, const char *key);
public long int   Parameter_get_int (const Parameter *this, const char *key);
public bool       Parameter_get_bool(const Parameter *this, const char *key);
public char      *Parameter_get_str (const Parameter *this, const char *key);
public void       Parameter_load    (Parameter *this, const char *filename, const int mode);

#endif
