#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"

struct Function;
struct Call;
struct Thing;
struct Type;
struct Types;
struct Cons;
struct Applicative;
struct Function;

/* struct Function */
/* { */
/*     struct Call *(*f)(const void*, const void*, struct Function*); */
/*     const void *scope; */
/* }; */

/* struct Call */
/* { */
/*     struct Function *func; */
/*     const void *args; */
/* }; */

struct Thing;

struct Pacman
{
    struct Thing *root;
    struct Thing *all;
};

struct Type
{
    const char *name;
    int id;
    void (*destroy)(struct Thing*);
    void (*mark)(struct Thing*);
    void (*track)(struct Thing*);
};

struct Thing
{
    void *value;
    const struct Type *type;
    int marked;
    int tracked;
    struct Pacman *pacman;
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
};

struct Function
{
    void (*f)(struct Thing*, struct Eva*);
    struct Thing *env;
};


struct Types
{
    struct Type integer, string, symbol, number, bool, cons, nil, native, function;
};


extern const struct Thing DEFAULT_THING;
extern const struct Types TYPES;

struct Thing *new_thing();
void thing_mark(struct Thing *thing);
void thing_track(struct Thing *thing, struct Thing *other);
void simple_track(struct Thing *thing);
void simple_free(struct Thing *thing, const char *purpose);
void free_cons(struct Thing *thing);
void free_nil(struct Thing *thing);
void free_function(struct Thing *thing);
void simple_mark(struct Thing *thing);
struct Thing *new_integer(int value);
struct Thing *new_string_no_copy(char *value);
struct Thing *new_string(const char *value);
struct Thing *new_symbol_no_copy(char *value);
struct Thing *new_symbol(const char *value);
struct Thing *new_number(double value);
struct Thing *new_bool(int value);
struct Thing *new_cons(struct Thing *car, struct Thing *cdr);
struct Thing *new_nil();
struct Thing *new_native(void *value);
struct Thing *new_function(void (*f)(struct Thing*, struct Eva*), struct Thing *env);
int get_references();

void set_car_unsafe(struct Thing *thing, struct Thing *new_car);
void set_cdr_unsafe(struct Thing *thing, struct Thing *new_cdr);

struct Pacman *new_pacman(struct Thing *root);
void pacman_track(struct Pacman *pacman, struct Thing *thing);
void pacman_set_root(struct Pacman *pacman, struct Thing *new_root);
void pacman_mark(struct Pacman *pacman);
void pacman_unmark(struct Pacman *pacman);
void pacman_eat(struct Pacman *pacman);
void pacman_mark_and_eat(struct Pacman *pacman);
void pacman_destroy(struct Pacman *pacman);

#endif
