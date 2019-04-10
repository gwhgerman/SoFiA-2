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
#include <stdint.h>
#include <ctype.h>

#include "common.h"



// ----------------------------------------------------------------- //
// Terminate programme execution if condition is false               //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   condition - Condition to be tested. If false, programme execu-  //
//               tion will be terminated with an error message.      //
//   format    - Message to be printed. This can contain optional    //
//               format specifiers as used in the printf() function. //
//   ...       - Optional parameters to be printed as defined by the //
//               format specifies, if present.                       //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   If the specified condition is false, an error message will be   //
//   printed and execution of the programme will be terminated with  //
//   a signal of 1. The specified error message can contain optional //
//   format specifiers as used in the printf() function, in which    //
//   case additional arguments need to be supplied that will be      //
//   printed as part of the message.                                 //
// ----------------------------------------------------------------- //

void ensure(const int condition, const char *format, ...)
{
	if(!condition)
	{
		va_list args;
		va_start(args, format);
		fprintf(stderr, "\n\33[31mERROR: ");
		vfprintf(stderr, format, args);
		fprintf(stderr, "\33[0m\n\n");
		va_end(args);
		exit(1);
	}
	return;
}



// ----------------------------------------------------------------- //
// Check if pointer is NULL                                          //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) ptr - Pointer to be checked.                                //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   This function will check if the supplied pointer is NULL and    //
//   terminate the current process in this case. Otherwise, the      //
//   function will do nothing.                                       //
// ----------------------------------------------------------------- //

void check_null(const void *ptr)
{
	ensure(ptr != NULL, "NULL pointer encountered.");
	return;
}



// ----------------------------------------------------------------- //
// Print informational message                                       //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   format - Message to be printed. This can contain optional for-  //
//            mat specifiers as used in the printf() function.       //
//   ...    - Optional parameters to be printed as defined by the    //
//            format specifies, if present.                          //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Prints an informational message to standard output. The message //
//   can contain optional format specifiers as used in the printf()  //
//   function, in which case additional arguments need to be sup-    //
//   plied that will be printed as part of the message.              //
// ----------------------------------------------------------------- //

void message(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	printf("  ");
	vprintf(format, args);
	printf("\n");
	va_end(args);
	return;
}

// Same, but with additional verbosity option

