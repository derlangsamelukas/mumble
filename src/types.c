#include "types.h"

const struct Thing DEFAULT_THING = {value: NULL, type: 0, marked: 0, tracked: 0, environment: NULL};

void thing_mark(struct Thing *thing)
{
    thing->type->mark(thing);
}

struct Thing *new_thing(struct Environment *environment)
{
    struct Thing *thing = memcpy(new_memory(sizeof(struct Thing), "new_thing"), &DEFAULT_THING, sizeof(struct Thing));
    thing->environment = environment;
    pacman_track(environment->pacman, thing);

    return thing;
}

void simple_free(struct Thing *thing, const char *purpose)
{
    char buffer[100];
    sprintf(buffer, "simple_free: free Thing->value (%s)", purpose);
    free_memory(thing->value, buffer);
    sprintf(buffer, "simple_free: free Thing (%s)", purpose);
    free_memory(thing, buffer);
}

void free_string(struct Thing *thing)
{
    simple_free(thing, "string");
}

void just_free(struct Thing *thing)
{
    simple_free(thing, "does not matter");
}

void free_cons(struct Thing *thing)
{
    simple_free(thing, "cons");
}

void free_function(struct Thing *thing)
{
    simple_free(thing, "function");
}

void free_nil(struct Thing *thing)
{
    free_memory(thing, "free_nil");
}

void free_native(struct Thing *thing)
{
    free_memory(thing, "free_native");
}


void simple_mark(struct Thing *thing)
{
    thing->marked = 1;
}

void cons_mark(struct Thing *thing)
{
    simple_mark(thing);
    struct Cons *cons = (struct Cons*)thing->value;
    if(!cons->car->marked)
    {
        thing_mark(cons->car);
    }
    if(!cons->cdr->marked)
    {
        thing_mark(cons->cdr);
    }
}

void function_mark(struct Thing *thing)
{
    simple_mark(thing);
    struct Function *fn = (struct Function*)thing->value;
    if(!fn->env->marked)
    {
        thing_mark(fn->env);
    }
}

const struct Types TYPES = {
    {"integer", 1, just_free, simple_mark},
    {"string", 2, free_string, simple_mark},
    {"symbol", 3, just_free, simple_mark},
    {"number", 4, just_free, simple_mark},
    {"bool", 5, just_free, simple_mark},
    {"cons", 6, free_cons, cons_mark},
    {"nil", 7, free_nil, simple_mark},
    {"native", 8, free_native, simple_mark},
    {"function", 9, free_function, function_mark},
    {"thunk", 10, free_thunk, thunk_mark}
};

struct Thing *new_integer(int value, struct Environment *environment)
{
    struct Thing *thing = new_thing(environment);
    int *integer = (int*)new_memory(sizeof(int), "new_integer");
    *integer = value;
    thing->value = (void*)(integer);
    thing->type = &TYPES.integer;

    return thing;
}

struct Thing *new_string_no_copy(char *value, struct Environment *environment)
{
    struct Thing *thing = new_thing(environment);
    thing->value = (void*)(value);
    thing->type = &TYPES.string;

    return thing;
}

struct Thing *new_string(const char *value, struct Environment *environment)
{
    const int length = strlen(value);
    char *string = (char*)new_memory(sizeof(char) * length +1, "new_string");
    memcpy(string, value, length);
    string[length] = '\0';

    return new_string_no_copy(string, environment);
}

struct Thing *new_symbol_no_copy(char *value, struct Environment *environment)
{
    struct Thing *thing = new_string_no_copy(value, environment);
    thing->type = &TYPES.symbol;

    return thing;
}

struct Thing *new_symbol(const char *value, struct Environment *environment)
{
    struct Thing *thing = new_string(value, environment);
    thing->type = &TYPES.symbol;

    return thing;
}

struct Thing *new_number(double value, struct Environment *environment)
{
    struct Thing *thing = new_thing(environment);
    double *number = (double*)new_memory(sizeof(double), "new_number");
    *number = value;
    thing->value = (void*)(number);
    thing->type = &TYPES.number;

    return thing;
}

struct Thing *new_bool(int value, struct Environment *environment)
{
    struct Thing *thing = new_thing(environment);
    int *bool = (int*)new_memory(sizeof(int), "new_bool");
    *bool = value != 0;
    thing->value = (void*)(bool);
    thing->type = &TYPES.bool;

    return thing;
}

struct Thing *new_cons(struct Thing *car, struct Thing *cdr, struct Environment *environment)
{
    struct Thing *thing = new_thing(environment);
    struct Cons *cons = (struct Cons*)new_memory(sizeof(struct Cons), "new_cons");
    cons->car = car;
    cons->cdr = cdr;
    thing->value = cons;
    thing->type = &TYPES.cons;

    return thing;
}

/* struct Thing NIL = { */
/*     NULL, */
/*     &TYPES.nil, */
/*     1, */
/*     1, */
/*     NULL */
/* }; */

struct Thing *new_nil(struct Environment *environment)
{
    // return &NIL;
    struct Thing *thing = new_thing(environment);
    thing->value = NULL;
    thing->type = &TYPES.nil;

    return thing;
}

struct Thing *new_native(void *value, struct Environment *environment)
{
    struct Thing *thing = new_thing(environment);
    thing->value = value;
    thing->type = &TYPES.native;

    return thing;
}

struct Thing *new_function(void (*f)(struct Thing*, struct Eva*), struct Thing *env, struct Environment *environment)
{
    struct Thing *thing = new_thing(environment);
    struct Function *thunk = (struct Function*)new_memory(sizeof(struct Function), "new_function");

    thunk->f = f;
    thunk->env = env;
    
    thing->value = thunk;
    thing->type = &TYPES.function;

    return thing;
}
