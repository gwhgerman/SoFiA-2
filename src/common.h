/// ____________________________________________________________________ ///
///                                                                      ///
/// SoFiA 2.0.0-beta (common.h) - Source Finding Application             ///
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

#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <time.h>

// SoFiA version number
#define VERSION "2.0.0-beta"
#define VERSION_FULL "SoFiA 2.0.0-beta"

// Verbosity
#define VERBOSE false

// Define object-oriented terminology
#define class struct
#define public extern
#define private static

// Generic compile time check; should result in a compiler error if
// condition is false due to attempt to create array of negative size
#define COMPILE_TIME_CHECK(cond, mess) typedef char mess[(cond) ? 1 : -1]

// Check condition and exit if not met
void ensure(const int condition, const char *format, ...);

// Print info and warning messages
void message(const char *format, ...);
void status(const char *format, ...);
void warning(const char *format, ...);

// Display progress bar and time stamp
void progress_bar(const char *text, const int progress, const int maximum);
void timestamp(const clock_t start);

// Trim string
char *trim_string(char *str);

#endif
