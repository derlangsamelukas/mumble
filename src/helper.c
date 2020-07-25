#include "helper.h"

int bit_or(int a, int b)
{
    return a | b;
}

SDL_Event create_sdl_event()
{
    SDL_Event eve;

    return eve;
}

int ptr_equals(void *p1, void *p2)
{
    return p1 == p2;
}

int equals(int a, int b)
{
    return a == b;
}

int multiply(int a, int b)
{
    return a * b;
}

int a_symbolp(char ch)
{
    return ((ch >= 'A' && ch <= 'z') || strchr("+-*/&?!<>.:", ch) != NULL) && ch != '`';
}

struct Thing *wrapper(const char *symbol, struct Thing *thing, struct Environment *environment)
{
    return new_cons(new_symbol(symbol, environment), new_cons(thing, new_nil(environment), environment), environment);
}

struct Thing *quote_wrapper(struct Thing *thing, struct Environment *environment)
{
    return wrapper("quote", thing, environment);
}

struct Thing *unquote_wrapper(struct Thing *thing, struct Environment *environment)
{
    return wrapper("unquote", thing, environment);
}

struct Thing *quasiquote_wrapper(struct Thing *thing, struct Environment *environment)
{
    return wrapper("quasiquote", thing, environment);
}

struct Thing *wrap_when_needed(struct Thing *thing, struct ParseObject *parse_object)
{
    if(parse_object->wrapper == NULL)
    {
        return thing;
    }

    struct FunctionWrapperCons *cons = parse_object->wrapper;
    thing = cons->car(thing, parse_object->environment);
    parse_object->wrapper = cons->cdr;
    free_memory(cons, "wrap_when_needed: FunctionWrapperCons");

    return thing;
}

void push_lst(struct Thing *car, struct ParseObject *parse_object)
{
    struct Thing *old = parse_object->lst;
    car = wrap_when_needed(car, parse_object);
    parse_object->lst = new_cons(car, old, parse_object->environment);
}

void a_symbol_cc(struct ParseObject *parse_object)
{
    if(a_symbolp(parse_object->ch))
    {
        parse_object->next = a_symbol_cc;
        return;
    }

    char *string = new_memory(sizeof(char) * (parse_object->index - parse_object->corresponding_value) +1, "a_symbol_cc");
    memcpy(
        string,
        parse_object->content + parse_object->corresponding_value,
        parse_object->index - parse_object->corresponding_value
    );
    string[parse_object->index - parse_object->corresponding_value] = '\0';
    push_lst(new_symbol_no_copy(string, parse_object->environment), parse_object);
    // parse_object->lst = new_cons(new_symbol_no_copy(string), parse_object->lst);

    return_corresponding_function(parse_object);
}

void a_symbol(struct ParseObject *parse_object)
{
    parse_object->corresponding_value = parse_object->index;
    
    parse_object->next = a_symbol_cc;
}

void a_string_cc(struct ParseObject *parse_object)
{
    if(parse_object->ch != '"')
    {
        parse_object->next = a_string_cc;
        return;
    }
    const int index = parse_object->index;

    char *string = new_memory(sizeof(char) * (index - parse_object->corresponding_value) +1, "a_string_cc");
    memcpy(
        string,
        parse_object->content + parse_object->corresponding_value,
        index - parse_object->corresponding_value
        );
    string[index - parse_object->corresponding_value] = '\0';
    push_lst(new_string_no_copy(string, parse_object->environment), parse_object);
    // parse_object->lst = new_cons(new_string_no_copy(string), parse_object->lst);

    parse_object->next = return_corresponding_function;
}

void a_string(struct ParseObject *parse_object)
{
    parse_object->corresponding_value = parse_object->index +1;
    parse_object->next = a_string_cc;
}

void a_list_start(struct ParseObject *parse_object)
{
    struct ParseObject copy;
    memcpy(&copy, parse_object, sizeof(struct ParseObject));
    copy.lst = new_nil(parse_object->environment);
    copy.index++;
    copy.next = return_corresponding_function;
    copy.wrapper = NULL;
    struct Thing *root = new_thing(parse_object->environment);
    root->type = &parse_object_type;
    root->value = &copy;
    struct Thing *old_root = copy.environment->pacman->root;
    pacman_set_root(copy.environment->pacman, new_cons(root, copy.environment->pacman->root, copy.environment));
    struct Thing *value = parse_helper(&copy);
    parse_object->index = copy.index;
    push_lst(value, parse_object);
    // parse_object->lst = new_cons(value, parse_object->lst);
    parse_object->next = return_corresponding_function;
    pacman_set_root(copy.environment->pacman, old_root);
}

