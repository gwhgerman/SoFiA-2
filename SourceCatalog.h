/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (SourceCatalog.h) - Source Finding Application      ///
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

#ifndef SOURCECATALOG_H
#define SOURCECATALOG_H

#include <stdint.h>
#include "common.h"

#define MAX_STR_LENGTH 63
#define MAX_ARR_LENGTH 64

typedef class Catalog Catalog;
typedef class Source Source;


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

class Source
{
	// Properties
	char        identifier[MAX_ARR_LENGTH];
	size_t      n_par;
	int64_t    *values;
	uint8_t    *types;
	char       *names;
	char       *units;
};

// Constructor and destructor
public Source  *Source_new            (void);
public void     Source_delete         (Source *this);

// Public member functions
public void     Source_set_identifier (Source *this, const char *name);
public void     Source_add_par_flt    (Source *this, const char *name, const double value, const char *unit);
public void     Source_add_par_int    (Source *this, const char *name, const int64_t value, const char *unit);
public double   Source_get_par_flt    (const Source *this, const char *name);
public int64_t  Source_get_par_int    (const Source *this, const char *name);
public void     Source_set_par_flt    (Source *this, const char *name, const double value, const char *unit);
public void     Source_set_par_int    (Source *this, const char *name, const int64_t value, const char *unit);
public size_t   Source_par_exists     (const Source *this, const char *name);
public char    *Source_get_unit       (const Source *this, const char *name);


// ----------------------------------------------------------------- //
// Class 'Catalog'                                                   //
// ----------------------------------------------------------------- //
// The purpose of this class is to provide a structure for storing   //
// and handling source catalogues as a simple list of objects of     //
// class 'Source'.                                                   //
// ----------------------------------------------------------------- //

class Catalog
{
	// Properties
	size_t n_src;
	Source **sources;
};

// Constructor and destructor
public Catalog *Catalog_new           (void);
public void     Catalog_delete        (Catalog *this);

// Public member functions
public void     Catalog_add_source    (Catalog *this, Source *src);
public size_t   Catalog_source_exists (const Catalog *this, const Source *src);
public Source  *Catalog_get_source    (const Catalog *this, const char *identifier);

#endif
