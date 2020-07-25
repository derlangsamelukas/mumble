#ifndef SDL_LIB_H
#define SDL_LIB_H

#include "SDL.h"
#include "SDL_image.h"
#include "types.h"
#include "eval.h"

struct Thing *add_sdl_lib(struct Thing *fn_env, struct Environment *environment);

#endif