void a_list_end(struct ParseObject *parse_object)
{
    parse_object->reached_end = 1;
    parse_object->next = return_corresponding_function;
}

void a_number_cc(struct ParseObject *parse_object)
{
    if(parse_object->ch >= '0' && parse_object->ch <= '9')
    {
        parse_object->next = a_number_cc;
        return;
    }

    double number = 0;
    sscanf(parse_object->content + parse_object->corresponding_value, "%lf", &number);
    push_lst(new_number(number, parse_object->environment), parse_object);

    return_corresponding_function(parse_object);
}

void a_integer_cc(struct ParseObject *parse_object)
{
    if(parse_object->ch >= '0' && parse_object->ch <= '9')
    {
        parse_object->next = a_integer_cc;
        return;
    }
    else if(parse_object->ch == '.')
    {
        parse_object->next = a_number_cc;
        return;
    }

    int integer = 0;
    sscanf(parse_object->content + parse_object->corresponding_value, "%d", &integer);
    push_lst(new_integer(integer, parse_object->environment), parse_object);

    return_corresponding_function(parse_object);
}

void a_bool_cc(struct ParseObject *parse_object)
{
    if(parse_object->ch == 't')
    {
        push_lst(new_bool(1, parse_object->environment), parse_object);
        parse_object->next = return_corresponding_function;
    }
    else if(parse_object->ch == 'f')
    {
        push_lst(new_bool(0, parse_object->environment), parse_object);
        parse_object->next = return_corresponding_function;
    }
    else
    {
        printf("unexpected char, expected 't' or 'f' got: '%c'\n", parse_object->ch);
        exit(1);
    }
}

void a_bool(struct ParseObject *parse_object)
{
    parse_object->next = a_bool_cc;
}

void a_integer(struct ParseObject *parse_object)
{
    parse_object->corresponding_value = parse_object->index;
    parse_object->next = a_integer_cc;
}

void a_whitespace(struct ParseObject *parse_object)
{
    parse_object->next = return_corresponding_function;
}

void a_comment_cc(struct ParseObject *parse_object)
{
    if(parse_object->ch != '\n' && parse_object->ch != '\0')
    {
        parse_object->next = a_comment_cc;
        return;
    }

    parse_object->next = return_corresponding_function;
}

void a_comment(struct ParseObject *parse_object)
{
    parse_object->next = a_comment_cc;
}

void wrap_with(struct Thing *(*fn)(struct Thing*, struct Environment*), struct ParseObject *parse_object)
{
    struct FunctionWrapperCons *wrapper = new_memory(sizeof(struct FunctionWrapperCons), "wrap_with: FunctionWrapperCons");
    wrapper->car = fn;
    wrapper->cdr = parse_object->wrapper;
    parse_object->wrapper = wrapper;
    parse_object->next = return_corresponding_function;
}

void a_quote(struct ParseObject *parse_object)
{
    wrap_with(quote_wrapper, parse_object);
}

void a_quasiquote(struct ParseObject *parse_object)
{
    wrap_with(quasiquote_wrapper, parse_object);
}

void a_unquote(struct ParseObject *parse_object)
{
    wrap_with(unquote_wrapper, parse_object);
}

void return_corresponding_function(struct ParseObject *parse_object)
{
    if(a_symbolp(parse_object->ch))
    {
        a_symbol(parse_object);
    }
    else if(parse_object->ch == '"')
    {
        a_string(parse_object);
    }
    else if(parse_object->ch == '(')
    {
        a_list_start(parse_object);
    }
    else if(parse_object->ch == ')')
    {
        a_list_end(parse_object);
    }
    else if(parse_object->ch >= '0' && parse_object->ch <= '9')
    {
        a_integer(parse_object);
    }
    else if(parse_object->ch == '#')
    {
        a_bool(parse_object);
    }
    else if(parse_object->ch == ' ' || parse_object->ch == '\n' || parse_object->ch == '\t')
    {
        a_whitespace(parse_object);
    }
    else if(parse_object->ch == ';')
    {
        a_comment(parse_object);
    }
    else if(parse_object->ch == '\'')
    {
        a_quote(parse_object);
    }
    else if(parse_object->ch == '`')
    {
        a_quasiquote(parse_object);
    }
    else if(parse_object->ch == ',')
    {
        a_unquote(parse_object);
    }
    else if(parse_object->ch == '\0')
    {
        //the end
    }
    else
    {
        printf("index: %d\n", parse_object->index);
        printf("char: %d not handled\n", parse_object->ch);
        exit(1);
    }
}

