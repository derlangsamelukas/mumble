#ifndef EVAL_H
#define EVAL_H

#include "types.h"
#include "helper.h"

void raiseit(struct Thing *message, struct Eva *eva);
void push_next(struct Thing *next, struct Eva *eva);
void update_args(struct Thing *args, struct Eva *eva);
struct Thing *assoc(const char *symbol, struct Thing *thing);
void expand_further(struct Thing *env, struct Eva *eva);
void eval(struct Thing *env, struct Eva *eva);
void eval_funcall(struct Cons *funcall, struct Thing *env, struct Eva *eva);
void print_(struct Thing *env, struct Eva *eva);
void execute_function(struct Thing *env, struct Eva *eva);
void eval_loop(struct Thing *thunk, struct Thing *args, struct Pacman *pacman);
void macro_expand(struct Thing *env, struct Eva *eva);
void execute_body(struct Thing *env, struct Eva *eva);

#endif
