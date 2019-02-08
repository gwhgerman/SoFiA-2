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
// Standard constructor                                              //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   size - Size of the array to be created.                         //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Pointer to newly created Array object.                          //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Standard constructor. Will create a new Array object of given   //
//   size and type double and return a pointer to the newly created  //
//   object. Sufficient memory will be allocated to store the array  //
//   values of type double. Note that the destructor will need to be //
//   called explicitly once the object is no longer required to re-  //
//   lease any memory allocated during the lifetime of the object.   //
// ----------------------------------------------------------------- //

public Array *Array_new(const size_t size, const int type)
{
	Array *this = (Array *)malloc(sizeof(Array));
	ensure(this != NULL, "Failed to allocate memory for new Array object.");
	ensure(type == ARRAY_TYPE_FLT || type == ARRAY_TYPE_INT, "Array data type must be ARRAY_TYPE_FLT or ARRAY_TYPE_INT.");
	
	this->size = size;
	this->type = type;
	
	if(size)
	{
		this->values = (double *)malloc(size * sizeof(double));
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
	ensure(string != NULL && strlen(string), "Invalid string encountered in Array object constructor.");
	
	// Count number of commas
	size_t size = 1;
	size_t i = strlen(string);
	while(i--) if(string[i] == ',') ++size;
	
	// Create array of given size
	Array *this = Array_new(size, type);
	
	// Fill array with values
	char *token = strtok(string, ",");
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
	if(this != NULL)
	{
		free(this->values);
		free(this);
	}
	
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
	ensure(this != NULL, "Invalid pointer to Array object encountered.");
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
	ensure(this != NULL, "Invalid pointer to Array object encountered.");
	ensure(index < this->size, "Array index out of range.");
	
	return *((int64_t *)(&this->values[index]));
}
