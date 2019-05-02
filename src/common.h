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
#define SOFIA_VERSION "2.0.0-beta"
#define SOFIA_VERSION_FULL "SoFiA 2.0.0-beta"

// Define mathematical constants
#ifndef M_PI
#define M_PI 3.141592653589793
#endif
#ifndef MAD_TO_STD
#define MAD_TO_STD 1.482602218505602
#endif

// Define object-oriented terminology
#define CLASS struct
#define PUBLIC extern
#define PRIVATE static

// Generic compile time check; should result in a compiler error if
// condition is false due to attempt to create array of negative size
#define COMPILE_TIME_CHECK(cond, mess) typedef char mess[(cond) ? 1 : -1]

// Check condition and exit if not met
void ensure(const int condition, const char *format, ...);
void check_null(const void *ptr);

// Print info and warning messages
void message(const char *format, ...);
void message_verb(const bool verbosity, const char *format, ...);
void status(const char *format, ...);
void warning(const char *format, ...);
void warning_verb(const bool verbosity, const char *format, ...);

// Display progress bar, time stamp and memory usage
void progress_bar(const char *text, const size_t progress, const size_t maximum);
void timestamp(const time_t start);
void memory_usage(void);

// String functions
char *trim_string(char *str);
void int_to_str(char *str, const size_t size, const long int value);

// Swap two values
void swap(double *val1, double *val2);

// Byte-order functions
bool is_little_endian(void);
void swap_byte_order(char *word, const size_t size);

#endif
