#include "eval.h"

void push_next(struct Thing *next, struct Eva *eva)
{
    struct Thing *old = eva->next;
    eva->next = new_cons(next, old);
    thing_track(old, eva->next);
}

void raiseit(struct Thing *message, struct Eva *eva)
{
    struct Thing *cc_stack = eva->next;
    eva->next = new_nil();
    thing_track(cc_stack, eva->next);
    if(eva->unwind->type != &TYPES.cons)
    {
        puts("raise called but no unwind handler is set");
        exit(1);
    }
    struct Thing *old_unwind = eva->unwind;
    struct Cons *unwind = (struct Cons*)eva->unwind->value;
    push_next(unwind->car, eva);
    update_args(new_cons(message, new_cons(cc_stack, new_nil())), eva);

    eva->unwind = unwind->cdr;
}

void update_args(struct Thing *args, struct Eva *eva)
{
    
    thing_track(eva->args, args);
    eva->args = args;
}

struct Thing *assoc(const char *symbol, struct Thing *lst)
{
    if(lst->type != &TYPES.cons)
    {
        return new_nil();
    }

    struct Cons *cons = (struct Cons*)lst->value;
    if(cons->car->type == &TYPES.cons)
    {
        struct Cons *car = (struct Cons*)cons->car->value;
        if(car->car->type == &TYPES.symbol && 0 == strcmp(symbol, (char*)car->car->value))
        {
            return cons->car;
        }
    }

    return assoc(symbol, cons->cdr);
}

struct Thing *resolve_symbol(const char *symbol, struct Thing *env)
{
    if(env->type != &TYPES.cons)
    {
        puts("resolve_symbol: env is no cons");
        exit(1);
    }
    struct Cons *env_cons = (struct Cons*)env->value;
    struct Thing *thing = assoc(symbol, env_cons->cdr);
    // puts(string_of_thing(env_cons->cdr));
    if(thing->type != &TYPES.cons)
    {
        return thing;
    }
    struct Cons *cons = (struct Cons*)thing->value;

    struct Thing *cdr = cons->cdr;
    return cdr;
}

void print_(struct Thing *env, struct Eva *eva)
{
    char *string = string_of_thing(eva->args);
    puts(string);
    free_memory(string, "print_: free thing_of_thing");

    update_args(new_nil(), eva);

    // raise(dope(new_string("whooops")), eva);
}

void progn(struct Thing *env, struct Eva *eva)
{
    if(env->type != &TYPES.cons)
    {
        puts("progn: env is no cons");
        exit(1);
    }
    struct Cons *pair = (struct Cons*)env->value;
    struct Thing *expr = pair->car;
    struct Thing *expr_env = pair->cdr;

    struct Thing *thunk = new_function(eval, expr_env);
    push_next(thunk, eva);
    update_args(new_cons(expr, new_nil()), eva);
    /* eva->next = new_cons(thunk, eva->next); */
    /* decrement_reference_counter(thunk); */
    /* decrement_reference_counter(eva->args); */
    /* eva->args = new_cons(expr, new_nil()); */
}

void add_macro(struct Thing *env, struct Eva *eva)
{
    if(env->type != &TYPES.cons)
    {
        puts("add_macro: env is no cons");
        exit(1);
    }
    struct Cons *env_cons = (struct Cons*)env->value;
    if(eva->args->type != &TYPES.cons)
    {
        puts("not enough arguments");
        exit(1);
    }
    struct Cons *args = (struct Cons*)eva->args->value;
    if(args->cdr->type != &TYPES.cons)
    {
        puts("not enough arguments");
        exit(1);
    }
    struct Cons *second_args = (struct Cons*)args->cdr->value;
    if(args->car->type != &TYPES.symbol)
    {
        puts("first argument must be a symbol");
        exit(1);
    }
    if(second_args->car->type != &TYPES.function)
    {
        puts("second argument must be a function");
        exit(1);
    }

    struct Thing *macros = env_cons->car;
    env_cons->car = new_cons(new_cons(args->car, second_args->car), macros);
    thing_track(macros, env_cons->car);

    update_args(new_nil(), eva);
}

