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
	double *rel;
	int     verbosity;
};

private size_t LinkerPar_get_index(const LinkerPar *this, const size_t label);
private void   LinkerPar_reallocate_memory(LinkerPar *this);
private void   LinkerPar_ps_header(FILE *fp);
private void   LinkerPar_ps_footer(FILE *fp);



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
	this->rel   = NULL;
	
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
// Return number of sources in LinkerPar object                      //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Number of sources stored in LinkerPar object.                   //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning the size of the LinkerPar object,   //
//   i.e. the number of sources it currently contains.               //
// ----------------------------------------------------------------- //

public size_t LinkerPar_get_size(const LinkerPar *this)
{
	check_null(this);
	return this->size;
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
	this->rel  [this->size - 1] = 0.0;  // Must be 0 (default for neg. sources), as only pos. sources will be updated later!
	
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
//   (2) label    - Index of the object to be retrieved.             //
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

public size_t LinkerPar_get_obj_size(const LinkerPar *this, const size_t label, const int axis)
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
// Get the number of pixels of an object                             //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) label    - Index of the object to be retrieved.             //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Number of pixels of the specified object.                       //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning the number of pixels that have been //
//   recorded for the specified object. The programme will terminate //
//   if the label is out of range.                                   //
// ----------------------------------------------------------------- //

public size_t LinkerPar_get_npix(const LinkerPar *this, const size_t label)
{
	// Sanity checks
	check_null(this);
	
	// Determine index
	size_t index = LinkerPar_get_index(this, label);
	
	return this->n_pix[index];
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
// Get the reliability of an object                                  //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) label    - Label of the object to be retrieved.             //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Reliability of the specified object.                            //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning the reliability of the object spe-  //
//   cified by 'label'. The programme will terminate if the label is //
//   out of range.                                                   //
// ----------------------------------------------------------------- //

public double LinkerPar_get_rel(const LinkerPar *this, const size_t label)
{
	// Sanity checks
	check_null(this);
	
	// Determine index
	size_t index = LinkerPar_get_index(this, label);
	
	return this->rel[index];
}



// ----------------------------------------------------------------- //
// Get the label of an object by index                               //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) index    - Index of the object to be retrieved.             //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Reliability of the specified object.                            //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning the label of the object with the    //
//   specified index. The programme will terminate if the index is   //
//   out of range.                                                   //
// ----------------------------------------------------------------- //

public size_t LinkerPar_get_label(const LinkerPar *this, const size_t index)
{
	// Sanity checks
	check_null(this);
	ensure(index < this->size, "Index out of range. Cannot retrieve label.");
	
	return this->label[index];
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
//   (2) filter    - Map object containing old and new label pairs   //
//                   of only those sources considered as reliable.   //
//   (3) flux_unit - String containing the flux unit of the data.    //
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
//   Unreliable sources can be excluded from the catalogue by provi- //
//   ding a non-empty filter object that contains the old and new    //
//   labels of all sources deemed reliable. The old labels will be   //
//   replaced by the new ones in the source ID column of the cata-   //
//   logue. If filter is NULL or empty, all sources will be copied   //
//   into the catalogue without filtering.                           //
// ----------------------------------------------------------------- //

public Catalog *LinkerPar_make_catalog(const LinkerPar *this, const Map *filter, const char *flux_unit)
{
	// Sanity checks
	check_null(this);
	ensure(this->size, "Empty LinkerPar object provided.");
	check_null(flux_unit);
	
	// Check if reliability filtering requested
	const bool remove_unreliable = filter != NULL && Map_get_size(filter);
	
	// Create an empty source catalogue
	Catalog *cat = Catalog_new();
	
	// Loop over all LinkerPar entries
	for(size_t i = 0; i < this->size; ++i)
	{
		const size_t new_label = remove_unreliable && Map_key_exists(filter, this->label[i]) ? Map_get_value(filter, this->label[i]) : this->label[i];
		
		if(!remove_unreliable || Map_key_exists(filter, this->label[i]))
		{
			// Create a new source
			Source *src = Source_new(this->verbosity);
			
			// Set the identifier to the current label
			char name[16];
			int_to_str(name, strlen(name), this->label[i]);
			Source_set_identifier(src, name);
			
			// Set other parameters
			Source_add_par_int(src, "id",    new_label,                       "-",       "meta.id");
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
			Source_add_par_flt(src, "rel",   this->rel  [i],                  "-",       "stat.probability");
			
			// Add source to catalogue
			Catalog_add_source(cat, src);
		}
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
	const double memory_usage = (double)(this->size * (8 * sizeof(size_t) + 7 * sizeof(double))) / 1024.0;  // in kB
	
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
	this->rel   = (double *)realloc(this->rel,   this->size * sizeof(double));
	
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
		&& this->f_sum != NULL
		&& this->rel   != NULL, "Memory allocation error while modifying LinkerPar object.");
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
		free(this->rel);
		
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
		this->rel   = NULL;
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Determine reliability of detections                               //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this         - Object self-reference.                       //
//   (2) scale_kernel - The size of the convolution kernel used in   //
//                      determining the density of positive and ne-  //
//                      gative detections in parameter space will be //
//                      scaled by this factor. If set to 1, the ori- //
//                      ginal covariance matrix derived from the     //
//                      distribution of negative sources is used.    //
//   (3) rms          - Global rms noise of the data. All parameters //
//                      will be normalised by the rms.               //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Covariance matrix from the negative detections.                 //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for measuring the reliability of all the sources  //
//   in the specified LinkerPar object. This will set the rel pro-   //
//   perty of the object, but not yet filter out unreliable sources. //
//   Reliability measurement works by comparing the density of posi- //
//   tive and negative detections in an N-dimensional parameter      //
//   space. For this purpose, the covariance matrix of the distribu- //
//   tion of negative sources in parameter space is first calcula-   //
//   ted. The covariance matrix is assumed to describe the multi-    //
//   variate normal distribution of the Gaussian noise of the data.  //
//   Next, the sum of the probability density functions of all posi- //
//   tive and negative sources is evaluated at the location of each  //
//   positive detection (multivariate Gaussian kernel density esti-  //
//   mation). From this, the reliability is estimated as             //
//                                                                   //
//      R = (P - N) / N,                                             //
//                                                                   //
//   where P is the sum of the PDFs of the positive sources, and N   //
//   is the sum of the PDFs of the negative sources. If N > P, R is  //
//   set to 0 to ensure that the resulting reliability is always in  //
//   the range of 0 to 1.                                            //
// ----------------------------------------------------------------- //

public Matrix *LinkerPar_reliability(LinkerPar *this, const double scale_kernel, const double rms)
{
	// Sanity checks
	check_null(this);
	ensure(this->size, "No sources left after linking. Cannot proceed.");
	
	// Dimensionality of parameter space
	const int dim = 3;
	
	// Define a few parameters
	size_t n_neg = 0;
	size_t n_pos = 0;
	size_t counter_neg = 0;
	size_t counter_pos = 0;
	const size_t threshold_warning = 50;
	Matrix *vector = Matrix_new(dim, 1);
	
	// Check number of positive and negative detections
	for(size_t i = this->size; i--;)
	{
		if(this->f_sum[i] < 0.0) ++n_neg;
		else ++n_pos;
	}
	
	ensure(n_neg, "No negative sources found. Cannot proceed.");
	ensure(n_pos, "No positive sources found. Cannot proceed.");
	message("Found %zu positive and %zu negative sources.\n", n_pos, n_neg);
	if(n_neg < threshold_warning) warning("Only %zu negative detections found.\n         Reliability calculation may not be accurate.", n_neg);
	
	// Extract relevant parameters
	double *par_pos = (double *)malloc(dim * n_pos * sizeof(double));
	size_t *idx_pos = (size_t *)malloc(n_pos * sizeof(size_t));
	double *par_neg = (double *)malloc(dim * n_neg * sizeof(double));
	size_t *idx_neg = (size_t *)malloc(n_neg * sizeof(size_t));
	ensure(par_pos != NULL && par_neg != NULL && idx_pos != NULL && idx_neg != NULL, "Memory allocation error while measuring reliability.");
	
	for(size_t i = this->size; i--;)
	{
		if(this->f_sum[i] < 0.0)
		{
			par_neg[dim * counter_neg + 0] = log10(-this->f_min[i] / rms);
			par_neg[dim * counter_neg + 1] = log10(-this->f_sum[i] / rms);
			par_neg[dim * counter_neg + 2] = log10(-this->f_sum[i] / (rms * this->n_pix[i]));
			idx_neg[counter_neg] = i;
			++counter_neg;
		}
		else
		{
			par_pos[dim * counter_pos + 0] = log10(this->f_max[i] / rms);
			par_pos[dim * counter_pos + 1] = log10(this->f_sum[i] / rms);
			par_pos[dim * counter_pos + 2] = log10(this->f_sum[i] / (rms * this->n_pix[i]));
			idx_pos[counter_pos] = i;
			++counter_pos;
		}
	}
	
	// Determine covariance matrix from negative detections
	Matrix *covar = Matrix_new(dim, dim);
	double mean[dim];
	
	// Calculate mean values first
	for(size_t i = dim; i--;)
	{
		mean[i] = 0.0;
		for(size_t j = 0; j < n_neg; ++j) mean[i] += par_neg[dim * j + i];
		mean[i] /= n_neg;
	}
	
	// Then calculate the covariance matrix
	for(size_t i = dim; i--;)
	{
		for(size_t j = dim; j--;)
		{
			for(size_t k = 0; k < n_neg; ++k)
			{
				Matrix_add_value(covar, i, j, (par_neg[dim * k + i] - mean[i]) * (par_neg[dim * k + j] - mean[j]));
			}
			Matrix_mul_value(covar, i, j, scale_kernel * scale_kernel / n_neg);  // NOTE: Variance = sigma^2, hence scale_kernel^2 here.
		}
	}
	
	// Invert + sanity checks
	Matrix *covar_inv = Matrix_invert(covar);
	ensure(covar_inv != NULL, "Covariance matrix is not invertible; cannot measure reliability.\n       Ensure that there are enough negative detections.");
	
	// Inverse of the square root of the determinant of 2 * pi * covar
	// This is the scale factor needed to calculate the PDF of the multivariate normal distribution later on.
	const double scal_fact = 1.0 / sqrt(Matrix_det(covar, 2.0 * M_PI));
	
	// Loop over all positive detections to measure their reliability
	const size_t cadence = n_pos / 100 ? n_pos / 100 : 1;
	for(size_t i = 0; i < n_pos; ++i)
	{
		if(i % cadence == 0 || i == n_pos - 1) progress_bar("Progress: ", i, n_pos - 1);
		
		const double p1 = par_pos[dim * i + 0];
		const double p2 = par_pos[dim * i + 1];
		const double p3 = par_pos[dim * i + 2];
		
		// Multivariate kernel density estimation for negative detections
		double pdf_neg_sum = 0.0;
		
		for(size_t j = n_neg; j--;)
		{
			// Set up relative position vector
			Matrix_set_value(vector, 0, 0, par_neg[dim * j + 0] - p1);
			Matrix_set_value(vector, 1, 0, par_neg[dim * j + 1] - p2);
			Matrix_set_value(vector, 2, 0, par_neg[dim * j + 2] - p3);
			
			pdf_neg_sum += Matrix_prob_dens(covar_inv, vector, scal_fact);
		}
		
		// Same for positive detections
		double pdf_pos_sum = 0.0;
		
		for(size_t j = n_pos; j--;)
		{
			// Set up relative position vector
			Matrix_set_value(vector, 0, 0, par_pos[dim * j + 0] - p1);
			Matrix_set_value(vector, 1, 0, par_pos[dim * j + 1] - p2);
			Matrix_set_value(vector, 2, 0, par_pos[dim * j + 2] - p3);
			
			pdf_pos_sum += Matrix_prob_dens(covar_inv, vector, scal_fact);
		}
		
		// Calculate reliability
		const double rel = pdf_pos_sum > pdf_neg_sum ? (pdf_pos_sum - pdf_neg_sum) / pdf_pos_sum : 0.0;
		this->rel[idx_pos[i]] = rel;
	}
	
	// Release memory again
	Matrix_delete(covar_inv);
	Matrix_delete(vector);
	free(par_pos);
	free(par_neg);
	free(idx_pos);
	free(idx_neg);
	
	return covar;
}



// Make reliability plots

public void LinkerPar_rel_plots(const LinkerPar *this, const double threshold, const double fmin, const Matrix *covar, const char *filename, const bool overwrite)
{
	// Sanity checks
	check_null(this);
	if(this->size == 0)
	{
		warning("No sources found; cannot generate reliability plots.");
		return;
	}
	ensure(filename != NULL && strlen(filename), "Empty file name for reliability plot provided.");
	
	size_t plot_size_x = 300;  // pt
	size_t plot_size_y = 300;  // pt
	size_t plot_offset_x = 50;  // pt
	size_t plot_offset_y = 50;  // pt
	
	const char *par_space_x[3] = {"log\\(max / rms\\)", "log\\(max / rms\\)", "log\\(sum / rms\\)"};
	const char *par_space_y[3] = {"log\\(sum / rms\\)", "log\\(mean / rms\\)", "log\\(mean / rms\\)"};
	
	// Create arrays for parameters
	double *data_x = (double *)malloc(this->size * sizeof(double));
	double *data_y = (double *)malloc(this->size * sizeof(double));
	ensure(data_x != NULL && data_y != NULL, "Memory allocation error while creating reliability plot.");
	
	// Open PS file
	FILE *fp;
	if(overwrite) fp = fopen(filename, "wb");
	else fp = fopen(filename, "wxb");
	ensure(fp != NULL, "Failed to open output file: %s", filename);
	
	message("Creating postscript file: %s", strrchr(filename, '/') == NULL ? filename : strrchr(filename, '/') + 1);
	
	// Print PS header
	LinkerPar_ps_header(fp);
	
	for(int n = 0; n < 3; ++n)
	{
		// Read values and determine plotting range
		plot_offset_x = 50 + n * (plot_size_x + 50);
		
		double data_min_x =  999.9;
		double data_max_x = -999.9;
		double data_min_y =  999.9;
		double data_max_y = -999.9;
		
		double radius_maj, radius_min, pa;
		if(n == 0) Matrix_err_ellipse(covar, 0, 1, 0.68, &radius_maj, &radius_min, &pa);
		else if(n == 1) Matrix_err_ellipse(covar, 0, 2, 0.68, &radius_maj, &radius_min, &pa);
		else if(n == 2) Matrix_err_ellipse(covar, 1, 2, 0.68, &radius_maj, &radius_min, &pa);
		
		for(size_t i = 0; i < this->size; ++i)
		{
			// Extract relevant parameters
			switch(n)
			{
				case 0:
					if(this->f_sum[i] < 0.0)
					{
						data_x[i] = log10(-this->f_min[i]);
						data_y[i] = log10(-this->f_sum[i]);
					}
					else
					{
						data_x[i] = log10(this->f_max[i]);
						data_y[i] = log10(this->f_sum[i]);
					}
					break;
				case 1:
					if(this->f_sum[i] < 0.0)
					{
						data_x[i] = log10(-this->f_min[i]);
						data_y[i] = log10(-this->f_sum[i] / this->n_pix[i]);
					}
					else
					{
						data_x[i] = log10(this->f_max[i]);
						data_y[i] = log10(this->f_sum[i] / this->n_pix[i]);
					}
					break;
				case 2:
					if(this->f_sum[i] < 0.0)
					{
						data_x[i] = log10(-this->f_sum[i]);
						data_y[i] = log10(-this->f_sum[i] / this->n_pix[i]);
					}
					else
					{
						data_x[i] = log10(this->f_sum[i]);
						data_y[i] = log10(this->f_sum[i] / this->n_pix[i]);
					}
					break;
			}
			
			if(data_min_x > data_x[i]) data_min_x = data_x[i];
			if(data_max_x < data_x[i]) data_max_x = data_x[i];
			if(data_min_y > data_y[i]) data_min_y = data_y[i];
			if(data_max_y < data_y[i]) data_max_y = data_y[i];
		}
		
		double data_range_x = data_max_x - data_min_x;
		double data_range_y = data_max_y - data_min_y;
		
		// Add a little bit of margin
		data_min_x -= 0.05 * data_range_x;
		data_max_x += 0.05 * data_range_x;
		data_min_y -= 0.05 * data_range_y;
		data_max_y += 0.05 * data_range_y;
		
		// Ensure that ranges are equal in x and y
		/*if(data_range_x > data_range_y)
		{
			data_min_y -= 0.5 * (data_range_x - data_range_y);
			data_max_y += 0.5 * (data_range_x - data_range_y);
			data_range_y = data_max_y - data_min_y;
		}
		else if(data_range_x < data_range_y)
		{
			data_min_x -= 0.5 * (data_range_y - data_range_x);
			data_max_x += 0.5 * (data_range_y - data_range_x);
			data_range_x = data_max_x - data_min_x;
		}*/
		
		// Determine the mean of negative sources
		double mean_x = 0.0;
		double mean_y = 0.0;
		size_t counter = 0;
		
		for(size_t i = 0; i < this->size; ++i)
		{
			if(this->f_sum[i] < 0.0)
			{
				mean_x += data_x[i];
				mean_y += data_y[i];
				++counter;
			}
		}
		
		mean_x /= counter;
		mean_y /= counter;
		
		const double centre_x = (mean_x - data_min_x) * plot_size_x / (data_max_x - data_min_x) + plot_offset_x;
		const double centre_y = (mean_y - data_min_y) * plot_size_y / (data_max_y - data_min_y) + plot_offset_y;
		const double radius_x = radius_maj * plot_size_x / (data_max_x - data_min_x);
		const double radius_y = radius_min * plot_size_x / (data_max_x - data_min_x);
		
		// Determine scale factor of kernel ellipse
		const double scale_factor = data_range_x / data_range_y;
		
		// Plot negative sources
		fprintf(fp, "1 0.4 0.4 rgb\n");
		fprintf(fp, "0.5 lw\n");
		fprintf(fp, "np\n");
		
		for(size_t i = this->size; i--;)
		{
			if(this->f_sum[i] < 0.0)
			{
				const double plot_x = (data_x[i] - data_min_x) * plot_size_x / (data_max_x - data_min_x) + plot_offset_x;
				const double plot_y = (data_y[i] - data_min_y) * plot_size_y / (data_max_y - data_min_y) + plot_offset_y;
				
				fprintf(fp, "%.2f %.2f 1.5 0 360 a f\n", plot_x, plot_y);
			}
		}
		
		// Plot unreliable positive sources
		fprintf(fp, "0.4 0.4 1 rgb\n");
		
		for(size_t i = this->size; i--;)
		{
			if(this->f_sum[i] > 0.0 && this->rel[i] < threshold)
			{
				const double plot_x = (data_x[i] - data_min_x) * plot_size_x / (data_max_x - data_min_x) + plot_offset_x;
				const double plot_y = (data_y[i] - data_min_y) * plot_size_y / (data_max_y - data_min_y) + plot_offset_y;
				
				fprintf(fp, "%.2f %.2f 1.5 0 360 a f\n", plot_x, plot_y);
			}
		}
		
		// Plot reliable positive sources
		fprintf(fp, "0 0 0 rgb\n");
		
		for(size_t i = this->size; i--;)
		{
			if(this->f_sum[i] > 0.0 && this->rel[i] >= threshold)
			{
				const double plot_x = (data_x[i] - data_min_x) * plot_size_x / (data_max_x - data_min_x) + plot_offset_x;
				const double plot_y = (data_y[i] - data_min_y) * plot_size_y / (data_max_y - data_min_y) + plot_offset_y;
				
				if(this->f_sum[i] / sqrt(this->n_pix[i]) >= fmin) fprintf(fp, "%.2f %.2f 2.5 0 360 a f\n", plot_x, plot_y);
				else fprintf(fp, "%.2f %.2f 2.5 0 360 a s\n", plot_x, plot_y);
			}
		}
		
		// Plot kernel ellipse
		fprintf(fp, "gsave\n");
		fprintf(fp, "0.8 0.8 0.8 rgb\n");
		fprintf(fp, "np %zu %zu m %zu %zu l %zu %zu l %zu %zu l cp clip\n", plot_offset_x, plot_offset_y, plot_offset_x + plot_size_x, plot_offset_y, plot_offset_x + plot_size_x, plot_offset_y + plot_size_y, plot_offset_x, plot_offset_y + plot_size_y);
		fprintf(fp, "%.2f %.2f %.2f %.2f %.2f %.2f ellipse\n", centre_x, centre_y, radius_x, radius_y, 180.0 * pa / M_PI, scale_factor);
		fprintf(fp, "grestore\n");
		
		// Plot fmin line if possible
		if(n == 2)
		{
			double plot_x = plot_offset_x;
			double plot_y = (2.0 * log10(fmin) - data_min_x - data_min_y) * plot_size_y / (data_max_y - data_min_y) + plot_offset_y;
			
			fprintf(fp, "gsave\n");
			fprintf(fp, "0.5 0.5 0.5 rgb\n");
			fprintf(fp, "[3 3] 0 setdash\n");
			fprintf(fp, "np %zu %zu m %zu %zu l %zu %zu l %zu %zu l cp clip\n", plot_offset_x, plot_offset_y, plot_offset_x + plot_size_x, plot_offset_y, plot_offset_x + plot_size_x, plot_offset_y + plot_size_y, plot_offset_x, plot_offset_y + plot_size_y);
			fprintf(fp, "%.2f %.2f m\n", plot_x, plot_y);
			
			plot_x = plot_offset_x + plot_size_x;
			plot_y = (2.0 * log10(fmin) - data_max_x - data_min_y) * plot_size_y / (data_max_y - data_min_y) + plot_offset_y;
			
			fprintf(fp, "%.2f %.2f l s\n", plot_x, plot_y);
			fprintf(fp, "grestore\n");
		}
		
		// Plot frame
		fprintf(fp, "0 0 0 rgb\n");
		fprintf(fp, "[] 0 setdash\n");
		fprintf(fp, "np\n");
		fprintf(fp, "%zu %zu m\n", plot_offset_x, plot_offset_y);
		fprintf(fp, "%zu %zu l\n", plot_offset_x + plot_size_x, plot_offset_y);
		fprintf(fp, "%zu %zu l\n", plot_offset_x + plot_size_x, plot_offset_y + plot_size_y);
		fprintf(fp, "%zu %zu l\n", plot_offset_x, plot_offset_y + plot_size_y);
		fprintf(fp, "cp s\n");
		
		// Plot tick marks
		for(double tm = ceil(2.0 * data_min_x) / 2.0; tm <= data_max_x; tm += 0.5)
		{
			if(fabs(tm) < 0.1) tm = 0.0;
			const double plot_x = (tm - data_min_x) * plot_size_x / (data_max_x - data_min_x) + plot_offset_x;
			fprintf(fp, "np %.2f %zu m %.2f %zu l s\n", plot_x, plot_offset_y, plot_x, plot_offset_y + 5);
			fprintf(fp, "np %.2f %zu m (%.1f) show\n", plot_x - 8.0, plot_offset_y - 14, tm);
		}
		
		for(double tm = ceil(2.0 * data_min_y) / 2.0; tm <= data_max_y; tm += 0.5)
		{
			if(fabs(tm) < 0.1) tm = 0.0;
			const double plot_y = (tm - data_min_y) * plot_size_y / (data_max_y - data_min_y) + plot_offset_y;
			fprintf(fp, "np %zu %.2f m %zu %.2f l s\n", plot_offset_x, plot_y, plot_offset_x + 5, plot_y);
			fprintf(fp, "np %zu %.2f m (%.1f) stringwidth pop neg 0 rmoveto (%.1f) show\n", plot_offset_x - 4, plot_y - 4.0, tm, tm);
		}
		
		// Print labels
		fprintf(fp, "np %zu (%s) stringwidth pop 2 div sub 20 m\n", plot_offset_x + plot_size_x / 2, par_space_x[n]);
		fprintf(fp, "(%s) show\n", par_space_x[n]);
		
		fprintf(fp, "np %zu %zu (%s) stringwidth pop 2 div sub m\n", plot_offset_x - 34, plot_offset_y + plot_size_y / 2, par_space_y[n]);
		fprintf(fp, "gsave\n");
		fprintf(fp, "90 rotate\n");
		fprintf(fp, "(%s) show\n", par_space_y[n]);
		fprintf(fp, "grestore\n");
	}
	
	// Print PS footer
	LinkerPar_ps_footer(fp);
	
	// Close output file
	fclose(fp);
	
	// Clean up
	free(data_x);
	free(data_y);
	
	return;
}



private void LinkerPar_ps_header(FILE *fp)
{
	fprintf(fp, "%%!PS-Adobe-3.0 EPSF-3.0\n");
	fprintf(fp, "%%%%Title: SoFiA Reliability Plots\n");
	fprintf(fp, "%%%%Creator: %s\n", SOFIA_VERSION_FULL);
	fprintf(fp, "%%%%BoundingBox: 0 10 1060 360\n");
	fprintf(fp, "%%%%EndComments\n");
	fprintf(fp, "/m {moveto} bind def\n");
	fprintf(fp, "/l {lineto} bind def\n");
	fprintf(fp, "/a {arc} bind def\n");
	fprintf(fp, "/s {stroke} bind def\n");
	fprintf(fp, "/f {fill} bind def\n");
	fprintf(fp, "/rgb {setrgbcolor} bind def\n");
	fprintf(fp, "/np {newpath} bind def\n");
	fprintf(fp, "/cp {closepath} bind def\n");
	fprintf(fp, "/lw {setlinewidth} bind def\n");
	fprintf(fp, "/Helvetica findfont 12 scalefont setfont\n");
	fprintf(fp , "/ellipse {gsave /scf exch def /pa exch def /rmin exch def /rmaj exch def /posy exch def /posx exch def 0.5 setlinewidth newpath posx posy translate matrix currentmatrix 1 scf scale pa rotate 1 rmin rmaj div scale 0 0 rmaj 0 360 arc closepath setmatrix stroke grestore} bind def\n");
	return;
}



private void LinkerPar_ps_footer(FILE *fp)
{
	fprintf(fp, "showpage\n");
	fprintf(fp, "%%%%EndDocument\n");
	return;
}
