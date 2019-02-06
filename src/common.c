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
		fprintf(stderr, "\33[31mERROR: ");
		vfprintf(stderr, format, args);
		fprintf(stderr, "\33[0m\n");
		va_end(args);
		exit(1);
	}
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
	vprintf(format, args);
	va_end(args);
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
	if(VERBOSE)
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

void progress_bar(const char *text, const int progress, const int maximum)
{
	const int size = 50;
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

void timestamp(const clock_t start)
{
	printf("\n\33[36m--- Elapsed time: %.3f s --------------------------------------------------\33[0m\n\n", ((double)(clock() - start)) / ((double)CLOCKS_PER_SEC));
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