void execute_function_or_macros(struct Thing *env, struct Eva *eva, struct Thing *(get)(struct Cons*), struct Thing *(build)(struct Cons*, struct Thing*))
{
    if(env->type != &TYPES.cons)
    {
        puts("cannot execute function with non cons env");
        exit(1);
    }
    struct Cons *cons = (struct Cons*)env->value;
    if(cons->car->type != &TYPES.cons)
    {
        puts("incorrect execute_function env");
        exit(1);
    }
    struct Thing *function_env = cons->cdr;
    if(function_env->type != &TYPES.cons)
    {
        puts("execute_function: function_env is no cons");
        exit(1);
    }
    struct Cons *function_env_cons = (struct Cons*)cons->cdr->value;
    struct Thing *function_env_fns = get(function_env_cons);
    struct Cons *pair = (struct Cons*)cons->car->value;
    struct Thing *parameter_list = pair->car;
    struct Thing *body = pair->cdr;
    if(!listp(body))
    {
        puts("incorrect lambda definition (body is no list)");
        exit(1);
    }

    struct Thing *args = eva->args;

    if(listp(parameter_list))
    {
        while(parameter_list->type == &TYPES.cons)
        {
            struct Cons *parameter_list_cons = (struct Cons*)parameter_list->value;
            if(args->type != &TYPES.cons)
            {
                puts("not enough arguments for function (no cons)");
                exit(1);
            }
            else if(parameter_list_cons->car->type != &TYPES.symbol)
            {
                puts("paramater in paramater list is no symbol");
                exit(1);
            }
            struct Cons *args_cons = (struct Cons*)args->value;
            struct Thing *symbol = parameter_list_cons->car;
            struct Thing *argument_pair = new_cons(symbol, args_cons->car);
            function_env_fns = new_cons(argument_pair, function_env_fns);

            parameter_list = parameter_list_cons->cdr;
            args = args_cons->cdr;
        }
        thing_track(function_env_cons->cdr, function_env_fns);
        if(args->type == &TYPES.cons)
        {
            puts("to many arguments for function");
            exit(1);
        }
    }
    else if(parameter_list->type == &TYPES.symbol)
    {
        struct Thing *symbol = parameter_list;
        struct Thing *argument_pair = new_cons(symbol, args);
        function_env_fns = new_cons(argument_pair, function_env_fns);
        thing_track(function_env_cons->cdr, function_env_fns);
    }
    else
    {
        puts("incorrect lambda definition (parameter list is no list)");
        exit(1);
    }

    struct Thing *new_function_env = build(function_env_cons, function_env_fns);//  new_cons(function_env_cons->car, function_env_fns);
    thing_track(body, new_function_env);

    struct Thing *def_and_env = new_cons(cons->car, new_function_env);
    push_next(new_function(execute_body, new_cons(body, new_function_env)), eva);
    push_next(new_function(macro_expand, def_and_env), eva);

    update_args(new_nil(), eva);
}

void execute_body(struct Thing *env, struct Eva *eva)
{
    if(env->type != &TYPES.cons)
    {
        puts("execute_body: env is no cons");
        exit(1);
    }
    struct Cons *env_cons = env->value;
    struct Thing *current = env_cons->car;
    struct Thing *new_function_env = env_cons->cdr;
    while(current->type == &TYPES.cons)
    {
        struct Cons *expr = (struct Cons*)current->value;
        struct Thing *thunk = new_function(progn, new_cons(expr->car, new_function_env));
        push_next(thunk, eva);
        current = expr->cdr;
    }
    update_args(new_nil(), eva);
}

struct Thing *execute_function_build_function_cons(struct Cons *env, struct Thing *fns)
{
    return new_cons(env->car, fns);
}

