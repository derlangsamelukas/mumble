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

struct Thing *parse_file(const char *filename, struct Environment *environment)
{
    char *content = read_file(filename);
    if(content == NULL)
    {
        return NULL;
    }
    struct Thing *parsed = parse(content, environment);
    free_memory(content, "parse_file: free read_file");

    return parsed;
}

struct Thing *add_slash_if_missing(const char *path, struct Environment* environment)
{
    if('/' != path[strlen(path) -1])
    {
        struct StringBuilder *builder = new_string_builder(path);
        builder = string_builder_add_copy(builder, "/");
        char *with_slash = string_builder_to_string_and_destroy(builder);

        return new_string_no_copy(with_slash, environment);
    }

    return new_string(path, environment);
}

struct Thing *parse_init_file(const char *lisp_root, struct Environment *environment)
{
    struct StringBuilder *builder = new_string_builder(lisp_root);
    builder = string_builder_add_copy(builder, "pff.lisp");
    char *filename = string_builder_to_string_and_destroy(builder);
    
    struct Thing *parsed = parse_file(filename, environment);

    free_memory(filename, "parse_init_file");

    return parsed;
}

int main(int argc, const char **argv)
{
    struct Environment environment = {new_pacman()};
    pacman_set_root(environment.pacman, new_nil(&environment));
    // environment.pacman->root->environment = &environment;
    if(argc < 3)
    {
        puts("missing argument, usage:\n./a.out <lisp dir> <project dir>");
        return 1;
    }
    struct Thing *lisp_root = add_slash_if_missing(argv[1], &environment);
    struct Thing *project_root = add_slash_if_missing(argv[2], &environment);
    
    struct Thing *parsed = parse_init_file(lisp_root->value, &environment);
    if(parsed == NULL)
    {
        puts("could not read filename (init.lisp)");
        return 1;
    }

    struct Thing *nil = new_nil(&environment);
    struct Thing *env = build_std_env();
    struct Thing *thunk = new_function(eval, env, &environment);
    struct Thing *args = new_cons(new_cons(new_cons(new_cons(new_symbol("lambda", &environment), new_cons(new_nil(&environment), parsed, &environment), &environment), nil, &environment), new_cons(lisp_root, new_cons(project_root, new_nil(&environment), &environment), &environment), &environment), new_nil(&environment), &environment);

    eval_loop(thunk, args, &environment);
    
    pacman_destroy(environment.pacman);
    printf("active references: %d\n", get_active_references());
    
    return 0;
}
