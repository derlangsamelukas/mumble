#include "types.h"

struct Pacman *new_pacman()
{
    static const int INIT_ARRAY_SIZE = 200;
    struct Pacman *pacman = (struct Pacman*)new_memory(sizeof(struct Pacman*), "Pacman");
    pacman->all = new_array(INIT_ARRAY_SIZE);
    pacman->root = NULL; // risky

    return pacman;
}

char *string_of_thing(struct Thing*);

void pacman_track(struct Pacman *pacman, struct Thing *thing)
{
    if(thing->tracked == 1)
    {
        return;
    }
    if(array_full(pacman->all))
    {
        struct Array *old = pacman->all;
        pacman->all = array_from(old, 2 * array_size(old));
        array_free(old);
    }
    array_push(pacman->all, thing);
    thing->tracked = 1;
    // thing->pacman = pacman;
    // thing->type->track(thing);
}

void pacman_set_root(struct Pacman *pacman, struct Thing *new_root)
{
    pacman->root = new_root;
    // pacman_track(pacman, new_root);
}

void pacman_mark(struct Pacman *pacman)
{
    thing_mark(pacman->root);
}

void pacman_unmark(struct Pacman *pacman)
{
    int i = 0;
    while(i < array_length(pacman->all))
    {
        struct Thing *current = array_get(pacman->all, i);
        current->marked = 0;

        i++;
    }
}


char *string_of_thing(struct Thing*);

void pacman_eat(struct Pacman *pacman)
{
    int freed_things = 0; // for debug purpose
    int i = 0;
    int length = array_length(pacman->all);
    while(i < length)
    {
        struct Thing *current = array_get(pacman->all, i);
        if(!current->marked)
        {
            printf("%p: %s\n", current, current->type->name);
            current->tracked = -1;
            current->type->destroy(current);
            array_remove(pacman->all, i);
            freed_things++;
            length = array_length(pacman->all);
        }
        else
        {
            i++;
        }
    }
}

void pacman_mark_and_eat(struct Pacman *pacman)
{
    pacman_mark(pacman);
    pacman_eat(pacman);
}

void pacman_destroy(struct Pacman *pacman)
{
    int i = 0;
    while(i < array_length(pacman->all))
    {
        struct Thing *current = array_get(pacman->all, i);
        current->type->destroy(current);
        
        i++;
    }
    
    array_free(pacman->all);
    free_memory(pacman, "pacman_destroy");
}
