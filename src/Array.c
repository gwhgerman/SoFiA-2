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

public Array *Array_new(const size_t size)
{
	Array *this = (Array *)malloc(sizeof(Array));
	ensure(this != NULL, "Failed to allocate memory for new Array object.");
	
	this->size = size;
	
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
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Pointer to newly created Array object.                          //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Alternative constructor. Will create a new Array object of type //
//   double, the size of which is determined by the number of comma- //
//   separated floating-point values specified in 'string'. A point- //
//   er to the newly created object will be returned. Sufficient me- //
//   mory will be allocated to store the array values as type dou-   //
//   ble. Note that the destructor will need to be called explicitly //
//   once the object is no longer required to release any memory al- //
//   located during the lifetime of the object.                      //
// ----------------------------------------------------------------- //

public Array *Array_new_str(char *string)
{
	ensure(strlen(string), "Empty string encountered in Array object constructor.");
	
	// Count number of commas
	size_t size = 1;
	size_t i = strlen(string);
	while(i--) if(string[i] == ',') ++size;
	
	// Create array of given size
	Array *this = Array_new(size);
	this->size = size;
	
	// Fill array with values
	char *token = strtok(string, ",");
	ensure(token != NULL, "Failed to parse string as array.");
	this->values[0] = strtod(token, NULL);
	
	for(i = 1; i < size; ++i)
	{
		token = strtok(NULL, ",");
		ensure(token != NULL, "Failed to parse string as array.");
		this->values[i] = strtod(token, NULL);
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
