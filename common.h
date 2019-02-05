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
void ensure(const int condition, const char *message);
void ensure_var(const int condition, const int n_args, ...);

// Print warning message
void warning(const char *message);
void warning_verb(const bool verbose, const char *message);
void warning_var(const int n_args, ...);
void warning_var_verb(const bool verbose, const int n_args, ...);

// Trim string
char *trim_string(char *str);

// Display progress bar and time stamp
void show_progress_bar(const char *text, const int progress, const int maximum);
void print_timestamp(const clock_t start);

#endif
