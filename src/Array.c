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

CLASS Array
{
	int    type;
	size_t size;
	void   *values;
};



// ----------------------------------------------------------------- //
// Standard constructor                                              //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   size - Size of the array to be created.                         //
//   type - Data type to be used. Can be ARRAY_TYPE_FLT for double   //
//          or ARRAY_TYPE_INT for long int.                          //
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
//   NOTE that the array values will be initialised to 0.            //
// ----------------------------------------------------------------- //

PUBLIC Array *Array_new(const size_t size, const int type)
{
	// Sanity checks
	ensure(type == ARRAY_TYPE_FLT || type == ARRAY_TYPE_INT, "Array data type must be ARRAY_TYPE_FLT or ARRAY_TYPE_INT.");
	
	Array *self = (Array *)malloc(sizeof(Array));
	ensure(self != NULL, "Failed to allocate memory for new Array object.");
	
	self->size = size;
	self->type = type;
	
	if(self->size)
	{
		if(self->type == ARRAY_TYPE_FLT) self->values = (double *)calloc(size, sizeof(double));
		else self->values = (long int *)calloc(size, sizeof(long int));
		ensure(self->values != NULL, "Failed to allocate memory for new Array object.");
	}
	else
	{
		self->values = NULL;
	}
	
	return self;
}



// ----------------------------------------------------------------- //
// Alternative constructor                                           //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   string - String containing the values to be stored in the       //
//            array, separated by commas.                            //
//   type   - Data type; can be ARRAY_TYPE_FLT for double-precision  //
//            floating-point data or ARRAY_TYPE_INT for long inte-   //
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
//   will be allocated to store the array values as either double-   //
//   precision floating-point data or long integer data. Note that   //
//   the destructor will need to be called explicitly once the ob-   //
//   ject is no longer required to release any memory allocated dur- //
//   ing the lifetime of the object.                                 //
// ----------------------------------------------------------------- //

PUBLIC Array *Array_new_str(char *string, const int type)
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
	Array *self = Array_new(size, type);
	
	// Fill array with values
	char *token = strtok(copy, ",");
	ensure(token != NULL, "Failed to parse string as array.");
	
	if(self->type == ARRAY_TYPE_FLT) *((double *)(self->values)) = strtod(token, NULL);
	else *((long int *)(self->values)) = strtol(token, NULL, 10);
	
	for(i = 1; i < size; ++i)
	{
		token = strtok(NULL, ",");
		ensure(token != NULL, "Failed to parse string as array.");
		
		if(self->type == ARRAY_TYPE_FLT) *((double *)(self->values) + i) = strtod(token, NULL);
		else *((long int *)(self->values) + i) = strtol(token, NULL, 10);
	}
	
	// Delete string copy again
	free(copy);
	
	return self;
}



// ----------------------------------------------------------------- //
// Destructor                                                        //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) self     - Object self-reference.                           //
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

PUBLIC void Array_delete(Array *self)
{
	if(self != NULL) free(self->values);
	free(self);
	
	return;
}



// ----------------------------------------------------------------- //
// Get size of Array                                                 //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) self     - Object self-reference.                           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Size of the array.                                              //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning the size of the specified array.    //
// ----------------------------------------------------------------- //

PUBLIC size_t Array_get_size(const Array *self)
{
	check_null(self);
	return self->size;
}



// ----------------------------------------------------------------- //
// Get type of Array                                                 //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) self     - Object self-reference.                           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Type of the array.                                              //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning the type of the specified array.    //
//   The type can be either ARRAY_TYPE_FLT or ARRAY_TYPE_INT.        //
// ----------------------------------------------------------------- //

PUBLIC int Array_get_type(const Array *self)
{
	check_null(self);
	return self->type;
}



// ----------------------------------------------------------------- //
// Get pointer to data array                                         //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) self     - Object self-reference.                           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Pointer to the first element of the array. If the array has     //
//   size 0, a NULL pointer will be returned instead,                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning a pointer to the first element of   //
//   the array. NOTE that the returned pointer is of type void, and  //
//   explicit casting to either (double *) or (long int *) will be   //
//   required before accessing the array values.                     //
// ----------------------------------------------------------------- //

PUBLIC const void *Array_get_ptr(const Array *self)
{
	// Sanity checks
	check_null(self);
	if(self->size == 0) return NULL;
	return self->values;
}



// ----------------------------------------------------------------- //
// Add new element of type double                                    //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) self     - Object self-reference.                           //
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
//   floating-point type.                                            //
// ----------------------------------------------------------------- //

PUBLIC void Array_push_flt(Array *self, const double value)
{
	// Sanity checks
	check_null(self);
	ensure(self->type == ARRAY_TYPE_FLT, "Array is not of floating-point type.");
	
	// Increase array size
	self->size += 1;
	self->values = (double *)realloc(self->values, self->size * sizeof(double));
	ensure(self->values != NULL, "Memory allocation error while adding array element.");
	
	// Insert new value at end
	*((double *)(self->values) + self->size - 1) = value;
	
	return;
}



