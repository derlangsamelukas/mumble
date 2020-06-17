#include "SDL.h"
#include "helper.h"
#include <stdio.h>

char* read_file (const char* filename)
{
    FILE *file = fopen(filename, "r");
    if(ptr_equals(file, NULL))
    {
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* buffer = new_memory(multiply(sizeof(char), length +1), "read_file");
    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);

    return buffer;
}

