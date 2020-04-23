#include "sdl_lib.h"

#define ARG(s, x) do {if(x->type != &TYPES.cons) { printf("%s: not enough arguments\n", s); exit(1); }} while(0)
#define ARG_END(s, x) do {if(x->type != &TYPES.nil) { printf("%s: too many arguments\n", s); exit(1); }} while(0)
#define TYPE(s, x, t) do {if(x->type != &TYPES.t){ printf("%s: invalid type", s); exit(1); }} while(0)
#define CUSTOM_TYPE(s, x, t) do {if(x->type != &t){ printf("%s: invalid type", s); exit(1); }} while(0)
#define INT(x) *((int*)x->value)

void free_event(struct Thing *thing)
{
    free_memory(thing->value, "free_event (value)");
    free_memory(thing, "free_event");
}

void free_texture(struct Thing *thing)
{
    SDL_DestroyTexture(thing->value);
    free_memory(thing, "free_texture");
}


struct Type sdl_window_type = {"SDL_Window", 78976, free_nil, simple_mark, simple_track};
struct Type sdl_renderer_type = {"SDL_Renderer", 78977, free_nil, simple_mark, simple_track};
struct Type sdl_event_type = {"SDL_Event", 78978, free_event, simple_mark, simple_track};
struct Type sdl_texture_type = {"SDL_Texture", 78979, free_texture, simple_mark, simple_track};

void sdl_init_(struct Thing *env, struct Eva *eva)
{
    if(eva->args->type != &TYPES.nil)
    {
        puts("sdl-init: too many arguments");
        exit(1);
    }
    SDL_Init(SDL_INIT_EVERYTHING);
    update_args(new_nil(), eva);
}

void sdl_image_init(struct Thing *env, struct Eva *eva)
{
    ARG_END("sdl_image_init", eva->args);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    update_args(new_cons(new_nil(), new_nil()), eva);
}

void create_window(struct Thing *env, struct Eva *eva)
{
    ARG("create_window", eva->args);
    struct Cons *first_arg = eva->args->value;
    ARG("create_window", first_arg->cdr);
    struct Cons *second_arg = first_arg->cdr->value;
    ARG("create_window", second_arg->cdr);
    struct Cons *third_arg = second_arg->cdr->value;
    ARG("create_window", third_arg->cdr);
    struct Cons *fourth_arg = third_arg->cdr->value;
    ARG("create_window", fourth_arg->cdr);
    struct Cons *fifth_arg = fourth_arg->cdr->value;
    ARG_END("create_window", fifth_arg->cdr);

    TYPE("create_window", first_arg->car, string);
    TYPE("create_window", second_arg->car, integer);
    TYPE("create_window", third_arg->car, integer);
    TYPE("create_window", fourth_arg->car, integer);
    TYPE("create_window", fifth_arg->car, integer);

    SDL_Window *win = SDL_CreateWindow(first_arg->car->value, INT(second_arg->car), INT(third_arg->car), INT(fourth_arg->car), INT(fifth_arg->car), SDL_WINDOW_RESIZABLE);
    if(NULL == win)
    {
        update_args(new_cons(new_bool(0), new_nil()), eva);
        return;
    }
    struct Thing *sdl_window = new_thing();
    sdl_window->value = win;
    sdl_window->type = &sdl_window_type;

    update_args(new_cons(sdl_window, new_nil()), eva);
}

void sdl_delay_(struct Thing *env, struct Eva *eva)
{
    SDL_Delay(5000);
    update_args(new_nil(), eva);
}

