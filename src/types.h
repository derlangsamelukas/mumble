#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"

struct Function;
struct Thing;
struct Type;
struct Types;
struct Cons;
struct Applicative;
struct Function;
struct Array;
struct Environment;

struct Pacman
{
    struct Thing *root;
    struct Array *all;
};

struct Type
{
    const char *name;
    int id;
    void (*destroy)(struct Thing*);
    void (*mark)(struct Thing*);
};

struct Thing
{
    void *value;
    const struct Type *type;
    int marked;
    int tracked;
    struct Environment *environment;;
};

struct Cons
{
    struct Thing *car;
    struct Thing *cdr;
};

struct Eva
{
    struct Thing *next;
    struct Thing *args;
    struct Thing *unwind;
    struct Environment *environment;
};

struct Function
{
    void (*f)(struct Thing*, struct Eva*);
    struct Thing *env;
};

struct Array
{
    struct Thing **buffer;
    int size; // real allocated memory of buffer may contain NULL or garbage.
    int length; // length of data that is actually filled with pointers
};

struct Thunk // same as function, but with better performance
{
    void (*f)(struct Array*, struct Eva*);
    struct Array *env;
};

struct Types
{
    struct Type integer, string, symbol, number, bool, cons, nil, native, function, thunk;
};

struct Environment
{
    struct Pacman *pacman;
};

extern const struct Thing DEFAULT_THING;
extern const struct Types TYPES;

struct Thing *new_thing(struct Environment*);
void thing_mark(struct Thing *thing);
void simple_free(struct Thing *thing, const char *purpose);
void free_cons(struct Thing *thing);
void free_nil(struct Thing *thing);
void free_function(struct Thing *thing);
void simple_mark(struct Thing *thing);
struct Thing *new_integer(int value, struct Environment*);
struct Thing *new_string_no_copy(char *value, struct Environment*);
struct Thing *new_string(const char *value, struct Environment*);
struct Thing *new_symbol_no_copy(char *value, struct Environment*);
struct Thing *new_symbol(const char *value, struct Environment*);
struct Thing *new_number(double value, struct Environment*);
struct Thing *new_bool(int value, struct Environment*);
struct Thing *new_cons(struct Thing *car, struct Thing *cdr, struct Environment*);
struct Thing *new_nil(struct Environment*);
struct Thing *new_native(void *value, struct Environment*);
struct Thing *new_function(void (*f)(struct Thing*, struct Eva*), struct Thing *env, struct Environment*);
int get_references();

struct Pacman *new_pacman();
void pacman_track(struct Pacman *pacman, struct Thing *thing);
void pacman_set_root(struct Pacman *pacman, struct Thing *new_root);
void pacman_mark(struct Pacman *pacman);
void pacman_unmark(struct Pacman *pacman);
void pacman_eat(struct Pacman *pacman);
void pacman_mark_and_eat(struct Pacman *pacman);
void pacman_destroy(struct Pacman *pacman);

struct Array *new_array(int size);
struct Array *array_from(struct Array *other, int size);
struct Thing *array_get(struct Array *array, int index);
void array_push(struct Array *array, struct Thing *value);
void array_remove(struct Array *array, int index);
void array_free(struct Array *array);
int array_size(struct Array *array);
int array_length(struct Array *array);
int array_full(struct Array *array);

struct Thing *new_thunk(void (*f)(struct Array*, struct Eva*), struct Array *env, struct Environment *environment);
void free_thunk(struct Thing *thing);
void thunk_mark(struct Thing *thing);

#endif