struct Thing *execute_function_get_function_cons(struct Cons *env)
{
    return env->cdr;
}

struct Thing *execute_function_build_macro_cons(struct Cons *env, struct Thing *macros)
{
    return new_cons(macros, env->cdr);
}

struct Thing *execute_function_get_macro_cons(struct Cons *env)
{
    return env->car;
}

void execute_function(struct Thing *env, struct Eva *eva)
{
    execute_function_or_macros(env, eva, execute_function_get_function_cons, execute_function_build_function_cons);
}

void execute_macros(struct Thing *env, struct Eva *eva)
{
    execute_function_or_macros(env, eva, execute_function_get_macro_cons, execute_function_build_macro_cons);
}

void macro_expander_update_expr(struct Thing *env, struct Eva *eva)
{
    if(env->type != &TYPES.cons)
    {
        puts("macro_expander_update_expr: env is no cons");
        exit(1);
    }
    if(eva->args->type != &TYPES.cons)
    {
        puts("macro_expander_update_expr: args are no cons");
        exit(1);
    }

    struct Cons *env_cons = (struct Cons*)env->value;
    struct Cons *args = (struct Cons*)eva->args->value;
    env_cons->car = args->car;
    update_args(new_nil(), eva);
}

void macro_expand_helper(struct Thing *env, struct Eva *eva)
{
    if(env->type != &TYPES.cons)
    {
        puts("macro_expand_helper: env is no cons");
        exit(1);
    }

    struct Cons *env_cons = (struct Cons*)env->value;
    struct Thing *macro = env_cons->car;
    struct Thing *expr = env_cons->cdr;
    if(macro->type != &TYPES.function)
    {
        puts("macro_expand_helper: macro is no function");
        exit(1);
    }
    if(expr->type != &TYPES.cons)
    {
        puts("macro_expand_helper: expr is no cons");
        exit(1);
    }
    struct Cons *expr_cons = (struct Cons*)expr->value;
    if(expr_cons->car->type != &TYPES.cons)
    {
        puts("macro_expand_helper: car(expr) is no cons");
        exit(1);
    }
    struct Cons *expr_cons2 = (struct Cons*)expr_cons->car->value;
    if(!listp(expr_cons2->cdr))
    {
        puts("macro_expand_helper: cdar(expr) is no list");
        exit(1);
    }

    push_next(new_function(macro_expander_update_expr, expr), eva);
    push_next(macro, eva);
    update_args(expr_cons2->cdr, eva);
}

void expand_args(struct Thing *args, struct Thing *macros, struct Eva *eva)
{
    while(args->type == &TYPES.cons)
    {
        struct Cons *args_cons = (struct Cons*)args->value;
        if(args_cons->car->type == &TYPES.cons)
        {
            struct Cons *arg = (struct Cons*)args_cons->car->value;
            if(arg->car->type == &TYPES.symbol)
            {
                const char *symbol = (const char*)arg->car->value;
                struct Thing *macro = assoc(symbol, macros);
                thing_track(args, macro);
                if(macro->type == &TYPES.cons)
                {
                    struct Cons *macro_cons = (struct Cons*)macro->value;
                    struct Thing *thunk = new_function(macro_expand_helper, new_cons(macro_cons->cdr, args));
                    struct Thing *further_expand = new_function(expand_further, new_cons(args, macros));
                    push_next(further_expand, eva);
                    push_next(thunk, eva);
                }
                else
                {
                    expand_args(arg->cdr, macros, eva);
                }
            }
            else
            {
                expand_args(arg->cdr, macros, eva);
            }
        }

        args = args_cons->cdr;
    }
}

void expand_further(struct Thing *env, struct Eva *eva)
{
    if(env->type != &TYPES.cons)
    {
        puts("expand_further: env is no cons");
        exit(1);
    }
    struct Cons *env_cons = (struct Cons*)env->value;
    struct Thing *args = env_cons->car;
    struct Thing *macros = env_cons->cdr;
    expand_args(args, macros, eva);
}

