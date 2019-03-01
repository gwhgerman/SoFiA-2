/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (statistics_flt.h) - Source Finding Application     ///
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

#ifndef STATISTICS_flt_H
#define STATISTICS_flt_H

#include <stdbool.h>
// Data type definition automatically removed

// --------------------------------------------------- //
// Conversion factor between MAD and STD               //
// Calculated as 1.0 / scipy.stats.norm.ppf(3.0 / 4.0) //
// --------------------------------------------------- //
#ifndef MAD_TO_STD
#define MAD_TO_STD 1.482602218505602
#endif

// -------------------------------------- //
// Define value of mathematical constants //
// -------------------------------------- //
#ifndef M_PI
#define M_PI 3.141592653589793
#endif
#ifndef INV_SQRT_TWO_PI
#define INV_SQRT_TWO_PI 0.3989422804014327
#endif

// ------------- //
// Check for NaN //
// ------------- //
#ifndef IS_NAN
#define IS_NAN(x) ((x) != (x))
#endif
#ifndef IS_NOT_NAN
#define IS_NOT_NAN(x) ((x) == (x))
#endif
#ifndef FILTER_NAN
#define FILTER_NAN(x) ((x) == (x) ? (x) : 0)
#endif

// ------------ //
// Check if odd //
// ------------ //
#ifndef IS_ODD
#define IS_ODD(x) ((x) & 1)
#endif
#ifndef IS_EVEN
#define IS_EVEN(x) (!((x) & 1))
#endif

// -------------------------- //
// Settings for boxcar filter //
// -------------------------- //
#ifndef BOXCAR_MIN_ITER
#define BOXCAR_MIN_ITER 3
#endif
#ifndef BOXCAR_MAX_ITER
#define BOXCAR_MAX_ITER 6
#endif

typedef struct Array_flt Array_flt;
struct Array_flt
{
	float *data;
	size_t size;
};



// -------------------- //
// Statistics functions //
// -------------------- //

// Check if array contains NaN
int contains_nan_flt(const float *data, const size_t size);

// NaN-free copy of array
Array_flt clean_copy_flt(const float *data, const size_t size);

// Maximum and minimum
void max_min_flt(const float *data, const size_t size, float *value_max, float *value_min);
float max_flt(const float *data, const size_t size);
float min_flt(const float *data, const size_t size);

// Sum and mean
double summation_flt(const float *data, const size_t size, const bool mean);
double sum_flt(const float *data, const size_t size);
double mean_flt(const float *data, const size_t size);

// N-th moment
double moment_flt(const float *data, const size_t size, unsigned int order, const double value);
void moments_flt(const float *data, const size_t size, const double value, double *m2, double *m3, double *m4);

// Standard deviation
double std_dev_flt(const float *data, const size_t size);
double std_dev_val_flt(const float *data, const size_t size, const double value, const size_t cadence, const int range);

// N-th-smallest element
float nth_element_flt(float *data, const size_t size, const size_t n);

// Median and MAD
float median_flt(float *data, const size_t size, const bool fast);
float mad_flt(float *data, const size_t size);
float mad_val_flt(float *data, const size_t size, const float value);

// Gaussian fit to histogram
size_t *create_histogram_flt(const float *data, const size_t size, const size_t n_bins, const float data_min, const float data_max, const size_t cadence);
float gaufit_flt(float *data, const size_t size, const size_t cadence, const int range);

// Skewness and kurtosis
void skew_kurt_flt(const float *data, const size_t size, double *skew, double *kurt);
double skewness_flt(const float *data, const size_t size);
double kurtosis_flt(const float *data, const size_t size);

// 1D boxcar filter
void filter_boxcar_1d_flt(float *data, float *data_copy, const size_t size, const size_t filter_radius, const bool replace_nan);

// 2D Gaussian filter
void filter_gauss_2d_flt(float *data, float *data_copy, float *data_row, float *data_col, const size_t size_x, const size_t size_y, const size_t n_iter, const size_t filter_radius, const bool replace_nan);


// ----------------- //
// Utility functions //
// ----------------- //

// Filter size and iterations for Gaussian function
void optimal_filter_size_flt(const double sigma, size_t *filter_radius, size_t *n_iter);

#endif
