#ifndef STD_H
#define STD_H

#include "eval.h"
#include "sdl_lib.h"

void list(struct Thing *env, struct Eva *eva);
struct Thing *simple_entry(const char *name, void (*fn)(struct Thing*, struct Eva*));
struct Thing *push_std(struct Thing *car, struct Thing *lst);
struct Thing *build_std_fn_env();
struct Thing *build_std_macro_env();
struct Thing *build_std_env();

#endif
