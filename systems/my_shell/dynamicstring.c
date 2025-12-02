
#include <stdio.h>
#include <stdlib.h>
#include "dynamicstring.h"

dynamicstring_t *DynamicString_Create(const char *input)
{

    // malloc space for the newly created struct
    dynamicstring_t *temp = malloc(sizeof(dynamicstring_t));

    // check if malloc failed
    if (temp == NULL)
    {
        return NULL;
    }

    // loop through until the null terminator to get the length
    size_t length = 0;
    for (size_t i = 0; input[i] != '\0'; i++)
    {
        length++;
    }

    // update struct values
    temp->length = length;
    temp->capacity = length + 1;

    // allocate space for the string
    temp->buf = (char *)malloc(sizeof(char) * temp->capacity);

    // check if malloc fails
    if (temp->buf == NULL)
    {
        free(temp);
        return NULL;
    }

    // loop through and copy the values in
    for (size_t i = 0; i < length; i++)
    {
        temp->buf[i] = input[i];
    }

    // make sure to terminate once the word is done
    temp->buf[length] = '\0';

    return temp;
}

// Free's the dynamic string and free's the underlying memory of the dynamicstring_t.
int DynamicString_Free(dynamicstring_t *str)
{
    // NULL check
    if (str == NULL)
    {
        return 0;
    }

    // if there is a buf, free it
    if (str->buf)
    {
        free(str->buf);
    }
    // finally free the str
    free(str);

    return 1;
}

// Append a single character to our dynamic string
int DynamicString_AppendChar(dynamicstring_t *str, char c)
{

    int result;

    // NULL check
    if (str == NULL)
    {
        result = 0;
        return result;
    }

    // If no room for new char + '\0', grow the buffer
    if (str->length + 1 >= str->capacity)
    {
        size_t newcap;
        // start with 16 in empty 
        if (str->capacity == 0)
        {
            newcap = 16; 
        }
        // otherwise double it 
        else
        {
            newcap = str->capacity * 2; 
        }
        char *newbuf = realloc(str->buf, newcap);

        // check allocation 
        if (newbuf == NULL)
        {
            return 0; 
        }
        str->buf = newbuf;
        str->capacity = newcap;
    }

    // append char and null-terminate
    str->buf[str->length] = c;
    str->length++;
    str->buf[str->length] = '\0';


    // Indicating success
    result = 1;

    return result;
}


// Allocates a new 'string'
dynamicstring_t *DynamicString_NewStringFromSlice(dynamicstring_t *str, int start, int end)
{

    // NULL check and check bounds for start and end indices
    if (str == NULL || start < 0 || end < 0 || start >= (int)str->length || end > (int)str->length || end <= start)
    {
        return NULL;
    }

    // get the size of the new string
    size_t new_length = end - start;

    // malloc space for new string
    char *new_string = (char *)malloc(sizeof(char) * (new_length + 1));

    // check if malloc failed
    if (new_string == NULL)
    {
        return NULL;
    }

    // loop through and copy of the chars
    for (size_t i = 0; i < new_length; i++)
    {
        new_string[i] = str->buf[start + i];
    }
    new_string[new_length] = '\0';

    // use the create function to make a new string
    dynamicstring_t *string = DynamicString_Create(new_string);

    free(new_string);

    return string;
}

// Split a dynamic string into multiple dynamic strings that are returned in the output parameter.
int DynamicString_Split(dynamicstring_t *input, const char *delimeters, dynamicstring_t ***array, int *size)
{

    int result;

    // NULL check
    if ((delimeters == NULL) || (input == NULL) || array == NULL || size == NULL)
    {
        result = 0;
        return result;
    }

    // initalize variables to count tokens and keep track of where we start
    int counter = 0;
    int start = 0;

    // use nested loops to go through with our first pass
    // checks each character against each delimiter
    for (size_t i = 0; i < input->length; i++)
    {
        for (size_t j = 0; delimeters[j] != '\0'; j++)
        {
            if (input->buf[i] == delimeters[j])
            {
                // makes sure we dont create empty tokens with consecutive delimiters
                if (i - start > 0)
                {
                    counter++;
                }
                // move our start index
                start = i + 1;
                break;
            }
        }
    }

    // account for the last token
    if (start < input->length)
    {
        counter++;
    }

    // handle case of no delimiters
    if (counter == 0)
    {
        *array = NULL;
        *size = 0;
        return 1;
    }

    // malloc space for the array
    *array = (dynamicstring_t **)malloc(sizeof(dynamicstring_t *) * counter);

    // check if malloc failed
    if (*array == NULL)
    {
        result = 0;
        return result;
    }

    // update our output parameter to the counter
    *size = counter;

    int index = 0;
    start = 0;

    // use nested loop for our second pass through
    // this pass makes sense to create the new strings based off when delimiter is found
    for (size_t i = 0; i < input->length; i++)
    {
        for (size_t j = 0; delimeters[j] != '\0'; j++)
        {
            // check if we found a delimiter
            if (input->buf[i] == delimeters[j])
            {
                // filters out zero length substrings
                if (i - start > 0)
                {
                    // use slice function with the start and i indicies
                    dynamicstring_t *tok = DynamicString_NewStringFromSlice(input, start, i);

                    // NULL check
                    if (tok == NULL)
                    {
                        for (int k = 0; k < index; k++)
                            DynamicString_Free((*array)[k]);
                        free(*array);
                        *array = NULL;
                        *size = 0;
                        return 0;
                    }
                    // update the array with each token
                    (*array)[index++] = tok;
                }
                // move our start index
                start = i + 1;
                break;
            }
        }
    }

    // account for the last token
    if (start < input->length)
    {
        // use slice function with the start and length
        dynamicstring_t *tok = DynamicString_NewStringFromSlice(input, start, input->length);

        // NULL check
        if (tok == NULL)
        {
            for (int k = 0; k < index; k++)
                DynamicString_Free((*array)[k]);
            free(*array);
            *array = NULL;
            *size = 0;
            return 0;
        }
        // update the array with each token
        (*array)[index++] = tok;
    }
    result = 1;

    return result;
}
