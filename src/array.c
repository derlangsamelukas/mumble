#include "types.h"

struct Array *new_array_with_garbage(int size)
{
    struct Array *array = new_memory(sizeof(struct Array), "new_array");

    array->buffer = new_memory(sizeof(struct Thing*) * size, "new_array->buffer");
    array->length = 0;
    array->size   = size;

    return array;
}

struct Array *new_array(int size)
{
    struct Array *array = new_array_with_garbage(size);

    int i = 0;
    while(i < size)
    {
        array->buffer[i] = NULL;
        i++;
    }

    return array;
}

void array_check_for_index_out_of_bounds(struct Array *array, int (*get_max)(struct Array*), int index, const char *from)
{
    if(index >= 0 && index < get_max(array))
    {
        return; // is ok
    }
    printf("index out of bounds: index: %d, max is: %d, from: %s\n", index, get_max(array), from);
    exit(1);
}

struct Array *array_from(struct Array *other, int size)
{
    if(array_length(other) > size)
    {
        printf("cannot create array from another array with lesser length, other length: %d, new size: %d\n", array_length(other), size);
        exit(1);
    }

    struct Array *array = new_array_with_garbage(size);
    int i = 0;
    while(i < array_length(other))
    {
        array->buffer[i] = array_get(other, i);
        i++;
    }
    while(i < size)
    {
        array->buffer[i] = NULL;
        i++;
    }
    array->length = array_length(other);

    return array;
}

struct Thing *array_get(struct Array *array, int index)
{
    array_check_for_index_out_of_bounds(array, array_length, index, "array_get");
    
    return array->buffer[index];
}

void array_push(struct Array *array, struct Thing *value)
{
    array_check_for_index_out_of_bounds(array, array_size, array_length(array), "array_set");
    array->buffer[array_length(array)] = value;
    array->length++;
}

void array_remove(struct Array *array, int index)
{
    array_check_for_index_out_of_bounds(array, array_length, index, "array_remove");
    array->buffer[index] = array->buffer[array_length(array) -1];
    array->buffer[array_length(array) -1] = NULL;
    array->length--;
}

void array_free(struct Array *array)
{
    free_memory(array->buffer, "array_free->buffer");
    free_memory(array, "array_free");
}

int array_size(struct Array *array)
{
    return array->size;
}

int array_length(struct Array *array)
{
    return array->length;
}

int array_full(struct Array *array)
{
    return array_length(array) >= array_size(array);
}
