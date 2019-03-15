/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (DataCube.h) - Source Finding Application           ///
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

#ifndef DATACUBE_H
#define DATACUBE_H

#define FITS_HEADER_BLOCK_SIZE   2880
#define FITS_HEADER_LINE_SIZE      80
#define FITS_HEADER_LINES          36
#define FITS_HEADER_KEYWORD_SIZE    8
#define FITS_HEADER_KEY_SIZE       10
#define FITS_HEADER_VALUE_SIZE     70
#define FITS_HEADER_FIXED_WIDTH    20

typedef enum {NOISE_STAT_STD, NOISE_STAT_MAD, NOISE_STAT_GAUSS} noise_stat;

#include <stdlib.h>
#include <stdbool.h>

#include "common.h"
#include "Array.h"
#include "Catalog.h"
#include "LinkerPar.h"


// ----------------------------------------------------------------- //
// Class 'DataCube'                                                  //
// ----------------------------------------------------------------- //
// The purpose of this class is to handle up to three-dimensional    //
// astronomical data cubes. The class is intended for reading and    //
// manipulating FITS data cubes by providing methods for loading and //
// saving FITS files and manipulating the header and data units of a //
// FITS file. Currently, only single-HDU files are supported.        //
// ----------------------------------------------------------------- //

typedef class DataCube DataCube;

// Constructor and destructor
public DataCube  *DataCube_new              (void);
public DataCube  *DataCube_copy             (const DataCube *source);
public DataCube  *DataCube_blank            (const size_t nx, const size_t ny, const size_t nz, const int type);
public void       DataCube_delete           (DataCube *this);

// Public methods
// Loading/saving from/to FITS format
public void       DataCube_load             (DataCube *this, const char *filename, const Array *region);
public void       DataCube_save             (const DataCube *this, const char *filename, const bool overwrite);

// Extract header entries
public long int   DataCube_gethd_int        (const DataCube *this, const char *key);
public double     DataCube_gethd_flt        (const DataCube *this, const char *key);
public bool       DataCube_gethd_bool       (const DataCube *this, const char *key);
public int        DataCube_gethd_str        (const DataCube *this, const char *key, char *value);

// Manipulate header entries
public int        DataCube_puthd_int        (DataCube *this, const char *key, const long int value);
public int        DataCube_puthd_flt        (DataCube *this, const char *key, const double value);
public int        DataCube_puthd_bool       (DataCube *this, const char *key, const bool value);
public int        DataCube_puthd_str        (DataCube *this, const char *key, const char *value);

// Miscellaneous header operations
public size_t     DataCube_chkhd            (const DataCube *this, const char *key);
public int        DataCube_delhd            (DataCube *this, const char *key);

// Extract data values
public double     DataCube_get_data_flt     (const DataCube *this, const size_t x, const size_t y, const size_t z);
public long int   DataCube_get_data_int     (const DataCube *this, const size_t x, const size_t y, const size_t z);

// Manipulate data values
public void       DataCube_set_data_flt     (DataCube *this, const size_t x, const size_t y, const size_t z, const double value);
public void       DataCube_set_data_int     (DataCube *this, const size_t x, const size_t y, const size_t z, const long int value);
public void       DataCube_add_data_flt     (DataCube *this, const size_t x, const size_t y, const size_t z, const double value);
public void       DataCube_add_data_int     (DataCube *this, const size_t x, const size_t y, const size_t z, const long int value);

// Statistical measurements
public double     DataCube_stat_std         (const DataCube *this, const double value, const size_t cadence, const int range);
public double     DataCube_stat_mad         (const DataCube *this, const double value, const size_t cadence, const int range);
public double     DataCube_stat_gauss       (const DataCube *this, const size_t cadence, const int range);

// Noise scaling
public void       DataCube_scale_noise_spec (const DataCube *this, const noise_stat statistic, const int range);

// Spatial and spectral smoothing
public void       DataCube_boxcar_filter    (DataCube *this, size_t radius);
public void       DataCube_gaussian_filter  (DataCube *this, const double sigma);

// Masking
public int        DataCube_mask             (const DataCube *this, DataCube *maskCube, const double threshold);
public int        DataCube_mask_32          (const DataCube *this, DataCube *maskCube, const double threshold);
public int        DataCube_set_masked       (DataCube *this, const DataCube *maskCube, const double value);
public int        DataCube_set_masked_32    (DataCube *this, const DataCube *maskCube, const double value);

// Source finding
public DataCube  *DataCube_run_scfind       (const DataCube *this, const Array *kernels_spat, const Array *kernels_spec, const double threshold, const double maskScaleXY, const noise_stat method, const int range);

// Linking
public LinkerPar *DataCube_run_linker       (const DataCube *this, DataCube *mask, const size_t radius_x, const size_t radius_y, const size_t radius_z, const size_t min_size_x, const size_t min_size_y, const size_t min_size_z, const bool positivity);

// Parameterisation
public void       DataCube_parameterise     (const DataCube *this, const DataCube *mask, Catalog *cat);

// Create moment maps
public void       DataCube_create_moments   (const DataCube *this, const DataCube *mask, DataCube **moment0, DataCube **moment1, DataCube **moment2);

#endif
