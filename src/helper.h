#ifndef HELPER_H
#define HELPER_H

#include <math.h>
#include "SDL.h"
#include "types.h"

struct StringBuilderCons
{
    int length;
    struct StringBuilderCons *next;
    char *string;
    void (*destroy)(char*);
};

struct FunctionWrapperCons
{
    struct Thing *(*car)(struct Thing*, struct Environment*);
    struct FunctionWrapperCons *cdr;
};

struct StringBuilder
{
    struct StringBuilderCons *start;
    struct StringBuilderCons *end;
};

struct ParseObject;
typedef void (*Fn)(struct ParseObject*);

struct ParseObject
{
    char ch;
    struct Thing *lst;
    int corresponding_value;
    int index;
    const char *content;
    Fn next;
    const int end;
    int reached_end;
    struct FunctionWrapperCons *wrapper;
    struct Environment *environment;
};

int bit_or(int, int);
SDL_Event create_sdl_event();
int ptr_equals(void *p1, void *p2);
int equals(int a, int b);
int multiply(int a, int b);
struct ParseObject;
void return_corresponding_function(struct ParseObject *parse_object);
char *string_of_list(struct Thing *lst);
char *string_of_thing(struct Thing *thing);
struct Thing *parse_helper(struct ParseObject *parse_object);
struct Thing *parse(const char *content, struct Environment *environment);
struct Thing *lst_reverse(struct Thing *lst, struct Environment *environment);

struct StringBuilder *new_string_builder(const char *init_and_copy_me);
struct StringBuilder *string_builder_add_copy(struct StringBuilder *builder, const char *copy_me);
struct StringBuilder *string_builder_add_ref(struct StringBuilder *builder, char *reference_me);
char *string_builder_to_string_and_destroy(struct StringBuilder *builder);
void parse_object_free(struct Thing *thing);
void parse_object_mark(struct Thing *thing);
int listp(struct Thing *thing);

extern const struct Type parse_object_type;

#endif