void string_free(char *string)
{
    free_memory(string, "string_free");
}

void string_ignore(char *string)
{
    
}

struct StringBuilder *new_string_builder(const char *init_and_copy_me)
{
    struct StringBuilder *builder = new_memory(sizeof(struct StringBuilder), "StringBuilder");
    builder->start = new_memory(sizeof(struct StringBuilderCons), "new_string_builder: StringBuilderCons");
    builder->end = builder->start;

    builder->start->next = NULL;

    builder->start->length = strlen(init_and_copy_me);
    builder->start->destroy = string_free;
    builder->start->string = string_dup(init_and_copy_me);
    
    return builder;
}

struct StringBuilder *string_builder_add_copy(struct StringBuilder *builder, const char *copy_me)
{
    builder = string_builder_add_ref(builder, string_dup(copy_me));
    builder->end->destroy = string_free;

    return builder;
}

struct StringBuilder *string_builder_add_ref(struct StringBuilder *builder, char *reference_me)
{
    struct StringBuilderCons *next = new_memory(sizeof(struct StringBuilderCons), "string_builder_add_ref: StringBuilderCons");

    next->length = strlen(reference_me);
    next->string = reference_me;
    next->destroy = string_free;
    next->next = NULL;
    
    builder->end->next = next;
    builder->end = next;

    return builder;
}

char *string_builder_to_string_and_destroy(struct StringBuilder *builder)
{
    int size = 0;
    struct StringBuilderCons *next = builder->start;
    do
    {
        size += next->length;
        next = next->next;
    } while(next != NULL);
    
    char *string = new_memory(sizeof(char) * (size +1), "string_builder_to_string (and destroy)");
    next = builder->start;
    int offset = 0;
    do
    {
        memcpy(string + offset, next->string, next->length);
        offset += next->length;

        struct StringBuilderCons *old = next;
        next = old->next;
        old->destroy(old->string);
        free_memory(old, "string_builder_to_string_and_destroy: StringBuilderCons");
    } while(next != NULL);

    string[offset] = '\0';

    free_memory(builder, "string_builder_to_string_and_destroy: StringBuilder");
    
    return string;
}

static struct StringBuilder *string_of_list_helper_identity(struct StringBuilder *builder)
{
    return builder;
}

static struct StringBuilder *string_of_list_helper_add_space(struct StringBuilder *builder)
{
    return string_builder_add_copy(builder, " ");
}

typedef struct StringBuilder *(*BuildFn)(struct StringBuilder*);

char *string_of_thing(struct Thing *thing)
{
    if(thing->type == &TYPES.string)
    {
        struct StringBuilder *builder = new_string_builder("\"");
        builder = string_builder_add_copy(builder, thing->value);
        builder = string_builder_add_copy(builder, "\"");

        return string_builder_to_string_and_destroy(builder);
    }
    else if(thing->type == &TYPES.symbol)
    {
        return string_dup(thing->value);
    }
    else if(thing->type == &TYPES.integer)
    {
        int decimals = 1;
        int original = *((int*)thing->value);
        double value = (double)original;
        while(value / pow(10, (double)decimals) >= 1)
        {
            decimals++;
        }
        char *buffer = new_memory(sizeof(char) * (decimals +1), "string_of_thing: integer");
        sprintf(buffer, "%d", original);

        return buffer;
    }
    else if(thing->type == &TYPES.number)
    {
        int decimals = 1;
        double original = *((double*)thing->value);
        double value = original;
        while(value / pow(10, (double)decimals) >= 1)
        {
            decimals++;
        }
        double points = original - floor(original);
        while(points != floor(points))
        {
            points = points * 10;
            points = points - floor(original);
            decimals++;
        }
        char *buffer = new_memory(sizeof(char) * (decimals +1), "string_of_thing: number");
        sprintf(buffer, "%lf", original);

        return buffer;
    }
    else if(thing->type == &TYPES.cons)
    {
        return string_of_list(thing);
    }
    else if(thing->type == &TYPES.nil)
    {
        return string_dup("()");
    }
    else if(thing->type == &TYPES.bool)
    {
        return string_dup(*((int*)thing->value) == 0 ? "#f" : "#t");
    }
    else if(thing->type == &TYPES.native)
    {
        char *buffer = new_memory(sizeof(char) * (9 + 15 + 1), "string_of_thing: native");// 15 = pointer length
        sprintf(buffer, "<native %p>", thing->value);

        return buffer;
    }
    else if(thing->type == &TYPES.function)
    {
        char *buffer = new_memory(sizeof(char) * 11 + 15 + 1, "string_of_thing: function");
        sprintf(buffer, "<function %p>", thing->value);

        return buffer;
    }
    else
    {
        char *buffer = new_memory(sizeof(char) * 12 + 15 + 1 + strlen(thing->type->name), "string_of_thing: function");
        sprintf(buffer, "<custom %s %p>", thing->type->name, thing->value);

        return buffer;
        /* printf("string_of_thing: dunno, type is nr: %d\n", thing->type->id); */
        /* exit(1); */
    }
}

