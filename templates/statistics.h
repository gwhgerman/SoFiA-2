/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (statistics_SUFFIX.h) - Source Finding Application     ///
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

#ifndef STATISTICS_SUFFIX_H
#define STATISTICS_SUFFIX_H

#include <stdbool.h>
#include "common.h"

// -------------------------- //
// Settings for boxcar filter //
// -------------------------- //
#define BOXCAR_MIN_ITER 3
#define BOXCAR_MAX_ITER 6


typedef struct Array_SUFFIX Array_SUFFIX;
struct Array_SUFFIX
{
	DATA_T *data;
	size_t size;
};



// -------------------- //
// Statistics functions //
// -------------------- //

// Check if array contains NaN
int contains_nan_SUFFIX(const DATA_T *data, const size_t size);

// NaN-free copy of array
Array_SUFFIX clean_copy_SUFFIX(const DATA_T *data, const size_t size);

// Maximum and minimum
void max_min_SUFFIX(const DATA_T *data, const size_t size, DATA_T *value_max, DATA_T *value_min);
DATA_T max_SUFFIX(const DATA_T *data, const size_t size);
DATA_T min_SUFFIX(const DATA_T *data, const size_t size);

// Sum and mean
double summation_SUFFIX(const DATA_T *data, const size_t size, const bool mean);
double sum_SUFFIX(const DATA_T *data, const size_t size);
double mean_SUFFIX(const DATA_T *data, const size_t size);

// N-th moment
double moment_SUFFIX(const DATA_T *data, const size_t size, unsigned int order, const double value);
void moments_SUFFIX(const DATA_T *data, const size_t size, const double value, double *m2, double *m3, double *m4);

// Standard deviation
double std_dev_SUFFIX(const DATA_T *data, const size_t size);
double std_dev_val_SUFFIX(const DATA_T *data, const size_t size, const double value, const size_t cadence, const int range);

// N-th-smallest element
DATA_T nth_element_SUFFIX(DATA_T *data, const size_t size, const size_t n);

// Median and MAD
DATA_T median_SUFFIX(DATA_T *data, const size_t size, const bool fast);
DATA_T mad_SUFFIX(DATA_T *data, const size_t size);
DATA_T mad_val_SUFFIX(const DATA_T *data, const size_t size, const DATA_T value, const size_t cadence, const int range);

// Gaussian fit to histogram
size_t *create_histogram_SUFFIX(const DATA_T *data, const size_t size, const size_t n_bins, const DATA_T data_min, const DATA_T data_max, const size_t cadence);
DATA_T gaufit_SUFFIX(const DATA_T *data, const size_t size, const size_t cadence, const int range);

// Skewness and kurtosis
void skew_kurt_SUFFIX(const DATA_T *data, const size_t size, double *skew, double *kurt);
double skewness_SUFFIX(const DATA_T *data, const size_t size);
double kurtosis_SUFFIX(const DATA_T *data, const size_t size);

// 1D boxcar filter
void filter_boxcar_1d_SUFFIX(DATA_T *data, DATA_T *data_copy, const size_t size, const size_t filter_radius, const bool replace_nan);

// 2D Gaussian filter
void filter_gauss_2d_SUFFIX(DATA_T *data, DATA_T *data_copy, DATA_T *data_row, DATA_T *data_col, const size_t size_x, const size_t size_y, const size_t n_iter, const size_t filter_radius, const bool replace_nan);


// ----------------- //
// Utility functions //
// ----------------- //

// Filter size and iterations for Gaussian function
void optimal_filter_size_SUFFIX(const double sigma, size_t *filter_radius, size_t *n_iter);

#endif
