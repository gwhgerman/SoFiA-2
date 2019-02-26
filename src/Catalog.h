/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (Catalog.h) - Source Finding Application            ///
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

#ifndef CATALOG_H
#define CATALOG_H

#include <stdint.h>
#include "common.h"
#include "Source.h"

#define CATALOG_COLUMN_WIDTH 10

typedef enum {CATALOG_FORMAT_ASCII, CATALOG_FORMAT_XML, CATALOG_FORMAT_SQL} file_format;


// ----------------------------------------------------------------- //
// Class 'Catalog'                                                   //
// ----------------------------------------------------------------- //
// The purpose of this class is to provide a structure for storing   //
// and handling source catalogues as a simple list of objects of     //
// class 'Source'.                                                   //
// ----------------------------------------------------------------- //

typedef class Catalog Catalog;

// Constructor and destructor
public Catalog *Catalog_new           (void);
public void     Catalog_delete        (Catalog *this);

// Public member functions
public void     Catalog_add_source    (Catalog *this, Source *src);
public Source  *Catalog_get_source    (const Catalog *this, const char *identifier);
public bool     Catalog_source_exists (const Catalog *this, const Source *src, size_t *index);

public void     Catalog_save          (const Catalog *this, const char *filename, const file_format format);

#endif
