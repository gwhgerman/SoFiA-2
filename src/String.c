#include <stdlib.h>
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

PUBLIC void String_set(String *self, const char *string)
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
	
	return;
}



// Append to string

PUBLIC void String_append(String *self, const char *string)
{
	// Sanity checks
	check_null(self);
	if(string == NULL || *string == '\0') return;
	
	const size_t size_string = strlen(string);
	
	self->string = (char *)memory_realloc(self->string, self->size + size_string + 1, sizeof(char));
	strcpy(self->string + self->size, string);
	self->size += size_string;
	
	return;
}



// Clear string

PUBLIC void String_clear(String *self)
{
	// Sanity checks
	check_null(self);
	
	self->size = 0;
	self->string = (char *)memory_realloc(self->string, 1, sizeof(char));
	*(self->string) = '\0';
	
	return;
}



// Trim string

PUBLIC void String_trim(String *self)
{
	// Sanity checks
	if(self == NULL || self->size == 0) return;
	
	// Find first non-whitespace character
	char *start = self->string;
	while(is_whitespace(*start)) ++start;
	
	// All space?
	if(*start == '\0')
	{
		String_clear(self);
		return;
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
	
	return;
}