char *string_of_list(struct Thing *lst)
{
    struct StringBuilder *builder = new_string_builder("");
    builder = string_builder_add_copy(builder, "(");
    BuildFn add = string_of_list_helper_identity;
    
    while(lst->type == &TYPES.cons)
    {
        struct Cons *cons = (struct Cons*)lst->value;
        builder = add(builder);
        builder = string_builder_add_ref(builder, string_of_thing(cons->car));
        lst = cons->cdr;
        add = string_of_list_helper_add_space;
    }

    if(lst->type != &TYPES.nil)
    {
        builder = string_builder_add_copy(builder, " . ");
        builder = string_builder_add_ref(builder, string_of_thing(lst));
    }

    builder = string_builder_add_copy(builder, ")");

    return string_builder_to_string_and_destroy(builder);
}

void parse_object_free(struct Thing *thing)
{
    free_memory(thing, "parse_object_free");
}

void parse_object_mark(struct Thing *thing)
{
    simple_mark(thing);
    struct ParseObject *parse_object = thing->value;
    if(!parse_object->lst->marked)
    {
        thing_mark(parse_object->lst);
    }
}

struct Thing *parse_helper(struct ParseObject *parse_object)
{
    int eat_counter = 0;
    while(parse_object->index < parse_object->end && !parse_object->reached_end)
    {
        parse_object->ch = parse_object->content[parse_object->index];
        parse_object->next(parse_object);
        parse_object->index += !parse_object->reached_end;

        if(eat_counter > 1000)
        {
            pacman_mark_and_eat(parse_object->environment->pacman);
            eat_counter = 0;
        }
        eat_counter++;
    }
    
    parse_object->ch = '\0';
    parse_object->next(parse_object);

    struct Thing *reversed = lst_reverse(parse_object->lst, parse_object->environment);
    parse_object->lst = reversed;

    if(parse_object->wrapper != NULL)
    {
        puts("parse_object->wrapper != NULL");
        exit(1);
    }

    return reversed;
}

struct Thing *parse(const char *content, struct Environment *environment)
{
    struct ParseObject parse_object = {content[0], new_nil(environment), 0, 0, content, return_corresponding_function, strlen(content), 0, NULL, environment};
    struct Thing *root = new_thing(environment);
    struct Thing *old_root = environment->pacman->root;
    root->value = &parse_object;
    root->type = &parse_object_type;
    pacman_set_root(environment->pacman, new_cons(root, old_root, environment));

    struct Thing *thing = parse_helper(&parse_object);

    pacman_set_root(environment->pacman, old_root);

    return thing;
}

const struct Type parse_object_type = {"ParseObject", 345628, parse_object_free, parse_object_mark};

struct Thing *lst_reverse(struct Thing *lst, struct Environment *environment)
{
    struct Thing *current = new_nil(environment);

    while(lst->type == &TYPES.cons)
    {
        struct Cons *cons = (struct Cons*)lst->value;
        struct Thing *old = current;
        current = new_cons(cons->car, old, environment);
        lst = cons->cdr;
    }

    return current;
}

int listp(struct Thing *thing)
{
    return thing->type == &TYPES.cons || thing->type == &TYPES.nil;
}
