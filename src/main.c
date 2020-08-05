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
    if(argc < 2)
    {
        puts("missing argument, usage:\n./a.out <lisp file>");
        return 1;
    }
    
    
    struct Thing *parsed = parse_file(argv[1], &environment);
    if(parsed == NULL)
    {
        printf("could not parse file: '%s'", argv[1]);
        return 1;
    }
    
    struct Thing *env = build_std_env(&environment);
    struct Thing *thunk = new_function(eval_next_try, env, &environment);

    eval_loop(thunk, parsed, &environment);
    
    pacman_destroy(environment.pacman);
    printf("active references: %d\n", get_active_references());
    
    return 0;
}
