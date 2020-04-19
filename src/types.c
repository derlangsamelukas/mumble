#include "types.h"

void dummy_decrement(struct Thing *_)
{
    
}

const struct Thing DEFAULT_THING = {value: NULL, type: 0, marked: 0, tracked: 0, pacman: NULL};

void thing_mark(struct Thing *thing)
{
    thing->type->mark(thing);
}

void thing_track(struct Thing *thing, struct Thing *other)
{
    if(thing->tracked && !other->tracked)
    {
        pacman_track(thing->pacman, other);
    }
}

struct Thing *new_thing()
{
    return (struct Thing*)memcpy(new_memory(sizeof(struct Thing), "new_thing"), &DEFAULT_THING, sizeof(struct Thing));
}

void simple_free(struct Thing *thing, const char *purpose)
{
    char buffer[100];
    sprintf(buffer, "simple_free: free Thing->value (%s)", purpose);
    free_memory(thing->value, buffer);
    sprintf(buffer, "simple_free: free Thing (%s)", purpose);
    free_memory(thing, buffer);
}

void simple_track(struct Thing *thing)
{
    // ignore
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
        thing_track(thing, cons->car);
        thing_mark(cons->car);
    }
    if(!cons->cdr->marked)
    {
        thing_track(thing, cons->cdr);
        thing_mark(cons->cdr);
    }
}

void cons_track(struct Thing *thing)
{
    struct Cons *cons = (struct Cons*)thing->value;
    if(!cons->car->tracked)
    {
        thing_track(thing, cons->car);
    }
    if(!cons->cdr->tracked)
    {
        thing_track(thing, cons->cdr);
    }
}

void function_mark(struct Thing *thing)
{
    simple_mark(thing);
    struct Function *fn = (struct Function*)thing->value;
    if(!fn->env->marked)
    {
        thing_track(thing, fn->env);
        thing_mark(fn->env);
    }
}

void function_track(struct Thing *thing)
{
    struct Function *fn = (struct Function*)thing->value;
    if(!fn->env->tracked)
    {
        thing_track(thing, fn->env);
    }
}

const struct Types TYPES = {
    {"integer", 1, just_free, simple_mark, simple_track},
    {"string", 2, free_string, simple_mark, simple_track},
    {"symbol", 3, just_free, simple_mark, simple_track},
    {"number", 4, just_free, simple_mark, simple_track},
    {"bool", 5, just_free, simple_mark, simple_track},
    {"cons", 6, free_cons, cons_mark, cons_track},
    {"nil", 7, free_nil, simple_mark, simple_track},
    {"native", 8, free_nil, simple_mark, simple_track},
    {"function", 9, free_function, function_mark, function_track}
};

struct Thing *new_integer(int value)
{
    struct Thing *thing = new_thing();
    int *integer = (int*)new_memory(sizeof(int), "new_integer");
    *integer = value;
    thing->value = (void*)(integer);
    thing->type = &TYPES.integer;

    return thing;
}

struct Thing *new_string_no_copy(char *value)
{
    struct Thing *thing = new_thing();
    thing->value = (void*)(value);
    thing->type = &TYPES.string;

    return thing;
}

struct Thing *new_string(const char *value)
{
    const int length = strlen(value);
    char *string = (char*)new_memory(sizeof(char) * length +1, "new_string");
    memcpy(string, value, length);
    string[length] = '\0';

    return new_string_no_copy(string);
}

struct Thing *new_symbol_no_copy(char *value)
{
    struct Thing *thing = new_string_no_copy(value);
    thing->type = &TYPES.symbol;

    return thing;
}

struct Thing *new_symbol(const char *value)
{
    struct Thing *thing = new_string(value);
    thing->type = &TYPES.symbol;

    return thing;
}

struct Thing *new_number(double value)
{
    struct Thing *thing = new_thing();
    double *number = (double*)new_memory(sizeof(double), "new_number");
    *number = value;
    thing->value = (void*)(number);
    thing->type = &TYPES.number;

    return thing;
}

struct Thing *new_bool(int value)
{
    struct Thing *thing = new_thing();
    int *bool = (int*)new_memory(sizeof(int), "new_bool");
    *bool = value != 0;
    thing->value = (void*)(bool);
    thing->type = &TYPES.bool;
}

struct Thing *new_cons(struct Thing *car, struct Thing *cdr)
{
    struct Thing *thing = new_thing();
    struct Cons *cons = (struct Cons*)new_memory(sizeof(struct Cons), "new_cons");
    cons->car = car;
    cons->cdr = cdr;
    thing->value = cons;
    thing->type = &TYPES.cons;

    thing_track(thing, car);
    thing_track(thing, cdr);

    return thing;
}

void set_car_unsafe(struct Thing *thing, struct Thing *new_car)
{
    struct Cons *cons = thing->value;
    thing_track(thing, new_car);
    cons->car = new_car;
}

void set_cdr_unsafe(struct Thing *thing, struct Thing *new_cdr)
{
    struct Cons *cons = thing->value;
    thing_track(thing, new_cdr);
    cons->cdr = new_cdr;
}


struct Thing *new_nil()
{
    struct Thing *thing = new_thing();
    thing->value = NULL;
    thing->type = &TYPES.nil;

    return thing;
}

struct Thing *new_native(void *value)
{
    struct Thing *thing = new_thing();
    thing->value = value;
    thing->type = &TYPES.native;

    return thing;
}

struct Thing *new_function(void (*f)(struct Thing*, struct Eva*), struct Thing *env)
{
    struct Thing *thing = new_thing();
    struct Function *thunk = (struct Function*)new_memory(sizeof(struct Function), "new_function");

    thunk->f = f;
    thunk->env = env;
    
    thing->value = thunk;
    thing->type = &TYPES.function;

    thing_track(thing, env);

    return thing;
}
