/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (LinkerPar.c) - Source Finding Application          ///
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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "LinkerPar.h"



// ----------------------------------------------------------------- //
// Declaration of private properties and methods of class LinkerPar  //
// ----------------------------------------------------------------- //

class LinkerPar
{
	size_t  size;
	size_t *label;
	size_t *n_pix;
	size_t *x_min;
	size_t *x_max;
	size_t *y_min;
	size_t *y_max;
	size_t *z_min;
	size_t *z_max;
	double *x_ctr;
	double *y_ctr;
	double *z_ctr;
	double *f_min;
	double *f_max;
	double *f_sum;
	int     verbosity;
};

private size_t LinkerPar_get_index(const LinkerPar *this, const size_t label);
private void   LinkerPar_reallocate_memory(LinkerPar *this);



// ----------------------------------------------------------------- //
// Standard constructor                                              //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   No arguments.                                                   //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Pointer to newly created LinkerPar object.                      //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Standard constructor. Will create a new and empty LinkerPar ob- //
//   ject and return a pointer to the newly created object. No me-   //
//   mory will be allocated other than for the object itself. Note   //
//   that the destructor will need to be called explicitly once the  //
//   object is no longer required to release any memory allocated    //
//   during the lifetime of the object.                              //
// ----------------------------------------------------------------- //

public LinkerPar *LinkerPar_new(const bool verbosity)
{
	LinkerPar *this = (LinkerPar *)malloc(sizeof(LinkerPar));
	ensure(this != NULL, "Failed to allocate memory for LinkerPar object.");
	
	this->verbosity = verbosity;
	this->size = 0;
	
	this->label = NULL;
	this->n_pix = NULL;
	this->x_min = NULL;
	this->x_max = NULL;
	this->y_min = NULL;
	this->y_max = NULL;
	this->z_min = NULL;
	this->z_max = NULL;
	this->x_ctr = NULL;
	this->y_ctr = NULL;
	this->z_ctr = NULL;
	this->f_min = NULL;
	this->f_max = NULL;
	this->f_sum = NULL;
	
	return this;
}



// ----------------------------------------------------------------- //
// Destructor                                                        //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Destructor. Note that the destructor must be called explicitly  //
//   if the object is no longer required. This will release the me-  //
//   mory occupied by the object.                                    //
// ----------------------------------------------------------------- //

