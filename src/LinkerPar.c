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

#include "LinkerPar.h"



// ----------------------------------------------------------------- //
// Declaration of private properties and methods of class LinkerPar  //
// ----------------------------------------------------------------- //

class LinkerPar
{
	size_t    size;
	size_t    block_size;
	size_t   *label;
	size_t   *n_pix;
	uint16_t *x_min;
	uint16_t *x_max;
	uint16_t *y_min;
	uint16_t *y_max;
	uint16_t *z_min;
	uint16_t *z_max;
};



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

public LinkerPar *LinkerPar_new(void)
{
	LinkerPar *this = (LinkerPar *)malloc(sizeof(LinkerPar));
	ensure(this != NULL, "Failed to allocate memory for LinkerPar object.");
	
	this->size       = 0;
	this->block_size = 1024;
	
	this->label = (size_t   *)malloc(this->block_size * sizeof(size_t));
	this->n_pix = (size_t   *)malloc(this->block_size * sizeof(size_t));
	this->x_min = (uint16_t *)malloc(this->block_size * sizeof(uint16_t));
	this->x_max = (uint16_t *)malloc(this->block_size * sizeof(uint16_t));
	this->y_min = (uint16_t *)malloc(this->block_size * sizeof(uint16_t));
	this->y_max = (uint16_t *)malloc(this->block_size * sizeof(uint16_t));
	this->z_min = (uint16_t *)malloc(this->block_size * sizeof(uint16_t));
	this->z_max = (uint16_t *)malloc(this->block_size * sizeof(uint16_t));
	
	ensure(this->label != NULL
		&& this->n_pix != NULL
		&& this->x_min != NULL
		&& this->x_max != NULL
		&& this->y_min != NULL
		&& this->y_max != NULL
		&& this->z_min != NULL
		&& this->z_max != NULL, "Memory allocation error while creating new LinkerPar object.");
	
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
		free(this->label);
		free(this->n_pix);
		free(this->x_min);
		free(this->x_max);
		free(this->y_min);
		free(this->y_max);
		free(this->z_min);
		free(this->z_max);
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
//   (2) x        - x-position of the new object.                    //
//   (3) y        - y-position of the new object.                    //
//   (4) z        - z-position of the new object.                    //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for adding a new object to the end of the current //
//   list pointed to by 'this'. The label will be set to 0, the num- //
//   ber of pixels to 1, and the (x, y, z) position will be used as  //
//   the initial x_min, x_max, y_min, etc. The label can later be    //
//   updated with LinkerPar_set_label(). The memory allocation of    //
//   the object will automatically be expanded if necessary.         //
// ----------------------------------------------------------------- //

public void LinkerPar_push(LinkerPar *this, const uint16_t x, const uint16_t y, const uint16_t z)
{
	// Sanity checks
	check_null(this);
	
	// Check if current block is full
	if(this->size % this->block_size == 0)
	{
		// Allocate additional memory
		this->label = (size_t   *)realloc(this->label, (this->size + this->block_size) * sizeof(size_t));
		this->n_pix = (size_t   *)realloc(this->n_pix, (this->size + this->block_size) * sizeof(size_t));
		this->x_min = (uint16_t *)realloc(this->x_min, (this->size + this->block_size) * sizeof(uint16_t));
		this->x_max = (uint16_t *)realloc(this->x_max, (this->size + this->block_size) * sizeof(uint16_t));
		this->y_min = (uint16_t *)realloc(this->y_min, (this->size + this->block_size) * sizeof(uint16_t));
		this->y_max = (uint16_t *)realloc(this->y_max, (this->size + this->block_size) * sizeof(uint16_t));
		this->z_min = (uint16_t *)realloc(this->z_min, (this->size + this->block_size) * sizeof(uint16_t));
		this->z_max = (uint16_t *)realloc(this->z_max, (this->size + this->block_size) * sizeof(uint16_t));
		
		ensure(this->label != NULL
			&& this->n_pix != NULL
			&& this->x_min != NULL
			&& this->x_max != NULL
			&& this->y_min != NULL
			&& this->y_max != NULL
			&& this->z_min != NULL
			&& this->z_max != NULL, "Memory allocation error while expanding LinkerPar object.");
	}
	
	// Insert new element at end
	this->label[this->size] = 0;
	this->n_pix[this->size] = 1;
	this->x_min[this->size] = x;
	this->x_max[this->size] = x;
	this->y_min[this->size] = y;
	this->y_max[this->size] = y;
	this->z_min[this->size] = z;
	this->z_max[this->size] = z;
	
	// Lastly, increment size counter
	this->size += 1;
	
	return;
}



// ----------------------------------------------------------------- //
// Add another pixel to an existing object in the list               //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) index    - Index of the object to be updated.               //
//   (3) x        - x-position of the new object.                    //
//   (4) y        - y-position of the new object.                    //
//   (5) z        - z-position of the new object.                    //
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
//   terminate if the index is found to be out of range.             //
// ----------------------------------------------------------------- //

public void LinkerPar_update(LinkerPar *this, const size_t index, const uint16_t x, const uint16_t y, const uint16_t z)
{
	// Sanity checks
	check_null(this);
	ensure(index < this->size, "Index out of range in LinkerPar object.");
	
	this->n_pix[index] += 1;
	if(x < this->x_min[index]) this->x_min[index] = x;
	if(x > this->x_max[index]) this->x_max[index] = x;
	if(y < this->y_min[index]) this->y_min[index] = y;
	if(y > this->y_max[index]) this->y_max[index] = y;
	if(z < this->z_min[index]) this->z_min[index] = z;
	if(z > this->z_max[index]) this->z_max[index] = z;
	
	return;
}



// ----------------------------------------------------------------- //
// Get the size of an object in x, y or z                            //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) index    - Index of the object to be updated.               //
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
//   axis or index are out of range.                                 //
// ----------------------------------------------------------------- //

public size_t LinkerPar_get_size(const LinkerPar *this, const size_t index, const int axis)
{
	// Sanity checks
	check_null(this);
	ensure(index < this->size, "Index out of range in LinkerPar object.");
	ensure(axis >= 0 && axis <= 2, "Invalid axis in LinkerPar object.");
	
	if(axis == 0) return this->x_max[index] - this->x_min[index] + 1;
	if(axis == 1) return this->y_max[index] - this->y_min[index] + 1;
	return this->z_max[index] - this->z_min[index] + 1;
}



// ----------------------------------------------------------------- //
// Set the label of an object                                        //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) index    - Index of the object to be updated.               //
//   (3) label    - Label to be set.                                 //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for setting the label of the object specified by  //
//   its index. The programme will terminate if the index is found   //
//   to be out of range.                                             //
// ----------------------------------------------------------------- //

public void LinkerPar_set_label(LinkerPar *this, const size_t index, const size_t label)
{
	// Sanity checks
	check_null(this);
	ensure(index < this->size, "Index out of range in LinkerPar object.");
	
	this->label[index] = label;
	
	return;
}



// ----------------------------------------------------------------- //
// Get label of an object                                            //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) index    - Index of the object to be retrieved.             //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Label of the specified object.                                  //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning the label of an object specified by //
//   its index. The programme will terminate if the index is found   //
//   to be out of range.                                             //
// ----------------------------------------------------------------- //

public size_t LinkerPar_get_label(const LinkerPar *this, const size_t index)
{
	// Sanity checks
	check_null(this);
	ensure(index < this->size, "Index out of range in LinkerPar object.");
	
	return this->label[index];
}



// ----------------------------------------------------------------- //
// Remove objects without label                                      //
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
//   Public method for reducing the object list to only those ob-    //
//   jects that have a label greater than 0. All other, unlabelled   //
//   objects will be removed, and the memory requirements will auto- //
//   matically be adjusted to fit the reduced list.                  //
// ----------------------------------------------------------------- //

public void LinkerPar_reduce(LinkerPar *this)
{
	// Sanity checks
	check_null(this);
	
	// Counter for adjusted size
	size_t size_new = 0;
	
	// Move labelled object to beginning of list
	for(size_t i = 0; i < this->size; ++i)
	{
		if(this->label[i])
		{
			if(i > size_new)
			{
				this->label[size_new] = this->label[i];
				this->n_pix[size_new] = this->n_pix[i];
				this->x_min[size_new] = this->x_min[i];
				this->x_max[size_new] = this->x_max[i];
				this->y_min[size_new] = this->y_min[i];
				this->y_max[size_new] = this->y_max[i];
				this->z_min[size_new] = this->z_min[i];
				this->z_max[size_new] = this->z_max[i];
			}
			
			size_new += 1;
		}
	}
	
	// Update size to new, reduced list size
	this->size = size_new;
	
	if(this->size)
	{
		// Calculate how many memory blocks we need for the reduced list
		size_t arr_size = (this->size / this->block_size) * this->block_size;
		if(this->size % this->block_size) arr_size += this->block_size;
		
		// Reallocate memory as needed
		this->label = (size_t   *)realloc(this->label, arr_size * sizeof(size_t));
		this->n_pix = (size_t   *)realloc(this->n_pix, arr_size * sizeof(size_t));
		this->x_min = (uint16_t *)realloc(this->x_min, arr_size * sizeof(uint16_t));
		this->x_max = (uint16_t *)realloc(this->x_max, arr_size * sizeof(uint16_t));
		this->y_min = (uint16_t *)realloc(this->y_min, arr_size * sizeof(uint16_t));
		this->y_max = (uint16_t *)realloc(this->y_max, arr_size * sizeof(uint16_t));
		this->z_min = (uint16_t *)realloc(this->z_min, arr_size * sizeof(uint16_t));
		this->z_max = (uint16_t *)realloc(this->z_max, arr_size * sizeof(uint16_t));
		
		ensure(this->label != NULL
			&& this->n_pix != NULL
			&& this->x_min != NULL
			&& this->x_max != NULL
			&& this->y_min != NULL
			&& this->y_max != NULL
			&& this->z_min != NULL
			&& this->z_max != NULL, "Memory allocation error while reducing LinkerPar object.");
	}
	else
	{
		// No objects left; de-allocate all memory
		free(this->label);
		free(this->n_pix);
		free(this->x_min);
		free(this->x_max);
		free(this->y_min);
		free(this->y_max);
		free(this->z_min);
		free(this->z_max);
		
		// Set all pointers to NULL
		this->label = NULL;
		this->n_pix = NULL;
		this->x_min = NULL;
		this->x_max = NULL;
		this->y_min = NULL;
		this->y_max = NULL;
		this->z_min = NULL;
		this->z_max = NULL;
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Create source catalogue from LinkerPar object                     //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
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

public Catalog *LinkerPar_make_catalog(const LinkerPar *this)
{
	// Sanity checks
	check_null(this);
	
	// Create an empty source catalogue
	Catalog *cat = Catalog_new();
	
	// Loop over all LinkerPar entries
	for(size_t i = 0; i < this->size; ++i)
	{
		// Create a new source
		Source *src = Source_new();
		
		// Set the identifier to the current label
		char name[16];
		int_to_str(name, strlen(name), this->label[i]);
		Source_set_identifier(src, name);
		
		// Set other parameters
		Source_add_par_int(src, "id",    this->label[i], "",   "meta.id");
		Source_add_par_int(src, "x_min", this->x_min[i], "px", "pos.cartesian.x;stat.min");
		Source_add_par_int(src, "x_max", this->x_max[i], "px", "pos.cartesian.x;stat.max");
		Source_add_par_int(src, "y_min", this->y_min[i], "px", "pos.cartesian.y;stat.min");
		Source_add_par_int(src, "y_max", this->y_max[i], "px", "pos.cartesian.y;stat.max");
		Source_add_par_int(src, "z_min", this->z_min[i], "px", "pos.cartesian.z;stat.min");
		Source_add_par_int(src, "z_max", this->z_max[i], "px", "pos.cartesian.z;stat.max");
		Source_add_par_int(src, "n_pix", this->n_pix[i], "",   "meta.number;instr.pixel");
		
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
	
	// Calculate array size
	size_t arr_size = (this->size / this->block_size) * this->block_size;
	if(this->size % this->block_size) arr_size += this->block_size;
	
	// Print size and memory information
	message("Linker status:");
	message(" - No. of objects: %zu", this->size);
	message(" - Memory usage:   %.2f MB\n", (double)(arr_size * (2 * sizeof(size_t) + 6 * sizeof(uint16_t))) / (1024.0 * 1024.0));
	
	return;
}
