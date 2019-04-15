/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (LinkerPar.h) - Source Finding Application          ///
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

#ifndef LINKERPAR_H
#define LINKERPAR_H

#include "common.h"
#include "Map.h"
#include "Catalog.h"


// ----------------------------------------------------------------- //
// Class 'LinkerPar'                                                 //
// ----------------------------------------------------------------- //
// The purpose of this class is to provide a structure for storing   //
// and updating source parameters handled by the linker implemented  //
// in the class 'DataCube'.                                          //
// ----------------------------------------------------------------- //

typedef class LinkerPar LinkerPar;

// Constructor and destructor
public LinkerPar *LinkerPar_new          (const bool verbosity);
public void       LinkerPar_delete       (LinkerPar *this);

// Public methods
public size_t     LinkerPar_get_size     (const LinkerPar *this);
public void       LinkerPar_push         (LinkerPar *this, const size_t label, const size_t x, const size_t y, const size_t z, const double flux);
public void       LinkerPar_pop          (LinkerPar *this);
public void       LinkerPar_update       (LinkerPar *this, const size_t label, const size_t x, const size_t y, const size_t z, const double flux);
public size_t     LinkerPar_get_obj_size (const LinkerPar *this, const size_t label, const int axis);
public size_t     LinkerPar_get_npix     (const LinkerPar *this, const size_t label);
public void       LinkerPar_get_bbox     (const LinkerPar *this, const size_t label, size_t *x_min, size_t *x_max, size_t *y_min, size_t *y_max, size_t *z_min, size_t *z_max);
public double     LinkerPar_get_flux     (const LinkerPar *this, const size_t label);
public double     LinkerPar_get_rel      (const LinkerPar *this, const size_t label);
public size_t     LinkerPar_get_label    (const LinkerPar *this, const size_t index);

public Catalog   *LinkerPar_make_catalog (const LinkerPar *this, const Map *filter, const char *flux_unit);
public void       LinkerPar_print_info   (const LinkerPar *this);

// Reliability filtering
public void       LinkerPar_reliability  (LinkerPar *this, const double scale_kernel, const double rms);
public void       LinkerPar_rel_plots    (const LinkerPar *this, const double threshold, const double fmin, const bool overwrite);

#endif