void message_verb(const bool verbosity, const char *format, ...)
{
	if(verbosity)
	{
		va_list args;
		va_start(args, format);
		printf("  ");
		vprintf(format, args);
		printf("\n");
		va_end(args);
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Print status message                                              //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   format - Message to be printed. This can contain optional for-  //
//            mat specifiers as used in the printf() function.       //
//   ...    - Optional parameters to be printed as defined by the    //
//            format specifies, if present.                          //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Prints an informational status message to standard output. The  //
//   message will be highlighted in some way in the terminal window  //
//   to distinguish it from standard messages. The message can con-  //
//   tain optional format specifiers as used in the printf() func-   //
//   tion, in which case additional arguments need to be supplied    //
//   that will be printed as part of the message.                    //
// ----------------------------------------------------------------- //

void status(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	printf("\33[36m____________________________________________________________________________\33[0;1m\n\n ");
	vprintf(format, args);
	printf("\n\33[0;36m____________________________________________________________________________\33[0m\n\n");
	va_end(args);
	return;
}



// ----------------------------------------------------------------- //
// Print warning message                                             //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   format - Message to be printed. This can contain optional for-  //
//            mat specifiers as used in the printf() function.       //
//   ...    - Optional parameters to be printed as defined by the    //
//            format specifies, if present.                          //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Prints the word 'WARNING' followed by the specified message and //
//   a final newline character to standard error. The message can    //
//   contain format specifiers as used in the printf() function, in  //
//   which case additional arguments are expected that will be prin- //
//   ted as part of the message. Output will be suppressed if the    //
//   VERBOSE parameter is set to false.                              //
// ----------------------------------------------------------------- //

void warning(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	fprintf(stderr, "\33[33mWARNING: ");
	vfprintf(stderr, format, args);
	fprintf(stderr, "\33[0m\n");
	va_end(args);
	
	return;
}

// Same, but with additional verbosity option

void warning_verb(const bool verbosity, const char *format, ...)
{
	if(verbosity)
	{
		va_list args;
		va_start(args, format);
		fprintf(stderr, "\33[33mWARNING: ");
		vfprintf(stderr, format, args);
		fprintf(stderr, "\33[0m\n");
		va_end(args);
	}
	
	return;
}



// ----------------------------------------------------------------- //
// Display progress bar                                              //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   text     - Text to appear in front of the progress bar.         //
//   progress - Progress value relative to maximum.                  //
//   maximum  - Maximum possible progress value.                     //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Displays a progress bar showing the current progress relative   //
//   to the maximum possible progress. The progress bar can be up-   //
//   dated by calling this function repeatedly until progress ==     //
//   maximum, in which case the progress is shown as 100%, and a     //
//   newline character will be printed to end the progress bar up-   //
//   date. No other output must occur in between the first and last  //
//   call to this function, as otherwise the progress bar would be   //
//   broken over onto a new line.                                    //
// ----------------------------------------------------------------- //

void progress_bar(const char *text, const size_t progress, const size_t maximum)
{
	const size_t size = 50;
	const size_t cur = size * progress / maximum;
	const bool status = (progress < maximum);
	size_t i;
	
	if(progress < maximum) printf("  %s |\33[33m", text);
	else printf("  %s |\33[32m", text);
	for(i = 0; i < cur; ++i) printf("=");
	if(status) for(i = cur; i < size; ++i) printf(" ");
	printf("\33[0m| %zu%%\r", 100 * progress / maximum);
	if(!status) printf("\n\n");
	fflush(stdout);
	return;
}



// ----------------------------------------------------------------- //
// Print time stamp                                                  //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   start - Start time relative to which the elapsed time is calcu- //
//           lated.                                                  //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Prints the elapsed time as the difference between the current   //
//   time and the specified start time.                              //
// ----------------------------------------------------------------- //

void timestamp(const time_t start)
{
	const unsigned long dt = difftime(time(NULL), start);
	const unsigned long h =  dt / 3600;
	const unsigned long m = (dt - 3600 * h) / 60;
	const unsigned long s =  dt - 3600 * h  - 60 * m;
	printf("\n  Elapsed time: %02ld:%02ld:%02ld h\n\n", h, m, s);
	return;
}



// ----------------------------------------------------------------- //
// Print current memory usage                                        //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   No arguments.                                                   //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Prints the current virtual memory usage in MB. The memory usage //
//   is extracted from /proc/self/statm, which will only work on Li- //
//   nux or Unix systems. If the file cannot be read for some reason //
//   the function will do nothing.                                   //
// ----------------------------------------------------------------- //

void memory_usage(void)
{
	FILE *fp = fopen( "/proc/self/statm", "r" );
	if(fp == NULL) return;
	char line[81];
	if(fgets(line, sizeof(line), fp) != NULL) message("Memory usage: %.1f MB", (double)strtol(line, NULL, 10) / 1024.0);
	fclose(fp);
	return;
}



// ----------------------------------------------------------------- //
// Trim whitespace from string                                       //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   str - String to be trimmed.                                     //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns a pointer to the beginning of the trimmed string.       //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Trims a string by removing whitespace from the beginning and    //
//   end of the string. Whitespace will be any character that is     //
//   considered as space by the isspace() function of the standard   //
//   library.                                                        //
//                                                                   //
//   Note that this will modify the original string and return a     //
//   pointer to a different position in the specified string. This   //
//   means that the original pointer must not be overwritten or dis- //
//   carded as otherwise its memory allocation could not be freed    //
//   any more! Furthermore, the returned string pointer must not be  //
//   freed under any circumstances, as it points to memory already   //
//   allocated elsewhere.                                            //
// ----------------------------------------------------------------- //

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



// ----------------------------------------------------------------- //
// Convert integer number to string                                  //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   str   - String to hold the result.                              //
//   size  - Size of the string.                                     //
//   value - Integer value to convert.                               //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Function for converting the specified integer value into a      //
//   string of size 'size' pointed to by 'str'. The user will need   //
//   to ensure that the string is large enough to be able to hold    //
//   the result. If the string is too small, the value will be trun- //
//   cated.                                                          //
// ----------------------------------------------------------------- //

void int_to_str(char *str, const size_t size, const long int value)
{
	snprintf(str, size, "%ld", value);
	return;
}



// ----------------------------------------------------------------- //
// Swap two values                                                   //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   val1  - Pointer to first value.                                 //
//   val2  - Pointer to second value.                                //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Function for swapping two double-precision floating-point val-  //
//   ues.                                                            //
// ----------------------------------------------------------------- //

void swap(double *val1, double *val2)
{
	check_null(val1);
	check_null(val2);
	
	double tmp = *val1;
	*val1 = *val2;
	*val2 = tmp;
	
	return;
}




// ----------------------------------------------------------------- //
// Check native endianness of system                                 //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   No arguments.                                                   //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   Returns true on little-endian and false on big-endian machines. //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Static function for checking the native endianness of the ma-   //
//   chine at run time. Note that the result will only be correct on //
//   machines where sizeof(char) < sizeof(int).                      //
// ----------------------------------------------------------------- //

bool is_little_endian(void)
{
	const unsigned int n = 1U;
	return *((unsigned char *)&n) == 1U;
}



// ----------------------------------------------------------------- //
// Swap the byte order of a multi-byte word                          //
// ----------------------------------------------------------------- //
// Arguments:                                                        //
//                                                                   //
//   (1) word - Pointer to a multi-byte value.                       //
//   (2) size - Number of bytes used by word; must be 2, 4 or 8.     //
//                                                                   //
// Return value:                                                     //
//                                                                   //
//   No return value.                                                //
//                                                                   //
// Description:                                                      //
//                                                                   //
//   Static function for reversing the order of the bytes in a mul-  //
//   ti-byte word provided as an array of char with given size. Only //
//   word sizes of 2, 4 or 8 are currently supported. Note that this //
//   function will only work correctly on systems where CHAR_BIT is  //
//   equal to 8. In addition, the built-in byte swap functions from  //
//   GCC are required.                                               //
// ----------------------------------------------------------------- //

void swap_byte_order(char *word, const size_t size)
{
	if(size == 2)
	{
		uint16_t tmp;
		memcpy(&tmp, word, size);
		tmp = __builtin_bswap16(tmp);
		memcpy(word, &tmp, size);
	}
	else if(size == 4)
	{
		uint32_t tmp;
		memcpy(&tmp, word, size);
		tmp = __builtin_bswap32(tmp);
		memcpy(word, &tmp, size);
	}
	else if(size == 8)
	{
		uint64_t tmp;
		memcpy(&tmp, word, size);
		tmp = __builtin_bswap64(tmp);
		memcpy(word, &tmp, size);
	}
	
	return;
}