void macro_expand(struct Thing *env, struct Eva *eva)
{
    if(env->type != &TYPES.cons)
    {
        puts("macro_expand: env is no cons");
        exit(1);
    }

    struct Cons *env_cons = (struct Cons*)env->value;
    struct Thing *parameter_list_and_body = env_cons->car;
    struct Thing *function_env = env_cons->cdr;
    if(function_env->type != &TYPES.cons)
    {
        puts("macro_expand: function_env is no cons");
        exit(1);
    }
    struct Cons *function_env_cons = (struct Cons*)function_env->value;
    struct Thing *macros = function_env_cons->car;

    if(parameter_list_and_body->type != &TYPES.cons)
    {
        puts("macro_expand: no parameter list and body");
        exit(1);
    }

    struct Cons *parameter_list_and_body_cons = (struct Cons*)parameter_list_and_body->value;
    struct Thing *body = parameter_list_and_body_cons->cdr;

    struct Thing *current = body;

    while(current->type == &TYPES.cons)
    {
        struct Cons *expr = (struct Cons*)current->value;
        if(expr->car->type == &TYPES.cons)
        {
            struct Cons *expr_cons = (struct Cons*)expr->car->value;
            if(expr_cons->car->type == &TYPES.symbol)
            {
                const char *symbol = (const char*)expr_cons->car->value;
                // puts(string_of_thing(expr->car));
                // puts(symbol);
                struct Thing *macro = assoc(symbol, macros);
                thing_track(body, macro);
                if(macro->type == &TYPES.cons)
                {
                    struct Cons *macro_cons = (struct Cons*)macro->value;
                    struct Thing *thunk = new_function(macro_expand_helper, new_cons(macro_cons->cdr, current));
                    struct Thing *further_expand = new_function(expand_further, new_cons(body, macros));
                    push_next(further_expand, eva);
                    push_next(thunk, eva);
                }
                else
                {
                    expand_args(expr_cons->cdr, macros, eva);
                }
            }
            else
            {
                expand_args(expr_cons->cdr, macros, eva);
            }
        }
        current = expr->cdr;
    }
}

void eval_function_helper(struct Thing *env, struct Eva *eva)
{
    if(env->type != &TYPES.cons)
    {
        puts("eval_function_helper: env is no cons");
        exit(1);
    }
    struct Cons *env_cons = (struct Cons*)env->value;
    struct Thing *parameter_list_and_body = env_cons->car;
    struct Thing *function_env = env_cons->cdr;

    update_args(new_cons(new_function(execute_function, new_cons(parameter_list_and_body, function_env)), new_nil()), eva);
}

void eval_macros_helper(struct Thing *env, struct Eva *eva)
{
    if(env->type != &TYPES.cons)
    {
        puts("eval_macros_helper: env is no cons");
        exit(1);
    }
    struct Cons *env_cons = (struct Cons*)env->value;
    struct Thing *parameter_list_and_body = env_cons->car;
    struct Thing *function_env = env_cons->cdr;

    update_args(new_cons(new_function(execute_macros, new_cons(parameter_list_and_body, function_env)), new_nil()), eva);
}

void eval_function(struct Cons *cons, struct Thing *env, struct Eva *eva, void (*cc)(struct Thing*, struct Eva*))
{
    if(cons->cdr->type != &TYPES.cons)
    {
        puts("incorrect lambda / macros definition (no parameter list & body)");
        exit(1);
    }
    struct Cons *parameter_list_and_body = (struct Cons*)cons->cdr->value;
    if(!listp(parameter_list_and_body->cdr))
    {
        puts("invalid lambda / macros definition: body is no list");
        exit(1);
    }
    struct Thing *reversed_body = lst_reverse(parameter_list_and_body->cdr);
    struct Thing *def = new_cons(parameter_list_and_body->car, reversed_body);
    struct Thing *def_and_env = new_cons(def, env);
    push_next(new_function(cc, def_and_env), eva);
    // push_next(new_function(macro_expand, def_and_env), eva);
    
    update_args(new_nil(), eva);
}

