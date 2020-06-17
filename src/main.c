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

struct Thing *add_slash_if_missing(const char *path)
{
    if('/' != path[strlen(path) -1])
    {
        struct StringBuilder *builder = new_string_builder(path);
        builder = string_builder_add_copy(builder, "/");
        char *with_slash = string_builder_to_string_and_destroy(builder);

        return new_string_no_copy(with_slash);
    }

    return new_string(path);
}

struct Thing *parse_init_file(const char *lisp_root, struct Pacman *pacman)
{
    struct StringBuilder *builder = new_string_builder(lisp_root);
    builder = string_builder_add_copy(builder, "pff.lisp");
    char *filename = string_builder_to_string_and_destroy(builder);
    
    struct Thing *parsed = parse_file(filename, pacman);

    free_memory(filename, "parse_init_file");

    return parsed;
}

int main(int argc, const char **argv)
{
    struct Pacman *pacman = new_pacman(new_nil());
    if(argc < 3)
    {
        puts("missing argument, usage:\n./a.out <lisp dir> <project dir>");
        return 1;
    }
    struct Thing *lisp_root = add_slash_if_missing(argv[1]);
    struct Thing *project_root = add_slash_if_missing(argv[2]);
    
    struct Thing *parsed = parse_init_file(lisp_root->value, pacman);
    if(parsed == NULL)
    {
        puts("could not read filename (init.lisp)");
        return 1;
    }

    struct Thing *nil = new_nil();
    struct Thing *env = build_std_env();
    struct Thing *thunk = new_function(eval, env);
    struct Thing *args = new_cons(new_cons(new_cons(new_cons(new_symbol("lambda"), new_cons(new_nil(), parsed)), nil), new_cons(lisp_root, new_cons(project_root, new_nil()))), new_nil());
    pacman_track(pacman, thunk);
    pacman_track(pacman, args);

    eval_loop(thunk, args, pacman);
    
    pacman_destroy(pacman);
    printf("active references: %d\n", get_active_references());
    
    return 0;
}
