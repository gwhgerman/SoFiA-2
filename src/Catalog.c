/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (Catalog.c) - Source Finding Application            ///
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

#include "Catalog.h"



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
	// Sanity checks
	check_null(this);
	check_null(src);
	ensure(!Catalog_source_exists(this, src, NULL), "Source \'%s\' is already in catalogue.", Source_get_identifier(src));
	
	Catalog_append_memory(this);
	*(this->sources + this->size - 1) = src;
	
	return;
}



// ----------------------------------------------------------------- //
// Get source index                                                  //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) src      - Pointer to the source to be checked              //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns the index (i.e. row number) of the source within the    //
//   catalogue if the source was found. Otherwise, SIZE_MAX will be  //
//   returned.                                                       //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for checking if the specified source is included  //
//   in the catalogue. If so, the function will return the row num-  //
//   ber of the source in the catalogue (starting with 0). If the    //
//   source is not found, the function will return SIZE_MAX.         //
// ----------------------------------------------------------------- //

public size_t Catalog_get_index(const Catalog *this, const Source *src)
{
	// Sanity checks
	check_null(this);
	check_null(src);
	
	for(size_t i = 0; i < this->size; ++i)
	{
		if(this->sources[i] == src) return i;
	}
	
	return SIZE_MAX;
}



// ----------------------------------------------------------------- //
// Check if source exists in catalogue                               //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) src      - Pointer to the source to be checked              //
//   (3) index    - Pointer to index variable that will be set to    //
//                  the catalogue index of the source.               //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns true if the source is included in the catalogue and     //
//   false otherwise.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for checking if the specified source is included  //
//   in the catalogue. If so, the function will return true, other-  //
//   wise false. If the source is found, the variable 'index' will   //
//   be set to the catalogue index of the source. Otherwise, it will //
//   be left untouched. If no index is required, a NULL pointer can  //
//   instead be provided.
// ----------------------------------------------------------------- //

public bool Catalog_source_exists(const Catalog *this, const Source *src, size_t *index)
{
	// Sanity checks
	check_null(this);
	check_null(src);
	
	for(size_t i = 0; i < this->size; ++i)
	{
		if(this->sources[i] == src)
		{
			if(index != NULL) *index = i;
			return true;
		}
	}
	
	return false;
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
			if(strcmp(identifier, Source_get_identifier(*ptr)) == 0) return *ptr;
		}
	}
	
	return NULL;
}



// ----------------------------------------------------------------- //
// Save catalogue to file                                            //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) filename - Full path to the output file.                    //
//   (3) format   - Output format; can be CATALOG_FORMAT_ASCII for   //
//                  plain text ASCII files, CATALOG_FORMAT_XML for   //
//                  VOTable format or CATALOG_FORMAT_SQL for SQL     //
//                  table format (not yet supported).                //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for saving the current catalogue under the speci- //
//   fied name in the specified file format. The file name will be   //
//   relative to the process execution directory unless the full     //
//   path to the output directory is specified. Available formats    //
//   are plain text ASCII, VOTable XML format and SQL format.        //
// ----------------------------------------------------------------- //

