#include "types.h"

struct Pacman *new_pacman(struct Thing *root)
{
    struct Pacman *pacman = (struct Pacman*)new_memory(sizeof(struct Pacman*), "Pacman");
    pacman->all = new_nil();
    pacman_set_root(pacman, root);

    return pacman;
}
char *string_of_thing(struct Thing*);
void pacman_track(struct Pacman *pacman, struct Thing *thing)
{
    if(thing->tracked == 1)
    {
        return;
    }
    pacman->all = new_cons(thing, pacman->all);
    thing->tracked = 1;
    thing->pacman = pacman;
    thing->type->track(thing);
}

void pacman_set_root(struct Pacman *pacman, struct Thing *new_root)
{
    pacman->root = new_root;
    pacman_track(pacman, new_root);
    /* pacman_mark(pacman); */
    /* pacman_unmark(pacman); */
}

void pacman_mark(struct Pacman *pacman)
{
    thing_mark(pacman->root);
}

void pacman_unmark(struct Pacman *pacman)
{
    struct Thing *next = pacman->all;
    while(next->type == &TYPES.cons)
    {
        struct Cons *next_cons = next->value;
        next_cons->car->marked = 0;
        next = next_cons->cdr;
    }
}


char *string_of_thing(struct Thing*);

void pacman_eat(struct Pacman *pacman)
{
    struct Thing *next = pacman->all;
    struct Thing *dummy_nil = new_nil();
    struct Thing *dummy_cons = new_cons(dummy_nil, next);
    struct Thing *prev = dummy_cons;
    int freed_things = 0;
    while(next->type == &TYPES.cons)
    {
        struct Cons *next_cons = (struct Cons*)next->value;
        if(!next_cons->car->marked)
        {
            /* if(next_cons->car->reference_counter > 0 && next_cons->car->type->id != 23) */
            /* { */
            /*     printf(">%d %s\n", next_cons->car->reference_counter, string_of_thing(next_cons->car)); */
            /* } */
            next_cons->car->type->destroy(next_cons->car);
            struct Cons *prev_cons = (struct Cons*)prev->value;
            struct Thing *destroy_me = next;
            prev_cons->cdr = next_cons->cdr;
            next = next_cons->cdr;
            destroy_me->type->destroy(destroy_me);
            freed_things++;
        }
        else
        {
            next_cons->car->marked = 0;
            prev = next;
            next = next_cons->cdr;
        }
        
    }
    // printf("freed %d things\n", freed_things);

    struct Cons *prev_cons = (struct Cons*)dummy_cons->value;
    pacman->all = prev_cons->cdr;

    dummy_nil->type->destroy(dummy_nil);
    dummy_cons->type->destroy(dummy_cons);
}

void pacman_mark_and_eat(struct Pacman *pacman)
{
    pacman_mark(pacman);
    pacman_eat(pacman);
}

void pacman_destroy(struct Pacman *pacman)
{
    struct Thing *next = pacman->all;
    while(next->type == &TYPES.cons)
    {
        struct Cons *next_cons = (struct Cons*)next->value;
        struct Thing *old = next;
        next_cons->car->type->destroy(next_cons->car);
        next = next_cons->cdr;
        old->type->destroy(old);
    }

    next->type->destroy(next);
    free_memory(pacman, "pacman_destroy");
}