public void LinkerPar_delete(LinkerPar *this)
{
	if(this != NULL)
	{
		this->size = 0;
		LinkerPar_reallocate_memory(this);
		free(this);
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Insert a new object at the end of the current list                //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) label    - Label of the new object.                         //
//   (3) x        - x-position of the new object.                    //
//   (4) y        - y-position of the new object.                    //
//   (5) z        - z-position of the new object.                    //
//   (6) flux     - Flux value of the new object.                    //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for adding a new object to the end of the current //
//   list pointed to by 'this'. The label will be set to 'label',    //
//   the number of pixels to 1, and the (x, y, z) position will be   //
//   used as the initial x_min, x_max, etc. The memory allocation of //
//   the object will automatically be expanded if necessary.         //
// ----------------------------------------------------------------- //

public void LinkerPar_push(LinkerPar *this, const size_t label, const size_t x, const size_t y, const size_t z, const double flux)
{
	// Sanity checks
	check_null(this);
	
	// Increment size counter
	this->size += 1;
	
	// Allocate additional memory
	LinkerPar_reallocate_memory(this);
	
	// Insert new element at end
	this->label[this->size - 1] = label;
	this->n_pix[this->size - 1] = 1;
	this->x_min[this->size - 1] = x;
	this->x_max[this->size - 1] = x;
	this->y_min[this->size - 1] = y;
	this->y_max[this->size - 1] = y;
	this->z_min[this->size - 1] = z;
	this->z_max[this->size - 1] = z;
	this->x_ctr[this->size - 1] = flux * x;
	this->y_ctr[this->size - 1] = flux * y;
	this->z_ctr[this->size - 1] = flux * z;
	this->f_min[this->size - 1] = flux;
	this->f_max[this->size - 1] = flux;
	this->f_sum[this->size - 1] = flux;
	
	return;
}



// ----------------------------------------------------------------- //
// Remove last object from list                                      //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for remove the most recently added object from    //
//   the list. The process will be terminated if the original list   //
//   is empty.                                                       //
// ----------------------------------------------------------------- //

public void LinkerPar_pop(LinkerPar *this)
{
	// Sanity checks
	check_null(this);
	ensure(this->size, "Failed to pop element from empty LinkerPar object.");
	
	// Decrement size
	this->size -= 1;
	
	// Reallocate memory
	LinkerPar_reallocate_memory(this);
	
	return;
}



// ----------------------------------------------------------------- //
// Add another pixel to an existing object in the list               //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) label    - Label of the object to be updated.               //
//   (3) x        - x-position of the new pixel.                     //
//   (4) y        - y-position of the new pixel.                     //
//   (5) z        - z-position of the new pixel.                     //
//   (6) flux     - Flux value of the new pixel.                     //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for adding another pixel to an existing object in //
//   the current linker list via that object's index. The object's   //
//   x_min, x_max, y_min, etc. values will be checked against the    //
//   newly added pixel and updated if necessary. The programme will  //
//   terminate if the label is found to be out of range.             //
// ----------------------------------------------------------------- //

public void LinkerPar_update(LinkerPar *this, const size_t label, const size_t x, const size_t y, const size_t z, const double flux)
{
	// Sanity checks
	check_null(this);
	
	// Determine index
	size_t index = LinkerPar_get_index(this, label);
	
	this->n_pix[index] += 1;
	if(x < this->x_min[index]) this->x_min[index] = x;
	if(x > this->x_max[index]) this->x_max[index] = x;
	if(y < this->y_min[index]) this->y_min[index] = y;
	if(y > this->y_max[index]) this->y_max[index] = y;
	if(z < this->z_min[index]) this->z_min[index] = z;
	if(z > this->z_max[index]) this->z_max[index] = z;
	this->x_ctr[index] += flux * x;
	this->y_ctr[index] += flux * y;
	this->z_ctr[index] += flux * z;
	if(flux > this->f_max[index]) this->f_max[index] = flux;
	if(flux < this->f_min[index]) this->f_min[index] = flux;
	this->f_sum[index] += flux;
	
	return;
}



// ----------------------------------------------------------------- //
// Get the size of an object in x, y or z                            //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) label    - Index of the object to be updated.               //
//   (3) axis     - Axis for which size should be returned; 0 = x,   //
//                  1 = y, and 2 = z.                                //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Size of the object in pixels along the specified axis.          //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning the size of the specified object    //
//   along the specified axis. The programme will terminate if the   //
//   axis or label are out of range.                                 //
// ----------------------------------------------------------------- //

public size_t LinkerPar_get_size(const LinkerPar *this, const size_t label, const int axis)
{
	// Sanity checks
	check_null(this);
	ensure(axis >= 0 && axis <= 2, "Invalid axis in LinkerPar object.");
	
	// Determine index
	size_t index = LinkerPar_get_index(this, label);
	
	if(axis == 0) return this->x_max[index] - this->x_min[index] + 1;
	if(axis == 1) return this->y_max[index] - this->y_min[index] + 1;
	return this->z_max[index] - this->z_min[index] + 1;
}



// ----------------------------------------------------------------- //
// Get the total flux of an object                                   //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) label    - Label of the object to be retrieved.             //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Total flux of the specified object.                             //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning the total flux of the object speci- //
//   fied by 'label'. The programme will terminate if the label is   //
//   out of range.                                                   //
// ----------------------------------------------------------------- //

public double LinkerPar_get_flux(const LinkerPar *this, const size_t label)
{
	// Sanity checks
	check_null(this);
	
	// Determine index
	size_t index = LinkerPar_get_index(this, label);
	
	return this->f_sum[index];
}



// ----------------------------------------------------------------- //
// Get bounding box of an object                                     //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) label    - Label of the object for which the bounding box   //
//                  is to be retrieved.                              //
//   (3)-(8) x_min, x_max, y_min, y_max, z_min, z_max                //
//                - Pointer for holding the bounding box values.     //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for retrieving the bounding box of the object     //
//   with the specified label. The process will be terminated if the //
//   label does not exist.                                           //
// ----------------------------------------------------------------- //

public void LinkerPar_get_bbox(const LinkerPar *this, const size_t label, size_t *x_min, size_t *x_max, size_t *y_min, size_t *y_max, size_t *z_min, size_t *z_max)
{
	// Sanity checks
	check_null(this);
	ensure(this->size, "Empty LinkerPar object provided.");
	
	// Determine index
	size_t index = LinkerPar_get_index(this, label);
	
	// Copy bounding box values
	*x_min = this->x_min[index];
	*x_max = this->x_max[index];
	*y_min = this->y_min[index];
	*y_max = this->y_max[index];
	*z_min = this->z_min[index];
	*z_max = this->z_max[index];
	
	return;
}



// ----------------------------------------------------------------- //
// Create source catalogue from LinkerPar object                     //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this      - Object self-reference.                          //
//   (2) flux_unit - String containing the flux unit of the data.    //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Pointer to newly created catalogue.                             //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for generating a source catalogue from the speci- //
//   fied LinkerPar object. A pointer to the newly created catalogue //
//   will be returned. Note that the user will assume ownership of   //
//   the catalogue and will be responsible for explicitly calling    //
//   the destructor if the catalogue is no longer required.          //
// ----------------------------------------------------------------- //

public Catalog *LinkerPar_make_catalog(const LinkerPar *this, const char *flux_unit)
{
	// Sanity checks
	check_null(this);
	ensure(this->size, "Empty LinkerPar object provided.");
	
	// Create an empty source catalogue
	Catalog *cat = Catalog_new();
	
	// Loop over all LinkerPar entries
	for(size_t i = 0; i < this->size; ++i)
	{
		// Create a new source
		Source *src = Source_new(this->verbosity);
		
		// Set the identifier to the current label
		char name[16];
		int_to_str(name, strlen(name), this->label[i]);
		Source_set_identifier(src, name);
		
		// Set other parameters
		Source_add_par_int(src, "id",    this->label[i],                  "-",       "meta.id");
		Source_add_par_flt(src, "x",     this->x_ctr[i] / this->f_sum[i], "pix",     "pos.cartesian.x");
		Source_add_par_flt(src, "y",     this->y_ctr[i] / this->f_sum[i], "pix",     "pos.cartesian.y");
		Source_add_par_flt(src, "z",     this->z_ctr[i] / this->f_sum[i], "pix",     "pos.cartesian.z");
		Source_add_par_int(src, "x_min", this->x_min[i],                  "pix",     "pos.cartesian.x;stat.min");
		Source_add_par_int(src, "x_max", this->x_max[i],                  "pix",     "pos.cartesian.x;stat.max");
		Source_add_par_int(src, "y_min", this->y_min[i],                  "pix",     "pos.cartesian.y;stat.min");
		Source_add_par_int(src, "y_max", this->y_max[i],                  "pix",     "pos.cartesian.y;stat.max");
		Source_add_par_int(src, "z_min", this->z_min[i],                  "pix",     "pos.cartesian.z;stat.min");
		Source_add_par_int(src, "z_max", this->z_max[i],                  "pix",     "pos.cartesian.z;stat.max");
		Source_add_par_int(src, "n_pix", this->n_pix[i],                  "-",       "meta.number;instr.pixel");
		Source_add_par_flt(src, "f_min", this->f_min[i],                  flux_unit, "phot.flux.density;stat.min");
		Source_add_par_flt(src, "f_max", this->f_max[i],                  flux_unit, "phot.flux.density;stat.max");
		Source_add_par_flt(src, "f_sum", this->f_sum[i],                  flux_unit, "phot.flux");
		
		// Add source to catalogue
		Catalog_add_source(cat, src);
	}
	
	// Return catalogue
	return cat;
}



// ----------------------------------------------------------------- //
// Print some basic information about the LinkerPar object           //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for printing some basic information on the size   //
//   and memory usage of the LinkerPar object pointed to by 'this'.  //
// ----------------------------------------------------------------- //

public void LinkerPar_print_info(const LinkerPar *this)
{
	// Sanity checks
	check_null(this);
	
	// Calculate memory usage
	const double memory_usage = (double)(this->size * (8 * sizeof(size_t) + 6 * sizeof(double))) / 1024.0;  // in kB
	
	// Print size and memory information
	message("Linker status:");
	message(" - No. of objects:  %zu", this->size);
	if(memory_usage < 1000.0) message(" - Memory usage:    %.2f kB\n", memory_usage);
	else message(" - Memory usage:    %.2f MB\n", memory_usage / 1024.0);
	
	return;
}



// ----------------------------------------------------------------- //
// Return the index of the element identified by label               //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) label    - Label of the element the index of which is to be //
//                  returned.                                        //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Index of the element identified by 'label'.                     //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Private method for finding and returning the index of the ele-  //
//   ment with the specified label. The process will be terminated   //
//   if the requested label does not exist.                          //
// ----------------------------------------------------------------- //

private size_t LinkerPar_get_index(const LinkerPar *this, const size_t label)
{
	size_t index = this->size - 1;
	while(index > 0 && this->label[index] != label) --index;
	ensure(this->label[index] == label, "Label not found.");
	return index;
}



// ----------------------------------------------------------------- //
// Reallocate memory for LinkerPar object                            //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Private method for reallocating the memory requirements of the  //
//   specified LinkerPar object, e.g. as necessitated by a change in //
//   size. If the new size is 0, all memory will be de-allocated and //
//   the pointers will be set to NULL.                               //
// ----------------------------------------------------------------- //

private void LinkerPar_reallocate_memory(LinkerPar *this)
{
	if(this->size)
	{
	// Reallocate memory
	this->label = (size_t *)realloc(this->label, this->size * sizeof(size_t));
	this->n_pix = (size_t *)realloc(this->n_pix, this->size * sizeof(size_t));
	this->x_min = (size_t *)realloc(this->x_min, this->size * sizeof(size_t));
	this->x_max = (size_t *)realloc(this->x_max, this->size * sizeof(size_t));
	this->y_min = (size_t *)realloc(this->y_min, this->size * sizeof(size_t));
	this->y_max = (size_t *)realloc(this->y_max, this->size * sizeof(size_t));
	this->z_min = (size_t *)realloc(this->z_min, this->size * sizeof(size_t));
	this->z_max = (size_t *)realloc(this->z_max, this->size * sizeof(size_t));
	this->x_ctr = (double *)realloc(this->x_ctr, this->size * sizeof(double));
	this->y_ctr = (double *)realloc(this->y_ctr, this->size * sizeof(double));
	this->z_ctr = (double *)realloc(this->z_ctr, this->size * sizeof(double));
	this->f_min = (double *)realloc(this->f_min, this->size * sizeof(double));
	this->f_max = (double *)realloc(this->f_max, this->size * sizeof(double));
	this->f_sum = (double *)realloc(this->f_sum, this->size * sizeof(double));
	
	ensure(this->label != NULL
		&& this->n_pix != NULL
		&& this->x_min != NULL
		&& this->x_max != NULL
		&& this->y_min != NULL
		&& this->y_max != NULL
		&& this->z_min != NULL
		&& this->z_max != NULL
		&& this->x_ctr != NULL
		&& this->y_ctr != NULL
		&& this->z_ctr != NULL
		&& this->f_min != NULL
		&& this->f_max != NULL
		&& this->f_sum != NULL, "Memory allocation error while modifying LinkerPar object.");
	}
	else
	{
		free(this->label);
		free(this->n_pix);
		free(this->x_min);
		free(this->x_max);
		free(this->y_min);
		free(this->y_max);
		free(this->z_min);
		free(this->z_max);
		free(this->x_ctr);
		free(this->y_ctr);
		free(this->z_ctr);
		free(this->f_min);
		free(this->f_max);
		free(this->f_sum);
		
		this->label = NULL;
		this->n_pix = NULL;
		this->x_min = NULL;
		this->x_max = NULL;
		this->y_min = NULL;
		this->y_max = NULL;
		this->z_min = NULL;
		this->z_max = NULL;
		this->x_ctr = NULL;
		this->y_ctr = NULL;
		this->z_ctr = NULL;
		this->f_min = NULL;
		this->f_max = NULL;
		this->f_sum = NULL;
	}
	
	return;
}



// Reliability filtering (### WORK IN PROGRESS ###)

public void LinkerPar_reliability(LinkerPar *this)
{
	// Sanity checks
	check_null(this);
	ensure(this->size, "No sources left after linking. Cannot proceed.");
	
	// Dimensionality of parameter space
	const int dim = 3;
	
	// Define a few parameters
	size_t n_neg = 0;
	size_t n_pos = 0;
	size_t counter_pos = 0;
	size_t counter_neg = 0;
	
	// Check number of positive and negative detections
	for(size_t i = this->size; i --> 0;)
	{
		if(this->f_sum[i] < 0.0) ++n_neg;
		else ++n_pos;
	}
	
	ensure(n_neg, "No negative sources found. Cannot proceed.");
	ensure(n_pos, "No positive sources found. Cannot proceed.");
	
	// Extract relevant parameters
	double *par_pos = (double *)malloc(dim * n_pos * sizeof(double));
	double *par_neg = (double *)malloc(dim * n_neg * sizeof(double));
	ensure(par_pos != NULL && par_neg != NULL, "Memory allocation error while measuring reliability.");
	
	for(size_t i = this->size; i --> 0;)
	{
		if(this->f_sum[i] < 0.0)
		{
			par_neg[dim * counter_neg + 0] = log10(-this->f_min[i]);
			par_neg[dim * counter_neg + 1] = log10(-this->f_sum[i]);
			par_neg[dim * counter_neg + 2] = log10( this->n_pix[i]);
			++counter_neg;
		}
		else
		{
			par_pos[dim * counter_pos + 0] = log10(-this->f_min[i]);
			par_pos[dim * counter_pos + 1] = log10(-this->f_sum[i]);
			par_pos[dim * counter_pos + 2] = log10( this->n_pix[i]);
			++counter_pos;
		}
	}
	
	// Determine covariance matrix from negative detections
	double covar[dim][dim];
	double mean[dim];
	const double scale_kernel = 0.25; // ALERT: scale_kernel to be provided by the user!
	
	// Calculate mean values first
	for(size_t i = 0; i < dim; ++i)
	{
		mean[i] = 0.0;
		for(size_t j = 0; j < n_neg; ++j) mean[i] += par_neg[dim * j + i];
		mean[i] /= n_neg;
	}
	
	// Then calculate the covariance
	for(size_t i = 0; i < dim; ++i)
	{
		for(size_t j = 0; j < dim; ++j)
		{
			covar[i][j] = 0.0;
			for(size_t k = 0; k < n_neg; ++k) covar[i][j] += (par_neg[dim * k + i] - mean[i]) * (par_neg[dim * k + j] - mean[j]);
			covar[i][j] *= scale_kernel / n_neg;
		}
	}
	
	// ALERT: Some sanity checks on the covariance matrix might be desirable here...
	
	// ### CONTINUE HERE WITH IMPLEMENTATION OF RELIABILITY MEASUREMENT...
	
	// Release memory again
	free(par_pos);
	free(par_neg);
	
	return;
}
