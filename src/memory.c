#include "memory.h"

static int active_references = 0;

void *new_memory(size_t size, const char *purpose)
{
    active_references++;
#ifdef DEBUG
    puts(purpose);
#endif
    return malloc(size);
}

void free_memory(void *pointer, const char *purpose)
{
    active_references--;
#ifdef DEBUG
    printf("free: %s\n", purpose);
#endif
    free(pointer);
}

char *string_dup(const char *str)
{
    int length = strlen(str);
    char *string = new_memory(sizeof(char) * (length +1), "string_dup");
    memcpy(string, str, length);
    string[length] = '\0';

    return string;
}

int get_active_references()
{
    return active_references;
}