public void Catalog_save(const Catalog *this, const char *filename, const file_format format)
{
	// Sanity checks
	ensure(this != NULL && this->size, "Invalid or empty catalogue provided.");
	
	// Open output file
	FILE *fp = fopen(filename, "w");
	ensure(fp != NULL, "Failed to open output file: %s", filename);
	
	// Some initial definitions
	const char char_comment = '#';
	const char char_nocomment = ' ';
	const char *data_type_names[2] = {"long", "double"};
	const char *indentation[7] = {"", "\t", "\t\t", "\t\t\t", "\t\t\t\t", "\t\t\t\t\t", "\t\t\t\t\t\t"}; // Better readability
	//const char *indentation[7] = {"", "", "", "", "", "", ""}; // Smaller file size
	
	// Get current date and time
	const time_t current_time = time(NULL);
	
	// Get first source to extract parameter names and units
	Source *src = this->sources[0];
	
	if(format == CATALOG_FORMAT_XML)
	{
		// Write XML catalogue (VOTable)
		fprintf(fp, "%s<?xml version=\"1.0\" ?>\n", indentation[0]);
		fprintf(fp, "%s<VOTABLE version=\"1.3\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.ivoa.net/xml/VOTable/v1.3\">\n", indentation[0]);
		fprintf(fp, "%s<RESOURCE name=\"SoFiA catalogue (version %s)\">\n", indentation[1], VERSION);
		fprintf(fp, "%s<DESCRIPTION>Source catalogue produced by the Source Finding Application (SoFiA) version %s</DESCRIPTION>\n", indentation[2], VERSION);
		fprintf(fp, "%s<COOSYS ID=\"wcs\" system=\"eq_FK5\" equinox=\"J2000\"/>\n", indentation[2]);
		// WARNING: COOSYS needs to be fixed; see http://www.ivoa.net/documents/VOTable/ for documentation
		fprintf(fp, "%s<TABLE ID=\"sofia_catalog\" name=\"SoFiA source catalogue\">\n", indentation[2]);
		
		// Column descriptors
		for(size_t j = 0; j < Source_get_num_par(src); ++j)
		{
			fprintf(fp, "%s<FIELD datatype=\"%s\" name=\"%s\" unit=\"%s\" ucd=\"%s\"/>\n", indentation[3], data_type_names[Source_get_type(src, j)], Source_get_name(src, j), Source_get_unit(src, j), Source_get_ucd(src, j));
		}
		
		// Start of data table
		fprintf(fp, "%s<DATA>\n", indentation[3]);
		fprintf(fp, "%s<TABLEDATA>\n", indentation[4]);
		
		// Data rows
		for(size_t i = 0; i < this->size; ++i)
		{
			Source *src = this->sources[i];
			fprintf(fp, "%s<TR>\n", indentation[5]);
			
			for(size_t j = 0; j < Source_get_num_par(src); ++j)
			{
				if(Source_get_type(src, j) == SOURCE_TYPE_INT) fprintf(fp, "%s<TD>%ld</TD>\n", indentation[6], (long int)(Source_get_par_int(src, j)));
				else fprintf(fp, "%s<TD>%e</TD>\n", indentation[6], (double)(Source_get_par_flt(src, j)));
			}
			
			fprintf(fp, "%s</TR>\n", indentation[5]);
		}
		
		// End of data table
		fprintf(fp, "%s</TABLEDATA>\n", indentation[4]);
		fprintf(fp, "%s</DATA>\n", indentation[3]);
		
		// Finalise XML file
		fprintf(fp, "%s</TABLE>\n", indentation[2]);
		fprintf(fp, "%s</RESOURCE>\n", indentation[1]);
		fprintf(fp, "%s</VOTABLE>\n", indentation[0]);
	}
	else
	{
		// Write ASCII catalogue
		fprintf(fp, "# SoFiA source catalogue\n# Creator: " VERSION_FULL "\n# Date:    %s#\n", ctime(&current_time));
		fprintf(fp, "# Header rows:\n#   1 = column number\n#   2 = parameter name\n#   3 = parameter unit\n%c\n%c", char_comment, char_comment);
		for(size_t j = 0; j < Source_get_num_par(src); ++j) fprintf(fp, "%*zu", CATALOG_COLUMN_WIDTH, j + 1);
		fprintf(fp, "\n%c", char_comment);
		for(size_t j = 0; j < Source_get_num_par(src); ++j) fprintf(fp, "%*s", CATALOG_COLUMN_WIDTH, Source_get_name(src, j));
		fprintf(fp, "\n%c", char_comment);
		for(size_t j = 0; j < Source_get_num_par(src); ++j) fprintf(fp, "%*s", CATALOG_COLUMN_WIDTH, Source_get_unit(src, j));
		fprintf(fp, "\n\n");
		
		// Loop over all sources to write parameters
		for(size_t i = 0; i < this->size; ++i)
		{
			Source *src = this->sources[i];
			
			fprintf(fp, "%c", char_nocomment);
			
			for(size_t j = 0; j < Source_get_num_par(src); ++j)
			{
				if(Source_get_type(src, j) == SOURCE_TYPE_INT) fprintf(fp, "%*ld", CATALOG_COLUMN_WIDTH, (long int)(Source_get_par_int(src, j)));
				else fprintf(fp, "%*.5e\t", CATALOG_COLUMN_WIDTH, (double)(Source_get_par_flt(src, j)));
			}
			
			fprintf(fp, "\n");
		}
	}
	
	fclose(fp);
	
	return;
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
