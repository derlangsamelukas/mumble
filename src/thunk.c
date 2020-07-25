#include "types.h"

struct Thing *new_thunk(void (*f)(struct Array*, struct Eva*), struct Array *env, struct Environment *environment)
{
    struct Thing *thing = new_thing(environment);
    struct Thunk *thunk = new_memory(sizeof(struct Thunk), "new_thunk");
    thunk->f = f;
    thunk->env = env;
    thing->value = thunk;
    thing->type = &TYPES.thunk;

    return thing;
}

void free_thunk(struct Thing *thing)
{
    struct Thunk *thunk = thing->value;
    array_free(thunk->env);
    simple_free(thing, "thunk_free");
}

void thunk_mark(struct Thing *thing)
{
    struct Thunk *thunk = thing->value;
    simple_mark(thing);
    int i = 0;
    while(i < array_size(thunk->env))
    {
        struct Thing *child = array_get(thunk->env, i);
        if(!child->marked)
        {
            thing_mark(child);
        }
        i++;
    }
}
