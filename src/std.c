#include "std.h"

void assoc_(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("assoc: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    if(cons->cdr->type != &TYPES.cons)
    {
        raiseit(new_string("assoc: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *second_arg = (struct Cons*)cons->cdr->value;
    if(second_arg->cdr->type != &TYPES.nil)
    {
        raiseit(new_string("assoc: too many arguments", eva->environment), eva);
        return;
    }
    if(cons->car->type != &TYPES.symbol)
    {
        raiseit(new_string("assoc: second argument is no symbol", eva->environment), eva);
        return;
    }

    update_args(new_cons(assoc(cons->car->value, second_arg->car, eva->environment), new_nil(eva->environment), eva->environment), eva);
}

void build_std_env_(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.nil)
    {
        raiseit(new_string("build_std_env_: too many arguments", eva->environment), eva);
        return;
    }

    update_args(new_cons(build_std_env(eva->environment), new_nil(eva->environment), eva->environment), eva);
}

void read_(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("read_: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    if(cons->cdr->type != &TYPES.nil)
    {
        raiseit(new_string("read_: too many arguments", eva->environment), eva);
        return;
    }
    if(cons->car->type != &TYPES.string)
    {
        raiseit(new_string("read_: second argument is no string", eva->environment), eva);
        return;
    }

    struct Thing *parsed = parse(cons->car->value, eva->environment);

    update_args(new_cons(parsed, new_nil(eva->environment), eva->environment), eva);
}

char* read_file (const char* filename);

void read_file_as_string(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("read_file_as_string: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    if(cons->cdr->type != &TYPES.nil)
    {
        raiseit(new_string("read_file_as_string: too many arguments", eva->environment), eva);
        return;
    }
    if(cons->car->type != &TYPES.string)
    {
        raiseit(new_string("read_file_as_string: second argument is no string", eva->environment), eva);
        return;
    }

    char *content = read_file(cons->car->value);
    if(NULL == content)
    {
        update_args(new_cons(new_bool(0, eva->environment), new_nil(eva->environment), eva->environment), eva);
        return;
    }

    update_args(new_cons(new_string_no_copy(content, eva->environment), new_nil(eva->environment), eva->environment), eva);
}

void eval_expr(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("eval: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    if(cons->cdr->type != &TYPES.cons)
    {
        raiseit(new_string("eval: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *second_arg = (struct Cons*)cons->cdr->value;
    if(second_arg->cdr->type != &TYPES.nil)
    {
        raiseit(new_string("eval: too many arguments", eva->environment), eva);
        return;
    }
    if(second_arg->car->type != &TYPES.cons) // env
    {
        raiseit(new_string("eval: second argument is no cons", eva->environment), eva);
        return;
    }

    push_next(new_function(eval, second_arg->car, eva->environment), eva);
    update_args(new_cons(cons->car, new_nil(eva->environment), eva->environment), eva);
}

void apply(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("apply: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    if(cons->cdr->type != &TYPES.cons)
    {
        raiseit(new_string("apply: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *second_arg = (struct Cons*)cons->cdr->value;
    if(second_arg->cdr->type != &TYPES.nil)
    {
        raiseit(new_string("apply: too many arguments", eva->environment), eva);
        return;
    }
    if(cons->car->type != &TYPES.function)
    {
        raiseit(new_string("apply: first argument is no function", eva->environment), eva);
        return;
    }
    if(!listp(second_arg->car))
    {
        raiseit(new_string("apply: second argument is no list", eva->environment), eva);
        return;
    }

    push_next(cons->car, eva);
    update_args(second_arg->car, eva);
}

void concat(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("concat: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    if(cons->cdr->type != &TYPES.cons)
    {
        raiseit(new_string("concat: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *second_arg = (struct Cons*)cons->cdr->value;
    if(second_arg->cdr->type != &TYPES.nil)
    {
        raiseit(new_string("concat: too many arguments", eva->environment), eva);
        return;
    }
    if(cons->car->type != &TYPES.string)
    {
        raiseit(new_string("concat: first argument is no string", eva->environment), eva);
        return;
    }
    if(second_arg->car->type != &TYPES.string)
    {
        raiseit(new_string("concat: second argument is no string", eva->environment), eva);
        return;
    }

    struct StringBuilder *builder = new_string_builder(cons->car->value);
    builder = string_builder_add_copy(builder, second_arg->car->value);

    update_args(new_cons(new_string_no_copy(string_builder_to_string_and_destroy(builder), eva->environment), new_nil(eva->environment), eva->environment), eva);
}

void plus__(const char *name, int (plus_integer)(int, int), double (plus_number)(double, double), struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        struct StringBuilder *builder = new_string_builder(name);
        builder = string_builder_add_copy(builder, ": not enough arguments");
        raiseit(new_string_no_copy(string_builder_to_string_and_destroy(builder), eva->environment), eva);
        return;
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    if(cons->cdr->type != &TYPES.cons)
    {
        struct StringBuilder *builder = new_string_builder(name);
        builder = string_builder_add_copy(builder, ": not enough arguments");
        raiseit(new_string_no_copy(string_builder_to_string_and_destroy(builder), eva->environment), eva);
        return;
    }
    struct Cons *second_arg = (struct Cons*)cons->cdr->value;
    if(second_arg->cdr->type != &TYPES.nil)
    {
        struct StringBuilder *builder = new_string_builder(name);
        builder = string_builder_add_copy(builder, ": too many arguments");
        raiseit(new_string_no_copy(string_builder_to_string_and_destroy(builder), eva->environment), eva);
        return;
    }
    if(cons->car->type != second_arg->car->type)
    {
        struct StringBuilder *builder = new_string_builder(name);
        builder = string_builder_add_copy(builder, ": arguments don't have the same type");
        raiseit(new_string_no_copy(string_builder_to_string_and_destroy(builder), eva->environment), eva);
        return;
    }
    if(cons->car->type == &TYPES.integer)
    {
        update_args(new_cons(new_integer(plus_integer(*((int*)cons->car->value), *((int*)second_arg->car->value)), eva->environment), new_nil(eva->environment), eva->environment), eva);
    }
    else if(cons->car->type == &TYPES.number)
    {
        update_args(new_cons(new_number(plus_number(*((double*)cons->car->value), *((double*)second_arg->car->value)), eva->environment), new_nil(eva->environment), eva->environment), eva);
    }
    else
    {
        struct StringBuilder *builder = new_string_builder(name);
        builder = string_builder_add_copy(builder, ": arguments are neither integer or number");
        raiseit(new_string_no_copy(string_builder_to_string_and_destroy(builder), eva->environment), eva);
    }
}

int    plus_integer  (int    a, int    b) { return a + b; }
double plus_double   (double a, double b) { return a + b; }
int    minus_integer (int    a, int    b) { return a - b; }
double minus_double  (double a, double b) { return a - b; }
int    mult_integer  (int    a, int    b) { return a * b; }
double mult_double   (double a, double b) { return a * b; }
int    divide_integer(int    a, int    b) { return a / b; }
double divide_double (double a, double b) { return a / b; }

void plus(struct Thing *env, struct Eva *eva)
{
    plus__("+", plus_integer, plus_double, env, eva);
}

void minus(struct Thing *env, struct Eva *eva)
{
    plus__("-", minus_integer, minus_double, env, eva);
}

void mult(struct Thing *env, struct Eva *eva)
{
    plus__("*", mult_integer, mult_double, env, eva);
}

void divide(struct Thing *env, struct Eva *eva)
{
    plus__("/", divide_integer, divide_double, env, eva);
}


void string_to_symbol(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("string->symbol: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    if(cons->cdr->type != &TYPES.nil)
    {
        raiseit(new_string("string->symbol: too many arguments", eva->environment), eva);
        return;
    }

    if(cons->car->type != &TYPES.string)
    {
        raiseit(new_string("string->symbol: first argument is no string", eva->environment), eva);
        return;
    }

    update_args(new_cons(new_symbol(cons->car->value, eva->environment), new_nil(eva->environment), eva->environment), eva);
}

void symbol_to_string(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("symbol->string: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    if(cons->cdr->type != &TYPES.nil)
    {
        raiseit(new_string("symbol->string: too many arguments", eva->environment), eva);
        return;
    }

    if(cons->car->type != &TYPES.symbol)
    {
        raiseit(new_string("symbol->string: first argument is no symbol", eva->environment), eva);
        return;
    }

    update_args(new_cons(new_string(cons->car->value, eva->environment), new_nil(eva->environment), eva->environment), eva);
}

void functionp(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("functionp: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    if(cons->cdr->type != &TYPES.nil)
    {
        raiseit(new_string("functionp: too many arguments", eva->environment), eva);
        return;
    }
    update_args(new_cons(new_bool(cons->car->type == &TYPES.function, eva->environment), new_nil(eva->environment), eva->environment), eva);
}

void integerp(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("integerp: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    if(cons->cdr->type != &TYPES.nil)
    {
        raiseit(new_string("integerp: too many arguments", eva->environment), eva);
        return;
    }
    update_args(new_cons(new_bool(cons->car->type == &TYPES.integer, eva->environment), new_nil(eva->environment), eva->environment), eva);
}

void numberp(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("numberp: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    if(cons->cdr->type != &TYPES.nil)
    {
        raiseit(new_string("numberp: too many arguments", eva->environment), eva);
        return;
    }
    update_args(new_cons(new_bool(cons->car->type == &TYPES.number, eva->environment), new_nil(eva->environment), eva->environment), eva);
}

void pairp(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("pairp: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    if(cons->cdr->type != &TYPES.nil)
    {
        raiseit(new_string("pairp: too many arguments", eva->environment), eva);
        return;
    }
    update_args(new_cons(new_bool(cons->car->type == &TYPES.cons, eva->environment), new_nil(eva->environment), eva->environment), eva);
}

void stringp(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("stringp: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    if(cons->cdr->type != &TYPES.nil)
    {
        raiseit(new_string("stringp: too many arguments", eva->environment), eva);
        return;
    }
    update_args(new_cons(new_bool(cons->car->type == &TYPES.string, eva->environment), new_nil(eva->environment), eva->environment), eva);
}

void symbolp(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("symbolp: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    if(cons->cdr->type != &TYPES.nil)
    {
        raiseit(new_string("symbolp: too many arguments", eva->environment), eva);
        return;
    }
    update_args(new_cons(new_bool(cons->car->type == &TYPES.symbol, eva->environment), new_nil(eva->environment), eva->environment), eva);
}

void boolp(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("boolp: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    if(cons->cdr->type != &TYPES.nil)
    {
        raiseit(new_string("boolp: too many arguments", eva->environment), eva);
        return;
    }
    update_args(new_cons(new_bool(cons->car->type == &TYPES.bool, eva->environment), new_nil(eva->environment), eva->environment), eva);
}

void identity(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("identity: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    if(cons->cdr->type != &TYPES.nil)
    {
        raiseit(new_string("identity: too arguments", eva->environment), eva);
        return;
    }
    update_args(new_cons(cons->car, new_nil(eva->environment), eva->environment), eva);
}

void list(struct Thing *env, struct Eva *eva)
{
    update_args(new_cons(eva->args, new_nil(eva->environment), eva->environment), eva);
}

void cons(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        puts("cons: not enough arguments");
        exit(1);
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    if(cons->cdr->type != &TYPES.cons)
    {
        puts("cons: not enough arguments");
        exit(1);
    }
    struct Cons *second_arg = (struct Cons*)cons->cdr->value;
    update_args(new_cons(new_cons(cons->car, second_arg->car, eva->environment), new_nil(eva->environment), eva->environment), eva);
}

void nullp(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        puts("nil?: not enough arguments");
        exit(1);
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    update_args(new_cons(new_bool(cons->car->type == &TYPES.nil, eva->environment), new_nil(eva->environment), eva->environment), eva);
}

void if_helper_execute(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        puts("if callback execute: not enough arguments");
        exit(1);
    }
    struct Cons *args_cons = (struct Cons*)eva->args->value;
    struct Cons *env_cons = env->value;
    struct Cons *second_env_cons = env_cons->cdr->value;
    struct Thing *true_thunk = second_env_cons->car;
    struct Thing *false_thunk = second_env_cons->cdr;

    push_next(args_cons->car->type != &TYPES.bool || *((int*)args_cons->car->value) != 0 ? true_thunk : false_thunk, eva);
    update_args(new_cons(env_cons->car, new_nil(eva->environment), eva->environment), eva);
}

void if_helper(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        puts("if callback: not enough arguments");
        exit(1);
    }
    struct Cons *args_cons = (struct Cons*)eva->args->value;
    if(args_cons->cdr->type != &TYPES.nil)
    {
        puts("if callback: too many arguments");
        exit(1);
    }
    struct Cons *env_cons = (struct Cons*)env->value;
    struct Thing *bool_thunk = env_cons->car;
    struct Cons *second_env_cons = (struct Cons*)env_cons->cdr->value;
    struct Thing *true_thunk = second_env_cons->car;
    struct Thing *false_thunk = second_env_cons->cdr;

    push_next(new_function(if_helper_execute, new_cons(args_cons->car, new_cons(true_thunk, false_thunk, eva->environment), eva->environment), eva->environment), eva);
    push_next(bool_thunk, eva);
    update_args(new_cons(args_cons->car, new_nil(eva->environment), eva->environment), eva);
}

void if_(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        puts("if: not enough arguments");
        exit(1);
    }

    struct Cons *cons = (struct Cons*)eva->args->value;
    struct Thing *bool_thunk = cons->car;
    if(cons->cdr->type != &TYPES.cons)
    {
        puts("if: not enough arguments");
        exit(1);
    }
    struct Cons *second_cons = (struct Cons*)cons->cdr->value;
    struct Thing *true_thunk = second_cons->car;
    if(second_cons->cdr->type != &TYPES.cons)
    {
        puts("if: not enough arguments");
        exit(1);
    }
    struct Cons *third_cons = (struct Cons*)second_cons->cdr->value;
    struct Thing *false_thunk = third_cons->car;
    if(third_cons->cdr->type != &TYPES.nil)
    {
        puts("if: to many arguments");
        exit(1);
    }
    if(bool_thunk->type != &TYPES.function || true_thunk->type != &TYPES.function || false_thunk->type != &TYPES.function)
    {
        puts("if: all arguments must be functions");
        exit(1);
    }
    update_args(new_cons(new_function(if_helper, new_cons(bool_thunk, new_cons(true_thunk, false_thunk, eva->environment), eva->environment), eva->environment), new_nil(eva->environment), eva->environment), eva);
}

void car_(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("car: args are no cons", eva->environment), eva);
        return;
    }
    struct Cons *cons = eva->args->value;
    if(cons->car->type != &TYPES.cons)
    {
        raiseit(new_string("car: argument is no cons", eva->environment), eva);
        return;
    }
    struct Cons *car_cons = cons->car->value;
    update_args(new_cons(car_cons->car, new_nil(eva->environment), eva->environment), eva);
}

void cdr_(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("cdr: args are no cons", eva->environment), eva);
        return;
    }
    struct Cons *cons = eva->args->value;
    if(cons->car->type != &TYPES.cons)
    {
        raiseit(new_string("cdr: argument is no cons", eva->environment), eva);
        return;
    }
    struct Cons *car_cons = cons->car->value;
    update_args(new_cons(car_cons->cdr, new_nil(eva->environment), eva->environment), eva);
}

int thing_equal(struct Thing *first, struct Thing *second)
{
    if(first->type != second->type)
    {
        return 0;
    }
    if(first->type == &TYPES.nil)
    {
        return 1;
    }
    else if(first->type == &TYPES.string || first->type == &TYPES.symbol)
    {
        return 0 == strcmp(first->value, second->value);
    }
    else if(first->type == &TYPES.bool)
    {
        return *((int*)first->value) == *((int*)second->value);
    }
    else if(first->type == &TYPES.cons)
    {
        struct Cons *first_cons = (struct Cons*)first->value;
        struct Cons *second_cons = (struct Cons*)second->value;

        return thing_equal(first_cons->car, second_cons->car) && thing_equal(first_cons->cdr, second_cons->cdr);
    }
    else if(first->type == &TYPES.number)
    {
        return *((double*)first->value) == *((double*)second->value);
    }
    else if(first->type == &TYPES.integer)
    {
        return *((int*)first->value) == *((int*)second->value);
    }
    else // if(first->type == &TYPES.native || first->type == &TYPES.function and custom types)
    {
        return first->value == second->value;
    }
}

void equal(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("equal?: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *args_cons = eva->args->value;
    if(args_cons->cdr->type != &TYPES.cons)
    {
        raiseit(new_string("equal?: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *second_args = args_cons->cdr->value;
    if(second_args->cdr->type != &TYPES.nil)
    {
        raiseit(new_string("equal?: too many arguments", eva->environment), eva);
        return;
    }
    struct Thing *first = args_cons->car;
    struct Thing *second = second_args->car;

    update_args(new_cons(new_bool(thing_equal(first, second), eva->environment), new_nil(eva->environment), eva->environment), eva);
}

void lst_length(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("length: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *args_cons = eva->args->value;
    struct Thing *lst = args_cons->car;
    int length = 0;
    while(lst->type == &TYPES.cons)
    {
        struct Cons *cons = lst->value;
        lst = cons->cdr;
        length++;
    }
    if(lst->type != &TYPES.nil)
    {
        raiseit(new_string("length: argument is no proper list", eva->environment), eva);
        return;
    }
    update_args(new_cons(new_integer(length, eva->environment), new_nil(eva->environment), eva->environment), eva);
}

void typeof_(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("typeof: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *args_cons = eva->args->value;

    update_args(new_cons(new_string(args_cons->car->type->name, eva->environment), new_nil(eva->environment), eva->environment), eva);
}

void reverse(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("reverse: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *args_cons = eva->args->value;

    update_args(new_cons(lst_reverse(args_cons->car, eva->environment), new_nil(eva->environment), eva->environment), eva);
}

void listp_(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raiseit(new_string("reverse: not enough arguments", eva->environment), eva);
        return;
    }
    struct Cons *args_cons = eva->args->value;

    update_args(new_cons(new_bool(listp(args_cons->car), eva->environment), new_nil(eva->environment), eva->environment), eva);
}

struct Thing *simple_entry(const char *name, void (*fn)(struct Thing*, struct Eva*), struct Environment *environment)
{
    return new_cons(new_symbol(name, environment), new_function(fn, new_nil(environment), environment), environment);
}

struct Thing *push_std(struct Thing *car, struct Thing *lst, struct Environment *environment)
{
    struct Thing *new_lst = new_cons(car, lst, environment);

    return new_lst;
}

void const_(struct Thing *env, struct Eva *eva)
{
    update_args(env, eva);
}

void quote_macro(struct Thing *env, struct Eva *eva)
{
    update_args(new_cons(new_cons(new_function(const_, eva->args, eva->environment), new_nil(eva->environment), eva->environment), new_nil(eva->environment), eva->environment), eva);
}

struct Thing *build_std_fn_env(struct Environment *environment)
{
    struct Thing *env = new_nil(environment);

    env = push_std(simple_entry("list", list, environment), env, environment);
    env = push_std(simple_entry("cons", cons, environment), env, environment);
    env = push_std(simple_entry("print", print_, environment), env, environment);
    env = push_std(simple_entry("null?", nullp, environment), env, environment);
    env = push_std(simple_entry("if*", if_, environment), env, environment);
    env = push_std(simple_entry("car", car_, environment), env, environment);
    env = push_std(simple_entry("cdr", cdr_, environment), env, environment);
    env = push_std(simple_entry("equal?", equal, environment), env, environment);
    env = push_std(simple_entry("length", lst_length, environment), env, environment);
    env = push_std(simple_entry("typeof", typeof_, environment), env, environment);
    env = push_std(simple_entry("reverse", reverse, environment), env, environment);
    env = push_std(simple_entry("list?", listp_, environment), env, environment);
    env = push_std(simple_entry("function?", functionp, environment), env, environment);
    env = push_std(simple_entry("integer?", integerp, environment), env, environment);
    env = push_std(simple_entry("number?", numberp, environment), env, environment);
    env = push_std(simple_entry("bool?", boolp, environment), env, environment);
    env = push_std(simple_entry("string?", stringp, environment), env, environment);
    env = push_std(simple_entry("symbol?", symbolp, environment), env, environment);
    env = push_std(simple_entry("pair?", pairp, environment), env, environment);
    env = push_std(simple_entry("identity", identity, environment), env, environment);
    env = push_std(simple_entry("symbol->string", symbol_to_string, environment), env, environment);
    env = push_std(simple_entry("string->symbol", string_to_symbol, environment), env, environment);
    env = push_std(simple_entry("identity", identity, environment), env, environment);
    env = push_std(simple_entry("+", plus, environment), env, environment);
    env = push_std(simple_entry("-", minus, environment), env, environment);
    env = push_std(simple_entry("*", mult, environment), env, environment);
    env = push_std(simple_entry("/", divide, environment), env, environment);
    env = push_std(simple_entry("concat", concat, environment), env, environment);
    env = push_std(simple_entry("apply", apply, environment), env, environment);
    env = push_std(simple_entry("eval", eval_expr, environment), env, environment);
    env = push_std(simple_entry("read-file-as-string", read_file_as_string, environment), env, environment);
    env = push_std(simple_entry("read", read_, environment), env, environment);
    env = push_std(simple_entry("build-std-env", build_std_env_, environment), env, environment);
    env = push_std(simple_entry("assoc", assoc_, environment), env, environment);

    return env;
}

struct Thing *build_std_macro_env(struct Environment *environment)
{
    struct Thing *env = new_nil(environment);

    env = push_std(simple_entry("quote", quote_macro, environment), env, environment);

    return env;
}

struct Thing *build_std_env(struct Environment *environment)
{
    struct Thing *fn_env = add_sdl_lib(build_std_fn_env(environment), environment);
    struct Thing *macro_env = build_std_macro_env(environment);

    struct Thing *env = new_cons(macro_env, fn_env, environment);

    return env;
}