void eval(struct Thing *env, struct Eva *eva)
{
    struct Cons *cons = (struct Cons*)eva->args->value;
    struct Thing *eval_me = cons->car; // first arg

    if(eval_me->type == &TYPES.symbol)
    {
        struct Thing *val = assoc((char*)eval_me->value, env);
        thing_track(eval_me, val);
        if(val->type != &TYPES.cons)
        {
            printf("undefined symbol: %s\n", (char*)eval_me->value);
            exit(1);
        }
        struct Cons *val_cons = val->value;
        update_args(new_cons(val_cons->cdr, new_nil()), eva);
        // decrement_reference_counter(eva->args);
        // eva->args = new_cons(resolve_symbol((char*)eval_me->value, env), new_nil()); // @todo throw error when symbol does not exist
    }
    else if(eval_me->type == &TYPES.cons)
    {
        if(((struct Cons*)eval_me->value)->car->type == &TYPES.symbol)
        {
            if(0 == strcmp("lambda", (char*)((struct Cons*)eval_me->value)->car->value))
            {
                eval_function((struct Cons*)eval_me->value, env, eva, eval_function_helper);
            }
            else if(0 == strcmp("macros", (char*)((struct Cons*)eval_me->value)->car->value))
            {
                eval_function((struct Cons*)eval_me->value, env, eva, eval_macros_helper);
            }
            else
            {
                eval_funcall((struct Cons*)eval_me->value, env, eva);
            }
        }
        else
        {
            eval_funcall((struct Cons*)eval_me->value, env, eva);
        }
    }
    else
    {
        update_args(new_cons(eval_me, new_nil()), eva);
        /* decrement_reference_counter(eva->args); */
        /* eva->args = new_cons(eval_me, new_nil()); */
    }
}

void chain_finish(struct Thing *env, struct Eva *eva)
{
    struct Thing *current = NULL;
    if(eva->args->type == &TYPES.cons)
    {
        struct Cons *cons = (struct Cons*)eva->args->value;
        current = cons->car;
    }
    else if(eva->args->type == &TYPES.nil)
    {
        current = new_nil();
    }
    else
    {
        puts("chain_finish: args is neither cons nor nil");
        exit(1);
    }

    update_args(new_cons(current, env), eva);
    // eva->args = new_cons(current, env);
}

void chain_helper(struct Thing *env, struct Eva *eva)
{
    if(env->type != &TYPES.cons)
    {
        puts("why got the chain_helper an invalid env...");
        exit(1);
    }
    struct Cons *cons = (struct Cons*)env->value;
    struct Thing *finish = new_function(chain_finish, eva->args);
    push_next(finish, eva);
    push_next(cons->car, eva);
    /* eva->next = new_cons(finish, eva->next); */
    /* eva->next = new_cons(cons->car, eva->next); */
    update_args(new_cons(cons->cdr, new_nil()), eva);
    // decrement_reference_counter(eva->args);
    // eva->args = new_cons(cons->cdr, new_nil());
}

struct Thing *chain(struct Thing *thunk, struct Thing *eval_me)
{
    struct Thing *env = new_cons(thunk, eval_me);
    struct Thing *f = new_function(chain_helper, env);

    return f; // free me
}