// ----------------------------------------------------------------- //
// Add new element of type long int                                  //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) self     - Object self-reference.                           //
//   (2) value    - Value to be added.                               //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for adding a new element of specified value at    //
//   the end of the array. The value must be of long int type.       //
// ----------------------------------------------------------------- //

PUBLIC void Array_push_int(Array *self, const long int value)
{
	// Sanity checks
	check_null(self);
	ensure(self->type == ARRAY_TYPE_INT, "Array is not of integer type.");
	
	// Increase array size
	self->size += 1;
	self->values = (long int *)realloc(self->values, self->size * sizeof(long int));
	
	// Insert new value at end
	*((long int *)(self->values) + self->size - 1) = value;
	
	return;
}



// ----------------------------------------------------------------- //
// Get array element as floating-point value                         //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) self     - Object self-reference.                           //
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

PUBLIC double Array_get_flt(const Array *self, const size_t index)
{
	check_null(self);
	ensure(index < self->size, "Array index out of range.");
	ensure(self->type == ARRAY_TYPE_FLT, "Array is not of floating-point type.");
	
	return *((double *)(self->values) + index);
}



// ----------------------------------------------------------------- //
// Get array element as long int value                               //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) self     - Object self-reference.                           //
//   (2) index    - Index of the element to be returned.             //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Value of the requested element.                                 //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for retrieving the array value at the specified   //
//   index as a long int value. NOTE that this function will not     //
//   cast the value, and the data must have been stored as a long    //
//   int value to begin with.                                        //
// ----------------------------------------------------------------- //

PUBLIC long int Array_get_int(const Array *self, const size_t index)
{
	check_null(self);
	ensure(index < self->size, "Array index out of range.");
	ensure(self->type == ARRAY_TYPE_INT, "Array is not of integer type.");
	
	return *((long int *)(self->values) + index);
}



// ----------------------------------------------------------------- //
// Get array element as unsigned long int value                      //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) self     - Object self-reference.                           //
//   (2) index    - Index of the element to be returned.             //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Value of the requested element.                                 //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for retrieving the array value at the specified   //
//   index as an unsigned long int value. NOTE that this function    //
//   will not cast the value from floating-point types, and the data //
//   must have been stored as a long int value to begin with.        //
// ----------------------------------------------------------------- //

PUBLIC unsigned long int Array_get_uint(const Array *self, const size_t index)
{
	check_null(self);
	ensure(index < self->size, "Array index out of range.");
	ensure(self->type == ARRAY_TYPE_INT, "Array is not of integer type.");
	
	return (unsigned long int)(*((long int *)(self->values) + index));
}



// ----------------------------------------------------------------- //
// Set array element as floating-point value                         //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) self     - Object self-reference.                           //
//   (2) index    - Index of the element to be set.                  //
//   (3) value    - Value of the element to be set.                  //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for setting the value of the array element at the //
//   specified index to the specified double-precision floating-     //
//   point value.                                                    //
// ----------------------------------------------------------------- //

PUBLIC void Array_set_flt(Array *self, const size_t index, const double value)
{
	check_null(self);
	ensure(index < self->size, "Array index out of range.");
	ensure(self->type == ARRAY_TYPE_FLT, "Array is not of floating-point type.");
	
	*((double *)(self->values) + index) = value;
	return;
}



// ----------------------------------------------------------------- //
// Set array element as long int value                               //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) self     - Object self-reference.                           //
//   (2) index    - Index of the element to be set.                  //
//   (3) value    - Value of the element to be set.                  //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for setting the value of the array element at the //
//   specified index to the specified long int value.                //
// ----------------------------------------------------------------- //

PUBLIC void Array_set_int(Array *self, const size_t index, const long int value)
{
	check_null(self);
	ensure(index < self->size, "Array index out of range.");
	ensure(self->type == ARRAY_TYPE_INT, "Array is not of integer type.");
	
	*((long int *)(self->values) + index) = value;
	return;
}



// ----------------------------------------------------------------- //
// Add floating-point value to array element                         //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) self     - Object self-reference.                           //
//   (2) index    - Index of the element to be set.                  //
//   (3) value    - Value to be added to the element.                //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for adding the specified double-precision float-  //
//   ing-point value to the array element at the specified index.    //
// ----------------------------------------------------------------- //

PUBLIC void Array_add_flt(Array *self, const size_t index, const double value)
{
	check_null(self);
	ensure(index < self->size, "Array index out of range.");
	ensure(self->type == ARRAY_TYPE_FLT, "Array is not of floating-point type.");
	
	*((double *)(self->values) + index) += value;
	return;
}



// ----------------------------------------------------------------- //
// Add long int value to array element                               //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) self     - Object self-reference.                           //
//   (2) index    - Index of the element to be set.                  //
//   (3) value    - Value to be added to the element.                //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for adding the specified long int value to the    //
//   array element at the specified index.                           //
// ----------------------------------------------------------------- //

PUBLIC void Array_add_int(Array *self, const size_t index, const long int value)
{
	check_null(self);
	ensure(index < self->size, "Array index out of range.");
	ensure(self->type == ARRAY_TYPE_INT, "Array is not of integer type.");
	
	*((long int *)(self->values) + index) += value;
	return;
}
