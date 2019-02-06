/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (Parameter.c) - Source Finding Application          ///
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

#include "Parameter.h"



// ----------------------------------------------------------------- //
// Declaration of private methods of Parameter                       //
// ----------------------------------------------------------------- //

private void Parameter_append_memory(Parameter *this);
private char *Parameter_get_raw(const Parameter *this, const char *key);



// ----------------------------------------------------------------- //
// Standard constructor                                              //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   No arguments.                                                   //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Pointer to newly created Parameter object.                      //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Standard constructor. Will create a new and empty Parameter ob- //
//   ject and return a pointer to the newly created object. No memo- //
//   ry will be allocated other than for the object itself. Note     //
//   that the destructor will need to be called explicitly once the  //
//   object is no longer required to release any memory allocated    //
//   during the lifetime of the object.                              //
// ----------------------------------------------------------------- //

public Parameter *Parameter_new(void)
{
	Parameter *this = (Parameter *)malloc(sizeof(Parameter));
	ensure(this != NULL, "Failed to allocate memory for new parameter object.");
	
	this->n_par  = 0;
	this->keys   = NULL;
	this->values = NULL;
	
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

public void Parameter_delete(Parameter *this)
{
	if(this != NULL)
	{
		if(this->keys != NULL)
		{
			for(size_t i = this->n_par; i--;) free(this->keys[i]);
			free(this->keys);
		}
		
		if(this->values != NULL)
		{
			for(size_t i = this->n_par; i--;) free(this->values[i]);
			free(this->values);
		}
		
		free(this);
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Set parameter to given value                                      //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) key      - Name of the parameter to be set.                 //
//   (3) value    - String containing the value of the parameter.    //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for setting a parameter to a specified value. If  //
//   a parameter of that name already exists, its previous value     //
//   will be overwritten. Otherwise, a new parameter will be append- //
//   ed to the current parameter list.                               //
// ----------------------------------------------------------------- //

public void Parameter_set(Parameter *this, const char *key, const char *value)
{
	// Check if parameter already exists
	size_t index = Parameter_exists(this, key);
	
	if(index)
	{
		--index;
		free(this->values[index]);
		warning("Parameter \'%s\' already exists.\n         Replacing existing definition.", key);
	}
	else
	{
		Parameter_append_memory(this);
		index = this->n_par - 1;
		this->keys[index] = (char *)malloc((strlen(key) + 1) * sizeof(char));
		ensure(this->keys[index] != NULL, "Failed to allocate memory for new parameter setting.");
		strcpy(this->keys[index], key);
	}
	
	this->values[index] = (char *)malloc((strlen(value) + 1) * sizeof(char));
	ensure(this->values[index] != NULL, "Failed to allocate memory for new parameter setting.");
	strcpy(this->values[index], value);
	
	return;
}



// ----------------------------------------------------------------- //
// Check if parameter exists                                         //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) key      - Name of the parameter to be checked.             //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns the index number of the parameter, if found. Otherwise, //
//   a value of 0 will be returned.                                  //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for checking if the specified parameter name al-  //
//   ready exists in the current parameter list. If the parameter is //
//   found, its index number (i.e. row number in the current list)   //
//   will be returned. Otherwise, a value of 0 will be returned.     //
//   Note that indices start with 1 rather than 0.                   //
// ----------------------------------------------------------------- //

public int Parameter_exists(const Parameter *this, const char *key)
{
	ensure(this != NULL, "Invalid Parameter object provided.");
	ensure(strlen(key), "Empty parameter keyword provided.");
	
	if(this->n_par)
	{
		char **ptr = this->keys + this->n_par;
		size_t i = this->n_par;
		
		while(ptr --> this->keys)
		{
			if(strcmp(key, *ptr) == 0) return i;
			--i;
		}
	}
	
	return 0;
}



// ----------------------------------------------------------------- //
// Extract parameter value as floating-point number                  //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) key      - Name of the parameter to be extracted.           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns the value of the specified parameter.                   //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning the value of the specified parame-  //
//   ter as a double-precision floating-point number. If the parame- //
//   ter does not exist, a value of NaN will instead be returned.    //
// ----------------------------------------------------------------- //

public double Parameter_get_flt(const Parameter *this, const char *key)
{
	const char *value_raw = Parameter_get_raw(this, key);
	if(value_raw == NULL) return NAN;
	return strtod(value_raw, NULL);
}



// ----------------------------------------------------------------- //
// Extract parameter value as integer number                         //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) key      - Name of the parameter to be extracted.           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns the value of the specified parameter.                   //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning the value of the specified parame-  //
//   ter as a long integer number. If the parameter does not exist,  //
//   a value of 0 will instead be returned.                          //
// ----------------------------------------------------------------- //

public long int Parameter_get_int(const Parameter *this, const char *key)
{
	const char *value_raw = Parameter_get_raw(this, key);
	if(value_raw == NULL) return 0L;
	return strtol(value_raw, NULL, 10);
}



// ----------------------------------------------------------------- //
// Extract parameter value as Boolean value                          //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) key      - Name of the parameter to be extracted.           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns the value of the specified parameter.                   //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning the value of the specified parame-  //
//   ter as a Boolean value (true or false). If the parameter does   //
//   not exist, a value of false will be returned.                   //
// ----------------------------------------------------------------- //

public bool Parameter_get_bool(const Parameter *this, const char *key)
{
	const char *value_raw = Parameter_get_raw(this, key);
	if(value_raw == NULL) return false;
	return strcmp(value_raw, "true") == 0;
}



// ----------------------------------------------------------------- //
// Extract parameter value as String                                 //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) key      - Name of the parameter to be extracted.           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns a pointer to the string representing the value.         //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning the value of the specified parame-  //
//   ter as a pointer to a string value. If the parameter does not   //
//   exist, a NULL pointer will be returned. Note that this is just  //
//   a convenience function that will simply return the output of    //
//   Parameter_get_raw().                                            //
// ----------------------------------------------------------------- //

public char *Parameter_get_str(const Parameter *this, const char *key)
{
	return Parameter_get_raw(this, key);
}



// ----------------------------------------------------------------- //
// Extract parameter value as raw string                             //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) key      - Name of the parameter to be extracted.           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns the value of the specified parameter as a raw string.   //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Private method for returning the value of the specified parame- //
//   ter as a pointer to the raw value string. If the parameter does //
//   not exist, a NULL pointer will instead be returned.             //
// ----------------------------------------------------------------- //

private char *Parameter_get_raw(const Parameter *this, const char *key)
{
	size_t index = Parameter_exists(this, key);
	if(index)
	{
		--index;
		return this->values[index];
	}
	return NULL;
}



// ----------------------------------------------------------------- //
// Load parameter settings from file                                 //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) filename - Name of the input file.                          //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for loading parameter settings from an input file //
//   of the name 'filename'. The parameter settings in the file must //
//   be of the form                                                  //
//                                                                   //
//     key = [value] [# comment]                                     //
//                                                                   //
//   where both the value and comment are optional (indicated by the //
//   brackets). Empty lines and lines starting with # (comments)     //
//   will be ignored. No checks will be made if a keyword already    //
//   exists, in which case the existing value would simply be over-  //
//   written.                                                        //
// ----------------------------------------------------------------- //

public void Parameter_load(Parameter *this, const char *filename)
{
	// Sanity checks
	ensure(this != NULL, "Invalid Parameter object provided.");
	ensure(strlen(filename), "Empty file name provided.");
	
	// Try to open file
	FILE *fp = fopen(filename, "r");
	ensure(fp != NULL, "Failed to open input file: %s.", filename);
	
	// Allocate memory for a single line
	char *line = (char *)malloc(MAX_LINE_SIZE * sizeof(char));
	ensure(line != NULL, "Memory allocation error while reading file.");
	
	// Read lines from file
	while(fgets(line, MAX_LINE_SIZE, fp))
	{
		// Trim line and check for comments and empty lines
		char *trimmed = trim_string(line);
		if(strlen(trimmed) == 0 || trimmed[0] == '#') continue;
		
		// Extract keyword
		char *key = trim_string(strtok(trimmed, "="));
		if(key == NULL || strlen(key) == 0)
		{
			warning("Failed to parse the following setting:\n         %s", trimmed);
			continue;
		}
		
		// Extract value and insert into parameter list
		char *value = trim_string(strtok(NULL, "#"));
		if(value == NULL || strlen(value) == 0)
		{
			warning("Parameter \'%s\' has no value.", key);
			Parameter_set(this, key, "");
		}
		else
		{
			Parameter_set(this, key, value);
		}
	}
	
	// De-allocate memory and close file
	free(line);
	fclose(fp);
	
	return;
}



// ----------------------------------------------------------------- //
// Reallocate memory for one additional parameter setting            //
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
//   parameter in the specified parameter list. Note that this will  //
//   not create a new parameter setting yet, but will just allocate  //
//   the memory needed to append a parameter at the end of the cur-  //
//   rent list. The function should be called from public member     //
//   functions that will add parameters to a parameter list prior to //
//   inserting the new parameter name and value.                     //
// ----------------------------------------------------------------- //

private void Parameter_append_memory(Parameter *this)
{
	// Extend memory for parameter settings
	if(this->n_par)
	{
		this->n_par += 1;
		this->keys   = (char **)realloc(this->keys,   this->n_par * sizeof(char *));
		this->values = (char **)realloc(this->values, this->n_par * sizeof(char *));
	}
	else
	{
		this->n_par  = 1;
		this->keys   = (char **)malloc(this->n_par * sizeof(char *));
		this->values = (char **)malloc(this->n_par * sizeof(char *));
	}
	
	ensure(this->keys != NULL && this->values != NULL, "Memory allocation for new parameter setting failed.");
	
	return;
}
