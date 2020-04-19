#ifndef MEMORY_H
#define MEMORY_H

// #define DEBUG

#ifdef DEBUG
#include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>

void *new_memory(size_t size, const char *purpose);
void free_memory(void *pointer, const char *purpose);
char *string_dup(const char *str);
int get_active_references();


#endif