void eval_funcall_helper(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.cons)
    {
        puts("eval_funcall_helper: args are no cons");
        exit(1);
    }
    struct Cons *cons = (struct Cons*)eva->args->value;
    struct Thing *thunk = cons->car;
    if(thunk->type != &TYPES.function)
    {
        puts("trying to call non function");
        exit(1);
    }
    if(env->type != &TYPES.cons)
    {
        puts("eval_funcall_helper: wrong env given");
        exit(1);
    }
    struct Cons *env_cons = (struct Cons*)env->value;
    if(!listp(env_cons->car))
    {
        printf("no proper list for function call arguments...\n");
        exit(1);
    }

    push_next(thunk, eva);
    // eva->next = new_cons(thunk, eva->next);
    struct Thing *args = env_cons->car;

    while(args->type == &TYPES.cons)
    {
        struct Cons *cons = (struct Cons*)args->value;
        push_next(chain(new_function(eval, env_cons->cdr), cons->car), eva);
        // eva->next = new_cons(chain(new_function(eval, env_cons->cdr), cons->car), eva->next);
        args = cons->cdr;
    }

    update_args(new_nil(), eva);
    /* decrement_reference_counter(eva->args); */
    /* eva->args = new_nil(); */
}

void eval_funcall(struct Cons *funcall, struct Thing *env, struct Eva *eva)
{
    struct Thing *cc = new_function(eval_funcall_helper, new_cons(funcall->cdr, env));
    push_next(cc, eva);
    //eva->next = new_cons(cc, eva->next);
    push_next(new_function(eval, env), eva);
    // eva->next = new_cons(new_function(eval, env), eva->next);
    update_args(new_cons(funcall->car, new_nil()), eva);
    /* decrement_reference_counter(eva->args); */
    /* eva->args = new_cons(funcall->car, new_nil()); */
}

/* void test_fn(struct Thing *env, struct Eva *eva) */
/* { */
/*     eva.next = new_cons(conc_thunk, new_cons(test_fn2_thunk, eva.next)); */
/* } */

void protect(struct Thing *env, struct Eva *eva)
{
    char *error = string_of_thing(eva->args);
    printf("error: %s\n", error);
    free_memory(error, "protect: free string_of_thing");
    update_args(new_nil(), eva);
}

void eva_mark(struct Thing *thing)
{
    struct Eva *eva = (struct Eva*)thing->value;
    simple_mark(thing);
    thing_track(thing, eva->next);
    thing_track(thing, eva->args);
    thing_track(thing, eva->unwind);

    // puts(string_of_thing(eva->next));
    thing_mark(eva->next);
    thing_mark(eva->args);
    thing_mark(eva->unwind);
}

void eva_track(struct Thing *thing)
{
    struct Eva *eva = (struct Eva*)thing->value;
    thing_track(thing, eva->next);
    thing_track(thing, eva->args);
    thing_track(thing, eva->unwind);
}

void eval_loop(struct Thing *thunk, struct Thing *args, struct Pacman *pacman)
{
    struct Eva eva;
    struct Type eva_type = {"eva", 23, free_nil, eva_mark, eva_track};
    eva.next = new_cons(thunk, new_nil());
    eva.args = args;
    eva.unwind = new_cons(new_function(protect, new_nil()), new_nil());
    struct Thing *eva_thing = new_thing();
    eva_thing->value = &eva;
    eva_thing->type = &eva_type;
    struct Thing *old_root = pacman->root;
    pacman_set_root(pacman, new_cons(eva_thing, old_root));
    int eat_counter = 0;

    while(eva.next->type != &TYPES.nil)
    {
        struct Cons *cons = (struct Cons*)eva.next->value;

        if(cons->car->type != &TYPES.function)
        {
            printf("eval_loop: car is no function\n");
            puts(string_of_thing(cons->car));
            exit(1);
        }
        struct Function *fn = (struct Function*)cons->car->value;

        struct Thing *old = eva.next;
        eva.next = cons->cdr;

        fn->f(fn->env, &eva);

        if(eat_counter > 1000)
        {
            pacman_mark_and_eat(pacman);
            eat_counter = 0;
        }
        eat_counter++;
    }

    pacman_set_root(pacman, old_root);
}
