/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (common.c) - Source Finding Application             ///
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
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include "common.h"

void ensure(const int condition, const char *message)
{
	if(!condition)
	{
		fprintf(stderr, "ERROR: %s\n", message);
		exit(1);
	}
	return;
}

void ensure_var(const int condition, const int n_args, ...)
{
	if(!condition)
	{
		va_list valist;
		va_start(valist, n_args);
		
		fprintf(stderr, "ERROR: ");
		for(int i = 0; i < n_args; ++i) fprintf(stderr, "%s", va_arg(valist, char *));
		fprintf(stderr, "\n");
		
		va_end(valist);
		exit(1);
	}
	return;
}

void warning(const char *message)
{
	fprintf(stderr, "WARNING: %s\n", message);
	return;
}

void warning_verb(const bool verbose, const char *message)
{
	if(verbose) warning(message);
	return;
}

void warning_var(const int n_args, ...)
{
	va_list valist;
	va_start(valist, n_args);
	
	fprintf(stderr, "WARNING: ");
	for(int i = 0; i < n_args; ++i) fprintf(stderr, "%s", va_arg(valist, char *));
	fprintf(stderr, "\n");
	
	va_end(valist);
	return;
}

void warning_var_verb(const bool verbose, const int n_args, ...)
{
	if(verbose)
	{
		va_list valist;
		va_start(valist, n_args);
		
		fprintf(stderr, "WARNING: ");
		for(int i = 0; i < n_args; ++i) fprintf(stderr, "%s", va_arg(valist, char *));
		fprintf(stderr, "\n");
		
		va_end(valist);
	}
	
	return;
}



// Trim whitespace from string                                          //
//                                                                      //
// Note that this will modify the original string and return a pointer  //
// to a different position in the specified string. This means that the //
// original pointer must not be overwritten or discarded as otherwise   //
// its memory allocation could not be freed any more! Furthermore, the  //
// returned string pointer must not be freed under any circumstances,   //
// as it points to memory already allocated elsewhere.                  //

char *trim_string(char *str)
{
	if(str != NULL)
	{
		// Trim leading whitespace
		while(isspace((unsigned char)(*str))) ++str;
		
		// All whitespace?
		if(*str == 0)  return str;
		
		// Trim trailing whitespace
		char *end = str + strlen(str) - 1;
		while(end > str && isspace((unsigned char)*end)) --end;
		
		// Terminate string with NUL
		end[1] = '\0';
	}
	
	return str;
}



// Display progress bar //

void show_progress_bar(const char *text, const int progress, const int maximum)
{
	const int size = 50;               /* Size of progress bar in characters */
	const int cur = size * progress / maximum;
	const int status = (progress < maximum);
	int i;
	
	printf("%s |", text);
	for(i = 0; i < cur; ++i) printf("=");
	if(status) for(i = cur; i <= size; ++i) printf(" ");
	printf("| %d%%\r", 100 * progress / maximum);
	if(!status) printf("\n");
	fflush(stdout);
	return;
}



// Print time stamp //

void print_timestamp(const clock_t start)
{
	printf("\nTime elapsed: %.3f s\n\n", ((double)(clock() - start)) / ((double)CLOCKS_PER_SEC));
	return;
}
