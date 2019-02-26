/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (Source.c) - Source Finding Application             ///
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
#include <math.h>
#include <time.h>

#include "Source.h"



// ----------------------------------------------------------------- //
// Declaration of private properties and methods of class Source     //
// ----------------------------------------------------------------- //

class Source
{
	// Properties
	char        identifier[MAX_ARR_LENGTH];
	size_t      n_par;
	int64_t    *values;
	uint8_t    *types;
	char       *names;
	char       *units;
};

private void Source_append_memory(Source *this);



// ----------------------------------------------------------------- //
// Standard constructor                                              //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   No arguments.                                                   //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Pointer to newly created Source object.                         //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Standard constructor. Will create a new and empty Source object //
//   and return a pointer to the newly created object. No memory     //
//   will be allocated other than for the object itself. Note that   //
//   the destructor will need to be called explicitly once the ob-   //
//   ject is no longer required to release any memory allocated du-  //
//   ring the lifetime of the object.                                //
// ----------------------------------------------------------------- //

public Source *Source_new(void)
{
	// Allocate memory for new source
	Source *this = (Source *)malloc(sizeof(Source));
	ensure(this != NULL, "Failed to allocate memory for new source object.");
	
	// Initialise properties
	strncpy(this->identifier, "", MAX_ARR_LENGTH);  // Note that this should fill the entire string with NUL.
	this->n_par  = 0;
	this->values = NULL;
	this->types  = NULL;
	this->names  = NULL;
	this->units  = NULL;
	
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
//   mory occupied by the object. NOTE that if the source is added   //
//   to a catalogue object, the destructor MUST NOT be called manu-  //
//   ally, as the catalogue object will take ownership of the source //
//   and call its destructor at the end of its lifetime.             //
// ----------------------------------------------------------------- //

public void Source_delete(Source *this)
{
	if(this != NULL)
	{
		free(this->values);
		free(this->types);
		free(this->names);
		free(this->units);
		free(this);
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Set source identifier                                             //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) name     - Name to be used as identifier.                   //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for setting the identifier of a source. The maxi- //
//   mum length of the identifier is determined by the value of the  //
//   MAX_STR_LENGTH parameter defined in the header file. Note that  //
//   names are case-sensitive.                                       //
// ----------------------------------------------------------------- //

public void Source_set_identifier(Source *this, const char *name)
{
	// Sanity checks
	check_null(this);
	check_null(name);
	ensure(strlen(name) <= MAX_STR_LENGTH, "Source name must be no more than %d characters long.", MAX_STR_LENGTH);
	
	strncpy(this->identifier, name, MAX_STR_LENGTH);
	return;
}



// ----------------------------------------------------------------- //
// Append a new parameter of floating-point type                     //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) name     - Name of the new parameter.                       //
//   (3) value    - Value of the new parameter.                      //
//   (3) unit     - Unit of the new parameter.                       //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for appending a new parameter of floating-point   //
//   type to an existing source. The value will be stored as a       //
//   double-precision floating-point number. Note that the length of //
//   both the name and unit strings are constrained by the value of  //
//   MAX_STR_LENGTH as defined in the header file. Also note that    //
//   the function does not check if the specified parameter name al- //
//   ready exists in the current parameter list; if it did, a new    //
//   parameter with the same name would be appended at the end. Note //
//   that names are case-sensitive.                                  //
// ----------------------------------------------------------------- //

public void Source_add_par_flt(Source *this, const char *name, const double value, const char *unit)
{
	// Sanity checks
	check_null(this);
	check_null(name);
	check_null(unit);
	ensure(strlen(name) <= MAX_STR_LENGTH, "Parameter name must be no more than %d characters long.", MAX_STR_LENGTH);
	ensure(strlen(unit) <= MAX_STR_LENGTH, "Parameter unit must be no more than %d characters long.", MAX_STR_LENGTH);
	
	// Reserve memory for one additional parameter
	Source_append_memory(this);
	
	// Copy new parameter information
	*((double *)(this->values + this->n_par - 1)) = value;
	*(this->types + this->n_par - 1) = SOURCE_TYPE_FLT;
	strncpy(this->names + (this->n_par - 1) * MAX_ARR_LENGTH, name, MAX_STR_LENGTH);
	strncpy(this->units + (this->n_par - 1) * MAX_ARR_LENGTH, unit, MAX_STR_LENGTH);
	
	return;
}



// ----------------------------------------------------------------- //
// Append a new parameter of integer type                            //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) name     - Name of the new parameter.                       //
//   (3) value    - Value of the new parameter.                      //
//   (3) unit     - Unit of the new parameter.                       //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for appending a new parameter of integer type to  //
//   an existing source. The value will be stored as a 64-bit inte-  //
//   ger number. Note that the length of both the name and unit      //
//   strings are constrained by the value of MAX_STR_LENGTH as de-   //
//   fined in the header file. Also note that the function does not  //
//   check if the specified parameter name already exists in the     //
//   current parameter list; if it did, a new parameter with the     //
//   same name would be appended at the end. Note that names are     //
//   case-sensitive.                                                 //
// ----------------------------------------------------------------- //

public void Source_add_par_int(Source *this, const char *name, const int64_t value, const char *unit)
{
	// Sanity checks
	check_null(this);
	check_null(name);
	check_null(unit);
	ensure(strlen(name) <= MAX_STR_LENGTH, "Parameter name must be no more than %d characters long.", MAX_STR_LENGTH);
	ensure(strlen(unit) <= MAX_STR_LENGTH, "Parameter unit must be no more than %d characters long.", MAX_STR_LENGTH);
	
	// Reserve memory for one additional parameter
	Source_append_memory(this);
	
	// Copy new parameter information
	*(this->values + this->n_par - 1) = value;
	*(this->types  + this->n_par - 1) = SOURCE_TYPE_INT;
	strncpy(this->names + (this->n_par - 1) * MAX_ARR_LENGTH, name, MAX_STR_LENGTH);
	strncpy(this->units + (this->n_par - 1) * MAX_ARR_LENGTH, unit, MAX_STR_LENGTH);
	
	return;
}



// ----------------------------------------------------------------- //
// Extract a parameter as floating-point value                       //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) index    - Index of the parameter to be extracted.          //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Parameter value as type double.                                 //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for extracting the value of the specified parame- //
//   ter from the specified source as a double-precision floating-   //
//   point number.                                                   //
// ----------------------------------------------------------------- //

public double Source_get_par_flt(const Source *this, const size_t index)
{
	check_null(this);
	ensure(index < this->n_par, "Source parameter index out of range.");
	return *((double *)(this->values + index));
}



// ----------------------------------------------------------------- //
// Extract a parameter as integer value                              //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) index    - Index of the parameter to be extracted.          //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Parameter value as type int64_t.                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for extracting the value of the specified parame- //
//   ter from the specified source as a 64-bit integer number.       //
// ----------------------------------------------------------------- //

public int64_t Source_get_par_int(const Source *this, const size_t index)
{
	check_null(this);
	ensure(index < this->n_par, "Source parameter index out of range.");
	return *(this->values + index);
}



// ----------------------------------------------------------------- //
// Set source parameter as floating-point value                      //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) name     - Name of the parameter to be set.                 //
//   (3) value    - Value of the parameter to be set.                //
//   (4) unit     - Unit of the parameter to be set.                 //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for setting the specified parameter of the speci- //
//   fied source as a double-precision floating-point number. If a   //
//   parameter of the same name already exists, its value will be    //
//   replaced; otherwise, a new parameter will be created and added  //
//   to the existing parameter list. Note that the length of both    //
//   the name and unit strings are constrained by the value of       //
//   MAX_STR_LENGTH as defined in the header file. Note that names   //
//   are case-sensitive.                                             //
// ----------------------------------------------------------------- //

public void Source_set_par_flt(Source *this, const char *name, const double value, const char *unit)
{
	// Sanity checks
	check_null(this);
	check_null(name);
	check_null(unit);
	ensure(strlen(name) <= MAX_STR_LENGTH, "Parameter name must be no more than %d characters long.", MAX_STR_LENGTH);
	ensure(strlen(unit) <= MAX_STR_LENGTH, "Parameter unit must be no more than %d characters long.", MAX_STR_LENGTH);
	
	// Check if parameter of same name already exists
	size_t index;
	
	if(Source_par_exists(this, name, &index))
	{
		// If so, overwrite with new parameter information
		*((double *)(this->values + index)) = value;
		*(this->types + index) = SOURCE_TYPE_FLT;
		strncpy(this->names + index * MAX_ARR_LENGTH, name, MAX_STR_LENGTH);
		strncpy(this->units + index * MAX_ARR_LENGTH, unit, MAX_STR_LENGTH);
	}
	else
	{
		// Otherwise add as new parameter
		Source_add_par_flt(this, name, value, unit);
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Set source parameter as integer value                             //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) name     - Name of the parameter to be set.                 //
//   (3) value    - Value of the parameter to be set.                //
//   (4) unit     - Unit of the parameter to be set.                 //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for setting the specified parameter of the speci- //
//   fied source as a 64-bit integer number. If a parameter of the   //
//   same name already exists, its value will be replaced; other-    //
//   wise, a new parameter will be created and added to the existing //
//   parameter list. Note that the length of both the name and unit  //
//   strings are constrained by the value of MAX_STR_LENGTH as de-   //
//   fined in the header file. Note that names are case-sensitive.   //
// ----------------------------------------------------------------- //

public void Source_set_par_int(Source *this, const char *name, const int64_t value, const char *unit)
{
	// Sanity checks
	check_null(this);
	check_null(name);
	check_null(unit);
	ensure(strlen(name) <= MAX_STR_LENGTH, "Parameter name must be no more than %d characters long.", MAX_STR_LENGTH);
	ensure(strlen(unit) <= MAX_STR_LENGTH, "Parameter unit must be no more than %d characters long.", MAX_STR_LENGTH);
	
	// Check if parameter already exists
	size_t index;
	
	if(Source_par_exists(this, name, &index))
	{
		// If so, overwrite with new parameter information
		*(this->values + index) = value;
		*(this->types  + index) = SOURCE_TYPE_INT;
		strncpy(this->names + index * MAX_ARR_LENGTH, name, MAX_STR_LENGTH);
		strncpy(this->units + index * MAX_ARR_LENGTH, unit, MAX_STR_LENGTH);
	}
	else
	{
		// Otherwise add as new parameter
		Source_add_par_int(this, name, value, unit);
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Check if source parameter exists                                  //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) name     - Name of the parameter to be checked.             //
//   (3) index    - Pointer to a variable that will hold the index   //
//                  of the source parameter if found.                //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns true if the parameter exists, false otherwise.          //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for checking if a parameter of the specified name //
//   already exists in the specified source. The function will re-   //
//   turn true if the parameter exists and false otherwise. Note     //
//   that name is case-sensitive. The variable 'index' will be set   //
//   to the index of the parameter if found. Otherwise it will be    //
//   left untouched. If no index is required, a NULL pointer can in- //
//   stead be provided.                                              //
// ----------------------------------------------------------------- //

public bool Source_par_exists(const Source *this, const char *name, size_t *index)
{
	// Sanity checks
	check_null(this);
	check_null(name);
	ensure(strlen(name) <= MAX_STR_LENGTH, "Parameter name must be no more than %d characters long.", MAX_STR_LENGTH);
	
	for(size_t i = 0; i < this->n_par; ++i)
	{
		if(memcmp(this->names + i * MAX_ARR_LENGTH, name, strlen(name)) == 0)
		{
			if(index != NULL) *index = i;
			return true;
		}
	}
	
	return false;
}



// ----------------------------------------------------------------- //
// Extract name of parameter by index                                //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) index    - Index of the parameter the unit of which is to   //
//                  be returned.                                     //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Pointer to the name string of the specified parameter.          //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning a pointer to the name string of the //
//   specified parameter.                                            //
// ----------------------------------------------------------------- //

public char *Source_get_name(const Source *this, const size_t index)
{
	check_null(this);
	ensure(index < this->n_par, "Source parameter index out of range.");
	return this->names + index * MAX_ARR_LENGTH;
}



// ----------------------------------------------------------------- //
// Extract unit of parameter by index                                //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) index    - Index of the parameter the unit of which is to   //
//                  be returned.                                     //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Pointer to the unit string of the specified parameter.          //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning a pointer to the unit string of the //
//   specified parameter.                                            //
// ----------------------------------------------------------------- //

public char *Source_get_unit(const Source *this, const size_t index)
{
	check_null(this);
	ensure(index < this->n_par, "Source parameter index out of range.");
	return this->units + index * MAX_ARR_LENGTH;
}



// ----------------------------------------------------------------- //
// Extract type of parameter by index                                //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) index    - Index of the parameter the type of which is to   //
//                  be returned.                                     //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Data type of the specified parameter.                           //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning the data type of the specified      //
//   parameter, where 0 means integer and 1 means floating point.    //
// ----------------------------------------------------------------- //

public uint8_t Source_get_type(const Source *this, const size_t index)
{
	check_null(this);
	ensure(index < this->n_par, "Source parameter index out of range.");
	return this->types[index];
}



// ----------------------------------------------------------------- //
// Get identifier of specified source                                //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Identifier of the specified source.                             //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning the identifier string of the speci- //
//   fied source.                                                    //
// ----------------------------------------------------------------- //

public const char *Source_get_identifier(const Source *this)
{
	check_null(this);
	return this->identifier;
}



// ----------------------------------------------------------------- //
// Get number of parameters for specified source                     //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Number of parameters currently defined.                         //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning the number of parameters currently  //
//   defined for the specified source.                               //
// ----------------------------------------------------------------- //

public size_t Source_get_num_par(const Source *this)
{
	check_null(this);
	return this->n_par;
}



// ----------------------------------------------------------------- //
// Reallocate memory for one additional parameter                    //
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
//   Private method for allocating additional memory for one more    //
//   parameter in the specified source. Note that this will not cre- //
//   ate a new parameter yet, but just allocate the memory needed to //
//   append a parameter at the end of the parameter list. The func-  //
//   tion should be called from public member functions that will    //
//   add parameters to a source prior to assigning the new parameter //
//   values.                                                         //
// ----------------------------------------------------------------- //

private void Source_append_memory(Source *this)
{
	this->n_par += 1;
	this->values = (int64_t *)realloc(this->values, this->n_par * sizeof(int64_t));
	this->types  = (uint8_t *)realloc(this->types,  this->n_par * sizeof(uint8_t));
	this->names  = (char *)   realloc(this->names,  this->n_par * MAX_ARR_LENGTH * sizeof(char));
	this->units  = (char *)   realloc(this->units,  this->n_par * MAX_ARR_LENGTH * sizeof(char));
	
	ensure(this->values != NULL && this->types != NULL && this->names != NULL && this->units != NULL, "Memory allocation for new source parameter failed.");
	
	return;
}
