#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL.h"
#include "SDL_image.h"
#define A(x) (void*)x

#include "types.h"
#include "helper.h"
#include "eval.h"
#include "std.h"

int run();
char* read_file (const char* filename);

void ident(struct Thing *env, struct Eva *eva)
{
    update_args(new_cons(new_string("hejooo"), new_nil()), eva);
}

struct Thing *parse_file(const char *filename, struct Pacman *pacman)
{
    char *content = read_file(filename);
    if(content == NULL)
    {
        return NULL;
    }
    struct Thing *parsed = parse(content, pacman);
    free_memory(content, "parse_file: free read_file");

    return parsed;
}

void test()
{
    struct Thing *root = new_cons(new_nil(), new_nil());
    struct Pacman *pacman = new_pacman(root);
    set_car_unsafe(root, new_cons(new_cons(new_nil(), new_symbol("hej")), new_string("haha")));
    pacman_set_root(pacman, new_cons(new_nil(), new_nil()));
    pacman_mark_and_eat(pacman);
    set_car_unsafe(pacman->root, new_nil());    
    pacman_mark_and_eat(pacman);
    
    pacman_destroy(pacman);
    printf("active references: %d\n", get_active_references());
}

int main(int argc, const char **argv)
{
    struct Pacman *pacman = new_pacman(new_nil());
    if(argc < 2)
    {
        puts("missing argument, usage:\n./a.out <filename>");
        return 1;
    }
    struct Thing *parsed = parse_file(argv[1], pacman);
    if(parsed == NULL)
    {
        puts("could not read filename");
        return 1;
    }

    struct Thing *nil = new_nil();
    struct Thing *env = build_std_env();
    struct Thing *thunk = new_function(eval, env);
    struct Thing *args = new_cons(new_cons(new_cons(new_symbol("lambda"), new_cons(new_nil(), parsed)), nil), nil);
    pacman_track(pacman, thunk);
    pacman_track(pacman, args);

    eval_loop(thunk, args, pacman);
    
    pacman_destroy(pacman);
    printf("active references: %d\n", get_active_references());



    
    /* struct Thing *nil = new_nil(); */
    /* struct Thing *env = build_std_env(); */
    /* struct Thing *execute_env = new_cons(dope(new_cons(nil, parsed)), env); */
    /* struct Thing *thunk = new_function(execute_function, execute_env); */

    /* eval_loop(thunk, nil); */

    // test();
    
    // printf("active references: %d\n", get_active_references());


    /* decrement_reference_counter(print_symbol); */
    /* decrement_reference_counter(print_thunk); */
    /* decrement_reference_counter(ident_thunk); */
    /* decrement_reference_counter(ident_symbol); */
    /* decrement_reference_counter(ident_pair); */
    /* decrement_reference_counter(print_pair); */
    
    return 0;
}
