#include "./parser.h"
#include <string.h>
#include <stdio.h>


static void append(dynamicstring_t *dst, const char *op)
{
    DynamicString_AppendChar(dst, ' ');

    for (int i = 0; op[i] != '\0'; i++)
    {
        DynamicString_AppendChar(dst, op[i]);
    }
    DynamicString_AppendChar(dst, ' ');
}

dynamicstring_t *pre_process(dynamicstring_t *input)
{
    dynamicstring_t *new = DynamicString_Create("");

    for (int i = 0; i < input->length; i++)
    {
        char c = input->buf[i];

        // check multi-char ops
        if ((c == '|' && input->buf[i + 1] == '|') ||
            (c == '&' && input->buf[i + 1] == '&'))
        {
            char op[3] = {c, input->buf[i + 1], '\0'};
            append(new, op);
            i++;
        }
        // single-char ops
        else if (c == '|' || c == '<' || c == '>' || c == ';')
        {
            char op[2] = {c, '\0'};
            append(new, op);
        }
        // normal char
        else
        {
            DynamicString_AppendChar(new, c);
        }
    }

    DynamicString_Free(input);
    return new;
}

dynamicstring_t **parse(dynamicstring_t ***array, dynamicstring_t *string, int *count)
{

    int result = DynamicString_Split(string, " ", array, count);
    if (result == 0)
    {
        return NULL;
    }

    return *array;
}


dynamicstring_t **run_parser(char **argv, int *count)
{
    dynamicstring_t *string = DynamicString_Create(argv[1]);
    dynamicstring_t *input = pre_process(string);

    dynamicstring_t **array = NULL;
    parse(&array, input, count);

    DynamicString_Free(input);

    return array;
}