/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (SourceCatalog.c) - Source Finding Application      ///
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

#include "SourceCatalog.h"



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
// Declaration of private properties and methods of class Catalog    //
// ----------------------------------------------------------------- //

class Catalog
{
	size_t size;
	Source **sources;
};

private void Catalog_append_memory(Catalog *this);



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
	
	strncpy(this->identifier, "Undefined", MAX_ARR_LENGTH);  // Note that this should fill the remainder of the string with NUL.
	this->n_par  = 0;
	this->values = NULL;
	this->types  = NULL;
	this->names  = NULL;
	this->units  = NULL;
	
	// Initialise properties
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
//   to a catalogue object, the destructor must not be called manu-  //
//   ally, as the catalogue object will take ownership of the source //
//   and call its destructor at the end of its lifetime.             //
// ----------------------------------------------------------------- //

public void Source_delete(Source *this)
{
	if(this != NULL)
	{
		// De-allocate memory for parameters first
		// (no need to check for NULL, as free(NULL) does nothing by definition)
		free(this->values);
		free(this->types);
		free(this->names);
		free(this->units);
		
		// Lastly, de-allocate memory for source object
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
//   MAX_STR_LENGTH parameter defined in the header file. Any names  //
//   longer than this will be truncated without warning. Note that   //
//   name is case-sensitive.                                         //
// ----------------------------------------------------------------- //

public void Source_set_identifier(Source *this, const char *name)
{
	ensure(this != NULL, "Invalid Source object provided.");
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
//   MAX_STR_LENGTH as defined in the header file; they will be      //
//   truncated without warning if necessary. Also note that the      //
//   function does not check if the specified parameter name already //
//   exists in the current parameter list; if it did, a new parame-  //
//   ter with the same name would be appended at the end. Note that  //
//   name is case-sensitive.                                         //
// ----------------------------------------------------------------- //

public void Source_add_par_flt(Source *this, const char *name, const double value, const char *unit)
{
	ensure(this != NULL, "Invalid Source object provided.");
	Source_append_memory(this);
	
	// Copy new parameter information
	*((double *)(this->values + this->n_par - 1)) = value;
	*(this->types + this->n_par - 1) = 1;
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
//   fined in the header file; they will be truncated without warn-  //
//   ing if necessary. Also note that the function does not check if //
//   the specified parameter name already exists in the current      //
//   parameter list; if it did, a new parameter with the same name   //
//   would be appended at the end. Note that name is case-sensitive. //
// ----------------------------------------------------------------- //

public void Source_add_par_int(Source *this, const char *name, const int64_t value, const char *unit)
{
	ensure(this != NULL, "Invalid Source object provided.");
	Source_append_memory(this);
	
	// Copy new parameter information
	*(this->values + this->n_par - 1) = value;
	*(this->types  + this->n_par - 1) = 0;
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
//   (2) name     - Name of the parameter to be extracted.           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   If the parameter name exists, its value will be returned.       //
//   Otherwise, NaN will be returned.                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for extracting the value of the specified parame- //
//   ter from the specified source as a double-precision floating-   //
//   point number. If the parameter name does not exist, a value of  //
//   NaN will be returned instead. Note that name is case-sensitive. //
// ----------------------------------------------------------------- //

public double Source_get_par_flt(const Source *this, const char *name)
{
	ensure(this != NULL, "Invalid Source object provided.");
	
	const char *ptr = this->names  + this->n_par * MAX_ARR_LENGTH;
	const int64_t *ptr_val = this->values + this->n_par;
	const size_t length  = strlen(name);
	
	while(ptr > this->names)
	{
		ptr -= MAX_ARR_LENGTH;
		--ptr_val;
		if(memcmp(ptr, name, length) == 0) return *((double *)ptr_val);
	}
	
	return NAN;
}



// ----------------------------------------------------------------- //
// Extract a parameter as integer number                             //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) name     - Name of the parameter to be extracted.           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   If the parameter name exists, its value will be returned.       //
//   Otherwise, NaN will be returned.                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for extracting the value of the specified parame- //
//   ter from the specified source as a 64-bit integer number. If    //
//   the parameter name does not exist, a value of 0 will be re-     //
//   turned instead. Note that name is case-sensitive.               //
// ----------------------------------------------------------------- //

public int64_t Source_get_par_int(const Source *this, const char *name)
{
	ensure(this != NULL, "Invalid Source object provided.");
	
	const char *ptr = this->names  + this->n_par * MAX_ARR_LENGTH;
	const int64_t *ptr_val = this->values + this->n_par;
	const size_t length  = strlen(name);
	
	while(ptr > this->names)
	{
		ptr -= MAX_ARR_LENGTH;
		--ptr_val;
		if(memcmp(ptr, name, length) == 0) return *ptr_val;
	}
	
	return 0;
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
//   MAX_STR_LENGTH as defined in the header file; they will be      //
//   truncated without warning if necessary. Note that name is case- //
//   sensitive.                                                      //
// ----------------------------------------------------------------- //

public void Source_set_par_flt(Source *this, const char *name, const double value, const char *unit)
{
	ensure(this != NULL, "Invalid Source object provided.");
	
	// Check if parameter already exists
	const size_t index = Source_par_exists(this, name);
	
	if(index)
	{
		// If so, overwrite with new parameter information
		*((double *)(this->values + index - 1)) = value;
		*(this->types + index - 1) = 1;
		strncpy(this->names + (index - 1) * MAX_ARR_LENGTH, name, MAX_STR_LENGTH);
		strncpy(this->units + (index - 1) * MAX_ARR_LENGTH, unit, MAX_STR_LENGTH);
	}
	else
	{
		// Else add as new parameter
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
//   fined in the header file; they will be truncated without warn-  //
//   ing if necessary. Note that name is case-sensitive.             //
// ----------------------------------------------------------------- //

public void Source_set_par_int(Source *this, const char *name, const int64_t value, const char *unit)
{
	ensure(this != NULL, "Invalid Source object provided.");
	
	// Check if parameter already exists
	const size_t index = Source_par_exists(this, name);
	
	if(index)
	{
		// If so, overwrite with new parameter information
		*(this->values + index - 1) = value;
		*(this->types  + index - 1) = 0;
		strncpy(this->names + (index - 1) * MAX_ARR_LENGTH, name, MAX_STR_LENGTH);
		strncpy(this->units + (index - 1) * MAX_ARR_LENGTH, unit, MAX_STR_LENGTH);
	}
	else
	{
		// Else add as new parameter
		Source_add_par_int(this, name, value, unit);
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Check if parameter name already exists                            //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) name     - Name of the parameter to be checked.             //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns 0 if the parameter name does not yet exist. Otherwise   //
//   returns the index number of the parameter in the current list.  //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for checking if a parameter of the specified name //
//   already exists in the specified source. The function will re-   //
//   turn the index number (or column) of existing parameters or a   //
//   value of 0 if the parameter name was not found. Note that index //
//   numbers are one-based, i.e. the first column will be number 1,  //
//   not 0. Note that name is case-sensitive.                        //
// ----------------------------------------------------------------- //

public size_t Source_par_exists(const Source *this, const char *name)
{
	ensure(this != NULL, "Invalid Source object provided.");
	
	size_t i = this->n_par;
	const char *ptr = this->names + i * MAX_ARR_LENGTH;
	const size_t length = strlen(name);
	
	while(ptr > this->names)
	{
		ptr -= MAX_ARR_LENGTH;
		if(memcmp(ptr, name, length) == 0) return i;
		--i;
	}
	
	return 0;
}



// ----------------------------------------------------------------- //
// Extract unit for the specified parameter                          //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) name     - Name of the parameter the unit of which is to be //
//                  returned.                                        //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns a pointer to the unit string of the specified parame-   //
//   ter. If the parameter does not exist, a NULL pointer will be    //
//   returned.                                                       //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning a pointer to the unit string of the //
//   specified parameter. If a parameter of the specified name does  //
//   not exist, then a NULL pointer is returned. Note that name is   //
//   case-sensitive.                                                 //
// ----------------------------------------------------------------- //

public char *Source_get_unit(const Source *this, const size_t index)
{
	ensure(this != NULL && this->n_par, "Invalid or empty Source object provided.");
	ensure(index < this->n_par, "Source parameter index out of range.");
	
	return this->units + index * MAX_ARR_LENGTH;
}



public char *Source_get_name(const Source *this, const size_t index)
{
	ensure(this != NULL && this->n_par, "Invalid or empty Source object provided.");
	ensure(index < this->n_par, "Source parameter index out of range.");
	
	return this->names + index * MAX_ARR_LENGTH;
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
	// Extend memory for parameter arrays
	if(this->n_par)
	{
		this->n_par += 1;
		this->values = (int64_t *)realloc(this->values, this->n_par * sizeof(int64_t));
		this->types  = (uint8_t *)realloc(this->types,  this->n_par * sizeof(uint8_t));
		this->names  = (char *)   realloc(this->names,  this->n_par * MAX_ARR_LENGTH * sizeof(char));
		this->units  = (char *)   realloc(this->units,  this->n_par * MAX_ARR_LENGTH * sizeof(char));
	}
	else
	{
		this->n_par  = 1;
		this->values = (int64_t *)malloc(sizeof(int64_t));
		this->types  = (uint8_t *)malloc(sizeof(uint8_t));
		this->names  = (char *)   malloc(MAX_ARR_LENGTH * sizeof(char));
		this->units  = (char *)   malloc(MAX_ARR_LENGTH * sizeof(char));
	}
	
	ensure(this->values != NULL && this->types != NULL && this->names != NULL && this->units != NULL, "Memory allocation for new source parameter failed.");
	
	return;
}



// ----------------------------------------------------------------- //
// Standard constructor                                              //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   No arguments.                                                   //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Pointer to newly created Catalog object.                        //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Standard constructor. Will create a new and empty Catalog ob-   //
//   ject and return a pointer to the newly created object. No memo- //
//   ry will be allocated other than for the object itself. Note     //
//   that the destructor will need to be called explicitly once the  //
//   object is no longer required to release any memory allocated    //
//   during the lifetime of the object.                              //
// ----------------------------------------------------------------- //

public Catalog *Catalog_new(void)
{
	// Allocate memory for new catalog
	Catalog *this = (Catalog*)malloc(sizeof(Catalog));
	ensure(this != NULL, "Failed to allocate memory for new catalogue object.");
	
	// Initialise properties
	this->size = 0;
	this->sources = NULL;
	
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
//   NOTE that the destructor will explicitly call the destructor on //
//   all source objects stored in the catalogue. Hence, deleting a   //
//   catalogue will automatically delete all sources associated with //
//   that catalogue!                                                 //
// ----------------------------------------------------------------- //

public void Catalog_delete(Catalog *this)
{
	if(this != NULL)
	{
		if(this->sources != NULL)
		{
			// Call the destructor on individual sources first
			Source **ptr = this->sources + this->size;
			while(ptr --> this->sources) Source_delete(*ptr);
			
			// Then de-allocate memory for pointers to those sources
			free(this->sources);
		}
		
		// Lastly, de-allocate memory for catalog object
		free(this);
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Add a new source to a catalogue                                   //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) src      - Pointer to the source to be added                //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for adding a new source to the specified cata-    //
//   logue. Note that the function does not check if a source with   //
//   the same name already exists; a new source will always be added //
//   to the existing source list.                                    //
// ----------------------------------------------------------------- //

public void Catalog_add_source(Catalog *this, Source *src)
{
	ensure(this != NULL, "Invalid catalogue object provided.");
	ensure(src  != NULL, "Invalid source object provided.");
	
	// Check if the same physical source object is already present
	if(Catalog_source_exists(this, src))
	{
		warning("Insertion of new source aborted.\n         Source \'%s\' is already in catalogue.", src->identifier);
		return;
	}
	
	Catalog_append_memory(this);
	
	*(this->sources + this->size - 1) = src;
	
	return;
}



// ----------------------------------------------------------------- //
// Check if source is in catalogue                                   //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) src      - Pointer to the source to be checked              //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns the index (i.e. row number) of the source within the    //
//   catalogue if the source was found; otherwise returns 0.         //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for checking if the specified source is included  //
//   in the catalogue. If so, the function will return the row num-  //
//   ber of the source in the catalogue (starting with 1). If the    //
//   source is not found, the function will return 0.                //
// ----------------------------------------------------------------- //

public size_t Catalog_source_exists(const Catalog *this, const Source *src)
{
	if(this->size)
	{
		Source **ptr = this->sources + this->size;
		size_t i = this->size;
		
		while(ptr --> this->sources)
		{
			if(*ptr == src) return i;
			--i;
		}
	}
	
	return 0;
}



// ----------------------------------------------------------------- //
// Retrieve a source from the catalogue by identifier                //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this       - Object self-reference.                         //
//   (2) identifier - Identifier of the source to be extracted.      //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns a pointer to the requested source if the source is pre- //
//   sent in the catalogue. If the source is not found, a NULL       //
//   pointer will be returned instead.                               //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for extracting a specific source from the cata-   //
//   logue by its identifier. If the source is found, a pointer to   //
//   the source will be returned. Otherwise, the function will re-   //
//   turn a NULL pointer. Note that if multiple sources with the     //
//   same identifier exist, a pointer to the last source (i.e. the   //
//   one with the highest index number) will be returned.            //
// ----------------------------------------------------------------- //

public Source *Catalog_get_source(const Catalog *this, const char *identifier)
{
	if(this->size)
	{
		Source **ptr = this->sources + this->size;
		
		while(ptr --> this->sources)
		{
			if(strcmp(identifier, (*ptr)->identifier) == 0) return *ptr;
		}
	}
	
	return NULL;
}



// ----------------------------------------------------------------- //
// Reallocate memory for one additional source                       //
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
//   source in the specified catalogue. Note that this will not cre- //
//   ate a new source yet, but just allocate the memory needed to    //
//   append a source at the end of the catalogue. The function       //
//   should be called from public member functions that will add     //
//   sources to a catalogue prior to inserting the new source.       //
// ----------------------------------------------------------------- //

private void Catalog_append_memory(Catalog *this)
{
	this->size += 1;
	this->sources = (Source **)realloc(this->sources, this->size * sizeof(Source *));
	ensure(this->sources != NULL, "Memory allocation for new catalogue source failed.");
	return;
}



public void Catalog_save(const Catalog *this, const char *filename, const file_format format)
{
	// Sanity checks
	ensure(this != NULL && this->size, "Invalid or empty catalogue provided.");
	
	// Open output file
	FILE *fp = fopen(filename, "w");
	ensure(fp != NULL, "Failed to open output file: %s", filename);
	
	// Get current date and time
	const time_t current_time = time(NULL);
	
	// Get first source to extract parameter names and units
	Source *src = this->sources[0];
	
	fprintf(fp, "# SoFiA source catalogue\n# Creator: " VERSION_FULL "\n# Date:    %s#\n", ctime(&current_time));
	fprintf(fp, "# Header rows:\n#   1 = column number\n#   2 = parameter name\n#   3 = parameter unit\n\n");
	for(size_t j = 0; j < src->n_par; ++j) fprintf(fp, "%*zu", CATALOG_COLUMN_WIDTH, j + 1);
	fprintf(fp, "\n");
	for(size_t j = 0; j < src->n_par; ++j) fprintf(fp, "%*s", CATALOG_COLUMN_WIDTH, Source_get_name(src, j));
	fprintf(fp, "\n");
	for(size_t j = 0; j < src->n_par; ++j) fprintf(fp, "%*s", CATALOG_COLUMN_WIDTH, Source_get_unit(src, j));
	fprintf(fp, "\n\n");
	
	// Loop over all sources to write parameters
	for(size_t i = 0; i < this->size; ++i)
	{
		Source *src = this->sources[i];
		
		for(size_t j = 0; j < src->n_par; ++j)
		{
			if(src->types[j] == 0) fprintf(fp, "%*ld", CATALOG_COLUMN_WIDTH, (long int)(src->values[j]));
			else fprintf(fp, "%*.5e\t", CATALOG_COLUMN_WIDTH - 5, *(double *)(&(src->values[j])));  // ALERT: This needs to be tested!
		}
		
		fprintf(fp, "\n"); // CONTINUE HERE...
	}
	
	fclose(fp);
	
	return;
}
