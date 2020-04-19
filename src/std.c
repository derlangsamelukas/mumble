#include "std.h"

void list(struct Thing *env, struct Eva *eva)
{
    update_args(new_cons(eva->args, new_nil()), eva);
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
    update_args(new_cons(new_cons(cons->car, second_arg->car), new_nil()), eva);
}

void nullp(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        puts("nil?: not enough arguments");
        exit(1);
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    update_args(new_cons(new_bool(cons->car->type == &TYPES.nil), new_nil()), eva);
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
    update_args(new_cons(env_cons->car, new_nil()), eva);
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

    push_next(new_function(if_helper_execute, new_cons(args_cons->car, new_cons(true_thunk, false_thunk))), eva);
    push_next(bool_thunk, eva);
    update_args(new_cons(args_cons->car, new_nil()), eva);
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
    update_args(new_cons(new_function(if_helper, new_cons(bool_thunk, new_cons(true_thunk, false_thunk))), new_nil()), eva);
}

void car_(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raise(new_string("car: args are no cons"), eva);
        return;
    }
    struct Cons *cons = eva->args->value;
    if(cons->car->type != &TYPES.cons)
    {
        raise(new_string("car: argument is no cons"), eva);
        return;
    }
    struct Cons *car_cons = cons->car->value;
    update_args(new_cons(car_cons->car, new_nil()), eva);
}

void cdr_(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        raise(new_string("cdr: args are no cons"), eva);
        return;
    }
    struct Cons *cons = eva->args->value;
    if(cons->car->type != &TYPES.cons)
    {
        raise(new_string("cdr: argument is no cons"), eva);
        return;
    }
    struct Cons *car_cons = cons->car->value;
    update_args(new_cons(car_cons->cdr, new_nil()), eva);
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
        raise(new_string("equal?: not enough arguments"), eva);
        return;
    }
    struct Cons *args_cons = eva->args->value;
    if(args_cons->cdr->type != &TYPES.cons)
    {
        raise(new_string("equal?: not enough arguments"), eva);
        return;
    }
    struct Cons *second_args = args_cons->cdr->value;
    if(second_args->cdr->type != &TYPES.nil)
    {
        raise(new_string("equal?: too many arguments"), eva);
        return;
    }
    struct Thing *first = args_cons->car;
    struct Thing *second = second_args->car;

    update_args(new_cons(new_bool(thing_equal(first, second)), new_nil()), eva);
}

struct Thing *simple_entry(const char *name, void (*fn)(struct Thing*, struct Eva*))
{
    return new_cons(new_symbol(name), new_function(fn, new_nil()));
}

struct Thing *push_std(struct Thing *car, struct Thing *lst)
{
    struct Thing *new_lst = new_cons(car, lst);

    return new_lst;
}

void const_(struct Thing *env, struct Eva *eva)
{
    update_args(env, eva);
}

void quote_macro(struct Thing *env, struct Eva *eva)
{
    update_args(new_cons(new_cons(new_function(const_, eva->args), new_nil()), new_nil()), eva);
}

struct Thing *build_std_fn_env()
{
    struct Thing *env = new_nil();

    env = push_std(simple_entry("list", list), env);
    env = push_std(simple_entry("cons", cons), env);
    env = push_std(simple_entry("print", print_), env);
    env = push_std(simple_entry("null?", nullp), env);
    env = push_std(simple_entry("if*", if_), env);
    env = push_std(simple_entry("car", car_), env);
    env = push_std(simple_entry("cdr", cdr_), env);
    env = push_std(simple_entry("equal?", equal), env);

    return env;
}

struct Thing *build_std_macro_env()
{
    struct Thing *env = new_nil();

    env = push_std(simple_entry("quote", quote_macro), env);

    return env;
}

struct Thing *build_std_env()
{
    struct Thing *fn_env = build_std_fn_env();
    struct Thing *macro_env = build_std_macro_env();

    struct Thing *env = new_cons(macro_env, fn_env);

    return env;
}
