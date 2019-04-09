/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (Array.c) - Source Finding Application              ///
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
#include <string.h>

#include "Array.h"



// ----------------------------------------------------------------- //
// Declaration of private properties and methods of class Array      //
// ----------------------------------------------------------------- //

class Array
{
	size_t  size;
	double *values;
	int     type;
};



// ----------------------------------------------------------------- //
// Standard constructor                                              //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   size - Size of the array to be created.                         //
//   type - Data type to be used. Can be ARRAY_TYPE_FLT for double   //
//          or ARRAY_TYPE_INT for 64-bit signed int.                 //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Pointer to newly created Array object.                          //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Standard constructor. Will create a new Array object of given   //
//   size and type and return a pointer to the newly created object. //
//   Sufficient memory will be allocated to store the array values   //
//   of the specified type. Note that the destructor will need to be //
//   called explicitly once the object is no longer required to re-  //
//   lease any memory allocated during the lifetime of the object.   //
// ----------------------------------------------------------------- //

public Array *Array_new(const size_t size, const int type)
{
	// Sanity checks
	ensure(type == ARRAY_TYPE_FLT || type == ARRAY_TYPE_INT, "Array data type must be ARRAY_TYPE_FLT or ARRAY_TYPE_INT.");
	
	Array *this = (Array *)malloc(sizeof(Array));
	ensure(this != NULL, "Failed to allocate memory for new Array object.");
	
	this->size = size;
	this->type = type;
	
	if(size)
	{
		this->values = (double *)calloc(size, sizeof(double));
		ensure(this->values != NULL, "Failed to allocate memory for new Array object.");
	}
	else
	{
		this->values = NULL;
	}
	
	return this;
}



// ----------------------------------------------------------------- //
// Alternative constructor                                           //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   string - String containing the values to be stored in the       //
//            array, separated by commas.                            //
//   type   - Data type; can be ARRAY_TYPE_FLT for double-precision  //
//            floating-point data or ARRAY_TYPE_INT for 64-bit inte- //
//            ger data.                                              //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Pointer to newly created Array object.                          //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Alternative constructor. Will create a new Array object of spe- //
//   cified data type, the size of which is determined by the number //
//   of comma-separated values specified in 'string'. A pointer to   //
//   the newly created object will be returned. Sufficient memory    //
//   will be allocated to store the array values as either 64-bit    //
//   double-precision floating-point data or 64-bit signed integer   //
//   data. Note that the destructor will need to be called explicit- //
//   ly once the object is no longer required to release any memory  //
//   allocated during the lifetime of the object.                    //
// ----------------------------------------------------------------- //

public Array *Array_new_str(char *string, const int type)
{
	// Sanity checks
	check_null(string);
	ensure(strlen(string), "Empty string supplied to Array object constructor.");
	
	// Create a copy of the string
	char *copy = (char *)malloc((strlen(string) + 1) * sizeof(char));
	ensure(copy != NULL, "Memory allocation error during array creation.");
	strcpy(copy, string);
	
	// Count number of commas
	size_t size = 1;
	size_t i = strlen(copy);
	while(i--) if(copy[i] == ',') ++size;
	
	// Create array of given size
	Array *this = Array_new(size, type);
	
	// Fill array with values
	char *token = strtok(copy, ",");
	ensure(token != NULL, "Failed to parse string as array.");
	
	if(this->type == ARRAY_TYPE_FLT) this->values[0] = strtod(token, NULL);
	else *((int64_t *)(&this->values[0])) = (int64_t)strtol(token, NULL, 10);
	
	for(i = 1; i < size; ++i)
	{
		token = strtok(NULL, ",");
		ensure(token != NULL, "Failed to parse string as array.");
		
		if(this->type == ARRAY_TYPE_FLT) this->values[i] = strtod(token, NULL);
		else *((int64_t *)(&this->values[i])) = (int64_t)strtol(token, NULL, 10);
	}
	
	// Delete string copy again
	free(copy);
	
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

public void Array_delete(Array *this)
{
	if(this != NULL) free(this->values);
	free(this);
	
	return;
}



// ----------------------------------------------------------------- //
// Get size of Array                                                 //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Size of the array.                                              //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning the size of the specified array.    //
// ----------------------------------------------------------------- //

public size_t Array_get_size(const Array *this)
{
	check_null(this);
	return this->size;
}



// ----------------------------------------------------------------- //
// Get pointer to array data                                         //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Pointer to the first element of the array.                      //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning a pointer to the first element of   //
//   the array. If the array has size 0, a NULL pointer is instead   //
//   returned.
// ----------------------------------------------------------------- //

public const double *Array_get_ptr(const Array *this)
{
	return this->values;
}



// ----------------------------------------------------------------- //
// Add new element of type double                                    //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) value    - Value to be added.                               //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for adding a new element of specified value at    //
//   the end of the array. The value must be of double-precision     //
//   floating-point type.
// ----------------------------------------------------------------- //

public void Array_push_flt(Array *this, const double value)
{
	// Sanity checks
	check_null(this);
	
	// Increase array size
	this->size += 1;
	this->values = (double *)realloc(this->values, this->size * sizeof(double));
	ensure(this->values != NULL, "Memory allocation error while adding array element.");
	
	// Insert new value at end
	this->values[this->size - 1] = value;
	
	return;
}



// ----------------------------------------------------------------- //
// Add new element of type int64_t                                   //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) value    - Value to be added.                               //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for adding a new element of specified value at    //
//   the end of the array. The value must be of 64-bit signed inte-  //
//   ger type.                                                       //
// ----------------------------------------------------------------- //

public void Array_push_int(Array *this, const int64_t value)
{
	// Sanity checks
	check_null(this);
	
	// Increase array size
	this->size += 1;
	this->values = (double *)realloc(this->values, this->size * sizeof(double));
	ensure(this->values != NULL, "Memory allocation error while adding array element.");
	
	// Insert new value at end
	*((int64_t *)(&this->values[this->size - 1])) = value;
	
	return;
}



// ----------------------------------------------------------------- //
// Get array element as floating-point value                         //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) index    - Index of the element to be returned.             //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Value of the requested element.                                 //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for retrieving the array value at the specified   //
//   index as a double-precision floating-point value. NOTE that     //
//   this function will not cast the value, and the data must have   //
//   been stored as a double-precision floating-point values to be-  //
//   gin with.                                                       //
// ----------------------------------------------------------------- //

public double Array_get_flt(const Array *this, const size_t index)
{
	check_null(this);
	ensure(index < this->size, "Array index out of range.");
	
	return this->values[index];
}



// ----------------------------------------------------------------- //
// Get array element as 64-bit signed integer value                  //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) index    - Index of the element to be returned.             //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Value of the requested element.                                 //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for retrieving the array value at the specified   //
//   index as a 64-bit signed integer value (int64_t). NOTE that     //
//   this function will not cast the value, and the data must have   //
//   been stored as a 64-bit signed integer value to begin with.     //
// ----------------------------------------------------------------- //

public int64_t Array_get_int(const Array *this, const size_t index)
{
	check_null(this);
	ensure(index < this->size, "Array index out of range.");
	
	return *((int64_t *)(&this->values[index]));
}
