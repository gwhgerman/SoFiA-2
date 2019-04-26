/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (Path.c) - Source Finding Application               ///
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

#include "Path.h"



// ----------------------------------------------------------------- //
// Declaration of private properties and methods of class Path       //
// ----------------------------------------------------------------- //

class Path
{
	char *dir;
	char *file;
	char *path;
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
//   Pointer to newly created Path object.                           //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Standard constructor. Will create a new and empty Path object   //
//   and return a pointer to the newly created object. No memory     //
//   will be allocated other than for the object itself. Note that   //
//   the destructor will need to be called explicitly once the       //
//   object is no longer required to release any memory allocated    //
//   during the lifetime of the object.                              //
// ----------------------------------------------------------------- //

public Path *Path_new(void)
{
	Path *this = (Path *)malloc(sizeof(Path));
	ensure(this != NULL, "Failed to allocate memory for Path object.");
	
	this->dir = NULL;
	this->file = NULL;
	this->path = NULL;
	
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

public void Path_delete(Path *this)
{
	if(this != NULL)
	{
		free(this->dir);
		free(this->file);
		free(this->path);
		free(this);
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Set path from string                                              //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) path     - String from which to extract path.               //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for setting the path from the specified string.   //
//   The path can contain just a directory, just a file, or a combi- //
//   nation of both. Anything before the final '/' will be treated   //
//   as a directory and anything after the final '/' as a file name. //
// ----------------------------------------------------------------- //

public void Path_set(Path *this, const char *path)
{
	// Sanity checks
	check_null(this);
	check_null(path);
	
	const size_t size = strlen(path);
	ensure(size, "Empty path name encountered.");
	
	// Check for last slash
	const char *delimiter = strrchr(path, '/');
	
	if(delimiter == NULL)
	{
		// No directory, just file name
		Path_set_file(this, path);
		free(this->dir);
		this->dir = NULL;
	}
	else if(delimiter == path + size - 1)
	{
		// No file name, just directory
		Path_set_dir(this, path);
		free(this->file);
		this->file = NULL;
	}
	else
	{
		// Both directory and file name
		
		// Copy directory
		this->dir = (char *)realloc(this->dir, (delimiter - path + 2) * sizeof(char));
		ensure(this->dir != NULL, "Memory allocation error while setting Path object.");
		strncpy(this->dir, path, delimiter - path + 1);
		this->dir[delimiter - path + 1] = '\0';
		
		// Copy file
		this->file = (char *)realloc(this->file, (size - (delimiter - path)) * sizeof(char));
		ensure(this->file != NULL, "Memory allocation error while setting Path object.");
		strcpy(this->file, delimiter + 1);
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Set file name of path                                             //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) file     - String from which to extract file name.          //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for setting the file name of the specified path.  //
//   The directory name of the path will be left unchanged.          //
// ----------------------------------------------------------------- //

public void Path_set_file(Path *this, const char *file)
{
	// Sanity checks
	check_null(this);
	check_null(file);
	
	const size_t size = strlen(file);
	ensure(size, "Empty file name encountered.");
	
	// (Re-)allocate memory and copy file name
	this->file = (char *)realloc(this->file, (size + 1) * sizeof(char));
	ensure(this->file != NULL, "Memory allocation error while setting Path object.");
	
	strcpy(this->file, file);
	
	return;
}



// ----------------------------------------------------------------- //
// Set directory name of path                                        //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) file     - String from which to extract directory name.     //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for setting the directory name of the specified   //
//   path. The file name of the path will be left unchanged.         //
// ----------------------------------------------------------------- //

public void Path_set_dir(Path *this, const char *dir)
{
	// Sanity checks
	check_null(this);
	check_null(dir);
	
	const size_t size = strlen(dir);
	ensure(size, "Empty directory name encountered.");
	
	// (Re-)allocate memory and copy directory name with trailing slash
	if(dir[size - 1] == '/')
	{
		this->dir = (char *)realloc(this->dir, (size + 1) * sizeof(char));
		ensure(this->dir != NULL, "Memory allocation error while setting Path object.");
		strcpy(this->dir, dir);
	}
	else
	{
		this->dir = (char *)realloc(this->dir, (size + 2) * sizeof(char));
		ensure(this->dir != NULL, "Memory allocation error while setting Path object.");
		strcpy(this->dir, dir);
		this->dir[size] = '/';
		this->dir[size + 1] = '\0';
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Append directory name of path from template                       //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) basename - Basename to be used for the subdirectory name to //
//                  be appended.                                     //
//   (3) appendix - Suffix to be appended to the subdirectory name.  //
//                  NOTE that any connecting character such as '_'   //
//                  must be explicitly included.                     //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for appending a subdirectory to the given direc-  //
//   tory. The name of the subdirectory will be constructed from the //
//   specified basename and appendix. Basename is expected to be a   //
//   file name, and if it includes a mime type suffix (e.g. .fits),  //
//   then that suffix will be removed first before concatenating the //
//   appendix. Note that neither basename nor appendix must contain  //
//   a slash (/).                                                    //
//                                                                   //
//   Example:                                                        //
//                                                                   //
//     Assume this->dir = "/home/user/"                              //
//            basename  = "data.fits"                                //
//            appendix  = "_cubelets"                                //
//                                                                   //
//     Then   this->dir = "/home/user/data_cubelets/"                //
// ----------------------------------------------------------------- //

public void Path_append_dir_from_template(Path *this, const char *basename, const char *appendix)
{
	// Sanity checks
	check_null(this);
	check_null(basename);
	check_null(appendix);
	ensure(strchr(basename, '/') == NULL && strchr(appendix, '/') == NULL, "Basename and appendix must not contain \'/\'.");
	
	const size_t size_old = this->dir == NULL ? 0 : strlen(this->dir);
	const size_t size_app = strlen(appendix);
	
	// Check if basename includes mimetype
	const char *dot = strrchr(basename, '.');
	size_t size_base = strlen(basename);
	if(dot != NULL && dot != basename) size_base = dot - basename;
	
	// Reallocate memory
	this->dir = (char *)realloc(this->dir, (size_old + size_base + size_app + 2) * sizeof(char));
	ensure(this->dir != NULL, "Memory allocation error while setting Path object.");
	
	memcpy(this->dir + size_old, basename, size_base);
	memcpy(this->dir + size_old + size_base, appendix, size_app);
	*(this->dir + size_old + size_base + size_app) = '/';
	*(this->dir + size_old + size_base + size_app + 1) = '\0';
	
	return;
}



// ----------------------------------------------------------------- //
// Construct file name of path from specified template               //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//   (2) basename - Base name to be used for the file name.          //
//   (3) suffix   - Suffix to be appended to basename such that the  //
//                  file name becomes "basename" "suffix". NOTE that //
//                  any connecting character (like '_') must be ex-  //
//                  plicitly included.                               //
//   (4) mimetype - Mime type to be appended to file name such that  //
//                  the name becomes "basename" "suffix" "mimetype". //
//                  NOTE that the dot ('.') must be explicitly in-   //
//                  cluded.                                          //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for constructing the file name of the specified   //
//   path from the given template. The final file name will become   //
//   "basename" "suffix" "mimetype". The directory name of the path  //
//   will remain unchanged.                                          //
//   Note that if 'basename' already contains a mimetype (identified //
//   by the presence of a dot that is not the first character), then //
//   that mimetype ending will be removed first.                     //
//                                                                   //
//   Example:                                                        //
//                                                                   //
//     Assume basename = "data.fits"                                 //
//            suffix   = "-mask"                                     //
//            mimetype = ".fits"                                     //
//                                                                   //
//     Then   filename = "data-mask.fits"                            //
// ----------------------------------------------------------------- //

public void Path_set_file_from_template(Path *this, const char *basename, const char *suffix, const char *mimetype)
{
	// Sanity checks
	check_null(this);
	check_null(basename);
	check_null(suffix);
	check_null(mimetype);
	
	// Check if basename includes mimetype
	const char *dot = strrchr(basename, '.');
	size_t basename_size = strlen(basename);
	if(dot != NULL && dot != basename) basename_size = dot - basename;
	
	// Reallocate memory
	this->file = (char *)realloc(this->file, (basename_size + strlen(suffix) + strlen(mimetype) + 1) * sizeof(char));
	ensure(this->file != NULL, "Memory allocation error while setting Path object.");
	
	memcpy(this->file, basename, basename_size);
	*(this->file + basename_size) = '\0';
	strcat(this->file, suffix);
	strcat(this->file, mimetype);
	
	return;
}



// ----------------------------------------------------------------- //
// Return the full path as a string                                  //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   String containing the full path.                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for returning the full path of the specified Path //
//   object as a string. The string will be internally stored as     //
//   part of the object and will be automatically deleted whenever   //
//   the destructor is called.                                       //
// ----------------------------------------------------------------- //

public const char *Path_get(Path *this)
{
	// Sanity checks
	check_null(this);
	
	size_t size = 0;
	
	if(this->dir != NULL) size += strlen(this->dir);
	if(this->file != NULL) size += strlen(this->file);
	
	if(size == 0) return NULL;
	
	this->path = (char *)realloc(this->path, (size + 1) * sizeof(char));
	ensure(this->path != NULL, "Memory allocation error while retrieving Path object.");
	
	if(this->dir != NULL) strcpy(this->path, this->dir);
	
	if(this->file != NULL)
	{
		if(this->dir != NULL) strcpy(this->path + strlen(this->dir), this->file);
		else strcpy(this->path, this->file);
	}
	
	return this->path;
}



public const char *Path_get_dir(const Path *this)
{
	check_null(this);
	return this->dir;
}



public const char *Path_get_file(const Path *this)
{
	check_null(this);
	return this->file;
}



// ----------------------------------------------------------------- //
// Check if file stored in path is readable                          //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) this     - Object self-reference.                           //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   True if path is readable, false otherwise.                      //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Public method for testing of the file pointed to by the speci-  //
//   fied path is readable or not. Note that this does not test if   //
//   the file exists, as it could exist but not be readable, e.g. as //
//   a result of insufficient read permission.                       //
// ----------------------------------------------------------------- //

public bool Path_file_is_readable(Path *this)
{
	check_null(this);
	FILE *fp = fopen(Path_get(this), "r");
	if(fp == NULL) return false;
	fclose(fp);
	return true;
}
