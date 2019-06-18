#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "String.h"



// ----------------------------------------------------------------- //
// Declaration of private properties and methods of class String     //
// ----------------------------------------------------------------- //

CLASS String
{
	size_t size;   // String size WITHOUT terminating null character!
	char *string;
};



// Constructor

PUBLIC String *String_new(const char *string)
{
	// Sanity check
	if(string == NULL) return NULL;
	
	// Allocate memory for new String object
	String *self = (String *)memory(MALLOC, 1, sizeof(String));
	
	// Initialise properties
	self->size = 0;
	self->string = NULL;
	
	// Call method for setting string value
	String_set(self, string);
	
	// Return new String object
	return self;
}



// Copy constructor
// Note that this copy constructor has only been included for semantic
// reasons, as its sole job is to call the standard constructor.

PUBLIC String *String_copy(const String *string)
{
	return String_new(string == NULL ? NULL : string->string);
}



// Destructor

PUBLIC void String_delete(String *self)
{
	if(self != NULL) free(self->string);
	free(self);
	return;
}



// Return size of string (without null)

PUBLIC size_t String_size(const String *self)
{
	return self == NULL ? 0 : self->size;
}



// Retrieve pointer to string

PUBLIC const char *String_get(const String *self)
{
	return self == NULL ? NULL : self->string;
}



// Extract single character

PUBLIC char String_at(const String *self, const size_t index)
{
	// Sanity checks
	check_null(self);
	ensure(index < self->size, "String index out of range.");
	
	return *(self->string + index);
}



// Compare two strings

PUBLIC bool String_compare(const String *self, const char *string)
{
	// Sanity checks
	check_null(self);
	check_null(string);
	
	return strcmp(self->string, string) == 0;
}



// Set string

PUBLIC String *String_set(String *self, const char *string)
{
	// Sanity checks
	check_null(self);
	
	// Empty string?
	if(string == NULL || *string == '\0')
	{
		String_clear(self);
	}
	else
	{
		self->size = strlen(string);
		self->string = (char *)memory_realloc(self->string, self->size + 1, sizeof(char));
		strcpy(self->string, string);
	}
	
	return self;
}



// Set string from integer

PUBLIC String *String_set_int(String *self, char *format, const long int value)
{
	// Sanity checks
	check_null(self);
	if(format == NULL) format = "%ld";
	
	// Create buffer
	char *buffer = (char *)memory(MALLOC, 32, sizeof(char));
	
	// Write number into buffer
	const int flag = snprintf(buffer, 32, format, value);
	if(flag >= 32) warning("Buffer overflow in int-to-string conversion.");
	if(flag <   0) warning("Encoding error in int-to-string conversion.");
	
	// Copy buffer into string
	String_set(self, buffer);
	
	// Clean up
	free(buffer);
	
	return self;
}



// Set string until/from delimiting character

PUBLIC String *String_set_delim(String *self, const char *string, const char delimiter, const bool first, const bool until)
{
	// Sanity checks
	check_null(self);
	check_null(string);
	
	// Some settings
	const size_t size_old = strlen(string);
	size_t size_new = 0;
	char *pos_delim = NULL;
	
	// Find delimiter
	if(first) pos_delim = strchr(string, delimiter);
	else pos_delim = strrchr(string, delimiter);
	
	// Not found?
	if(pos_delim == NULL) return String_set(self, string);
	
	// Determine size of new sub-string
	if(until) size_new = pos_delim - string;
	else size_new = string + size_old - pos_delim - 1;
	
	// If delimiter at edge, clear string
	if(size_new == 0) return String_clear(self);
	
	// Otherwise copy sub-string
	char *tmp = (char *)memory(MALLOC, size_new + 1, sizeof(char));
	if(until) strncpy(tmp, string, size_new);
	else strncpy(tmp, pos_delim + 1, size_new);
	*(tmp + size_new) = '\0';
	
	String_set(self, tmp);
	free(tmp);
	
	return self;
}



// Append to string

PUBLIC String *String_append(String *self, const char *string)
{
	// Sanity checks
	check_null(self);
	if(string == NULL || *string == '\0') return self;
	
	const size_t size = strlen(string);
	
	self->string = (char *)memory_realloc(self->string, self->size + size + 1, sizeof(char));
	strcpy(self->string + self->size, string);
	self->size += size;
	
	return self;
}



// Append to string from integer

PUBLIC String *String_append_int(String *self, char *format, const long int value)
{
	// Sanity checks
	check_null(self);
	if(format == NULL) format = "%ld";
	
	// Create buffer
	char *buffer = (char *)memory(MALLOC, 32, sizeof(char));
	
	// Write number into buffer
	const int flag = snprintf(buffer, 32, format, value);
	if(flag >= 32) warning("Buffer overflow in int-to-string conversion.");
	if(flag <   0) warning("Encoding error in int-to-string conversion.");
	
	// Append buffer to string
	String_append(self, buffer);
	
	// Clean up
	free(buffer);
	
	return self;
}



// Append to string from float

PUBLIC String *String_append_flt(String *self, char *format, const double value)
{
	// Sanity checks
	check_null(self);
	if(format == NULL) format = "%.5f";
	
	// Create buffer
	char *buffer = (char *)memory(MALLOC, 32, sizeof(char));
	
	// Write number into buffer
	const int flag = snprintf(buffer, 32, format, value);
	if(flag >= 32) warning("Buffer overflow in float-to-string conversion.");
	if(flag <   0) warning("Encoding error in float-to-string conversion.");
	
	// Append buffer to string
	String_append(self, buffer);
	
	// Clean up
	free(buffer);
	
	return self;
}



// Prepend to string

PUBLIC String *String_prepend(String *self, const char *string)
{
	// Sanity checks
	check_null(self);
	if(string == NULL || *string == '\0') return self;
	
	const size_t size = strlen(string);
	
	self->string = (char *)memory_realloc(self->string, self->size + size + 1, sizeof(char));
	memmove(self->string + size, self->string, self->size + 1);
	memcpy(self->string, string, size);
	self->size += size;
	
	return self;
}



// Clear string

PUBLIC String *String_clear(String *self)
{
	// Sanity checks
	check_null(self);
	
	self->size = 0;
	self->string = (char *)memory_realloc(self->string, 1, sizeof(char));
	*(self->string) = '\0';
	
	return self;
}



// Trim string

PUBLIC String *String_trim(String *self)
{
	// Sanity checks
	if(self == NULL || self->size == 0) return self;
	
	// Find first non-whitespace character
	char *start = self->string;
	while(is_whitespace(*start)) ++start;
	
	// All space?
	if(*start == '\0')
	{
		String_clear(self);
		return self;
	}
	
	// Find last non-whitespace character
	char *end = self->string + self->size - 1;
	while(end > start && is_whitespace(*end)) --end;
	
	// Shift sub-string to beginning of string buffer
	self->size = end - start + 1;
	memmove(self->string, start, self->size);
	*(self->string + self->size) = '\0';
	
	// Adjust memory allocation
	self->string = (char *)memory_realloc(self->string, self->size + 1, sizeof(char));
	
	return self;
}
