#ifndef DYNAMICSTRING_H
#define DYNAMICSTRING_H

#include <stdlib.h> 
#include <stddef.h> 
				

typedef struct dynamicstring{
	char* buf;  // short for 'buffer'
				// Holds the individual characters of the string.
	size_t capacity; // How 'big' the buffer is, including the '\0' character
	size_t length;   // The length of the string, NOT including the '\0' character

}dynamicstring_t;

// Input is a legal NULL terminated C style string
dynamicstring_t* DynamicString_Create(const char* input);

// Free's the dynamic string and free's the underlying memory of the dynamicstring_t.
// The underlying memory is freed first in 'buf'
// prior to freeing a heap allocated dynamicstring.
// Returns '1' on success, or 0 in failure.
int DynamicString_Free(dynamicstring_t* str);

// Append a single 'char' to the dynamicstring_t.
// This function ensures that the final character is also the '\0' and the 'buf' is thus a null terminated string.
// returns '1' on success
// returns '0' on failure
//   - An example failure is an operation on a NULL 'str' which would return 0.
//   - An example failure would be if malloc or realloc cannot allocate memory
int DynamicString_AppendChar(dynamicstring_t* str, char c);

// Allocates a new 'dynamicstring' based from a substring of an existing string.
// The 'slice' or portion of the string is a new heap allocated dynamicstring.
dynamicstring_t* DynamicString_NewStringFromSlice(dynamicstring_t* str, int start, int end);

// Split a dynamicstring based on delimeters. 
// Returns '0' if there is an error, and 1 on success
int DynamicString_Split(dynamicstring_t* input, const char* delimeters, dynamicstring_t*** array, int* size);

#endif