void create_renderer(struct Thing *env, struct Eva *eva)
{
    ARG("create_renderer", eva->args);
    struct Cons *args = eva->args->value;
    ARG_END("create_window", args->cdr);
    CUSTOM_TYPE("create_renderer", args->car, sdl_window_type);
    SDL_Renderer *raw_blender = SDL_CreateRenderer(args->car->value, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(NULL == raw_blender)
    {
        update_args(new_cons(new_bool(0), new_nil()), eva);
        return;
    }
    struct Thing *blender = new_thing();
    blender->value = raw_blender;
    blender->type = &sdl_renderer_type;
    
    update_args(new_cons(blender, new_nil()), eva);
}

void destroy_renderer(struct Thing *env, struct Eva *eva)
{
    ARG("destroy_renderer", eva->args);
    struct Cons *args = eva->args->value;
    ARG_END("destroy_renderer", args->cdr);
    CUSTOM_TYPE("destroy_renderer", args->car, sdl_renderer_type);

    SDL_DestroyRenderer(args->car->value);

    update_args(new_cons(new_nil(), new_nil()), eva);
}

void destroy_window(struct Thing *env, struct Eva *eva)
{
    ARG("destroy_window", eva->args);
    struct Cons *args = eva->args->value;
    ARG_END("destroy_window", args->cdr);
    CUSTOM_TYPE("destroy_window", args->car, sdl_window_type);

    SDL_DestroyWindow(args->car->value);

    update_args(new_cons(new_nil(), new_nil()), eva);
}

void renderer_clear(struct Thing *env, struct Eva *eva)
{
    ARG("renderer_clear", eva->args);
    struct Cons *args = eva->args->value;
    ARG_END("renderer_clear", args->cdr);
    CUSTOM_TYPE("renderer_clear", args->car, sdl_renderer_type);

    SDL_RenderClear(args->car->value);

    update_args(new_cons(new_nil(), new_nil()), eva);
}

void renderer_present(struct Thing *env, struct Eva *eva)
{
    ARG("renderer_present", eva->args);
    struct Cons *args = eva->args->value;
    ARG_END("renderer_present", args->cdr);
    CUSTOM_TYPE("renderer_present", args->car, sdl_renderer_type);

    SDL_RenderPresent(args->car->value);

    update_args(new_cons(new_nil(), new_nil()), eva);
}

void create_event(struct Thing *env, struct Eva *eva)
{
    ARG_END("create_event", eva->args);
    struct Thing *eve = new_thing();
    eve->value = new_memory(sizeof(SDL_Event), "SDL_Event");
    eve->type = &sdl_event_type;

    update_args(new_cons(eve, new_nil()), eva);
}

void event_poll(struct Thing *env, struct Eva *eva)
{
    ARG("event_poll", eva->args);
    struct Cons *args = eva->args->value;
    ARG_END("event_poll", args->cdr);

    CUSTOM_TYPE("event_poll", args->car, sdl_event_type);

    update_args(new_cons(new_bool(SDL_PollEvent(args->car->value)), new_nil()), eva);
}

void event_type(struct Thing *env, struct Eva *eva)
{
    ARG("event_type", eva->args);
    struct Cons *args = eva->args->value;
    ARG_END("event_type", args->cdr);

    CUSTOM_TYPE("event_type", args->car, sdl_event_type);

    SDL_Event *eve = args->car->value;

    update_args(new_cons(new_integer(eve->type), new_nil()), eva);
}

void load_texture(struct Thing *env, struct Eva *eva)
{
    ARG("load_texture", eva->args);
    struct Cons *args = eva->args->value;
    ARG("load_texture", args->cdr);
    struct Cons *second_arg = args->cdr->value;
    ARG_END("load_texture", second_arg->cdr);

    CUSTOM_TYPE("load_texture", args->car, sdl_renderer_type);
    TYPE("load_texture", second_arg->car, string);
    SDL_Texture *raw_texture = IMG_LoadTexture(args->car->value, second_arg->car->value);
    if(NULL == raw_texture)
    {
        update_args(new_cons(new_bool(0), new_nil()), eva);
        return;
    }
    struct Thing *texture = new_thing();
    texture->value = raw_texture;
    texture->type = &sdl_texture_type;

    update_args(new_cons(texture, new_nil()), eva);
}

void fill_rect(SDL_Rect *rect, struct Thing *lst)
{
    TYPE("render_copy", lst, cons);
    struct Cons *lst_cons = lst->value;
    TYPE("render_copy", lst_cons->car, integer);
    rect->x = INT(lst_cons->car);
    lst = lst_cons->cdr;
    
    TYPE("render_copy", lst, cons);
    lst_cons = lst->value;
    TYPE("render_copy", lst_cons->car, integer);
    rect->y = INT(lst_cons->car);
    lst = lst_cons->cdr;

    TYPE("render_copy", lst, cons);
    lst_cons = lst->value;
    TYPE("render_copy", lst_cons->car, integer);
    rect->w = INT(lst_cons->car);
    lst = lst_cons->cdr;

    TYPE("render_copy", lst, cons);
    lst_cons = lst->value;
    TYPE("render_copy", lst_cons->car, integer);
    rect->h = INT(lst_cons->car);
}

void render_copy(struct Thing *env, struct Eva *eva)
{
    ARG("render_copy", eva->args);
    struct Cons *args = eva->args->value;
    ARG("render_copy", args->cdr);
    struct Cons *second_arg = args->cdr->value;
    ARG("render_copy", second_arg->cdr);
    struct Cons *third_arg = second_arg->cdr->value;
    ARG("render_copy", third_arg->cdr);
    struct Cons *fourth_arg = third_arg->cdr->value;
    ARG_END("render_copy", fourth_arg->cdr);

    CUSTOM_TYPE("render_copy", args->car, sdl_renderer_type);
    CUSTOM_TYPE("render_copy", second_arg->car, sdl_texture_type);

    SDL_Rect src_;
    SDL_Rect dest_;
    SDL_Rect *src = NULL;
    SDL_Rect *dest = NULL;
    if(third_arg->car->type != &TYPES.nil)
    {
        fill_rect(&src_, third_arg->car);
        src = &src_;
    }
    if(fourth_arg->car->type != &TYPES.nil)
    {
        fill_rect(&src_, fourth_arg->car);
        dest = &dest_;
    }
    
    SDL_RenderCopy(args->car->value, second_arg->car->value, src, dest);
    update_args(new_cons(new_bool(0), new_nil()), eva);
}

struct Thing *add_sdl_lib(struct Thing *fn_env)
{
    struct Thing *env = new_cons(new_cons(new_symbol("sdl-init"), new_function(sdl_init_, new_nil())), fn_env);
    env = new_cons(new_cons(new_symbol("sdl-create-window"), new_function(create_window, new_nil())), env);
    env = new_cons(new_cons(new_symbol("sdl-delay"), new_function(sdl_delay_, new_nil())), env);
    env = new_cons(new_cons(new_symbol("sdl-create-renderer"), new_function(create_renderer, new_nil())), env);
    env = new_cons(new_cons(new_symbol("sdl-destroy-window"), new_function(destroy_window, new_nil())), env);
    env = new_cons(new_cons(new_symbol("sdl-destroy-renderer"), new_function(destroy_renderer, new_nil())), env);
    env = new_cons(new_cons(new_symbol("sdl-create-event"), new_function(create_event, new_nil())), env);
    env = new_cons(new_cons(new_symbol("sdl-event-poll"), new_function(event_poll, new_nil())), env);
    env = new_cons(new_cons(new_symbol("sdl-renderer-clear"), new_function(renderer_clear, new_nil())), env);
    env = new_cons(new_cons(new_symbol("sdl-renderer-present"), new_function(renderer_present, new_nil())), env);
    env = new_cons(new_cons(new_symbol("sdl-event-get-type"), new_function(event_type, new_nil())), env);
    env = new_cons(new_cons(new_symbol("sdl-renderer-copy"), new_function(render_copy, new_nil())), env);
    env = new_cons(new_cons(new_symbol("sdl-load-texture"), new_function(load_texture, new_nil())), env);
    env = new_cons(new_cons(new_symbol("sdl-img-init"), new_function(sdl_image_init, new_nil())), env);

    return env;
}
