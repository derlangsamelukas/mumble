#include "SDL.h"
#include "helper.h"
#include <stdio.h>

char* read_file (const char* filename)
{
  FILE* file = fopen(filename, "r");
  if(ptr_equals(file, NULL))
  {

    return NULL;
  }
  fseek(file, 0, SEEK_END);
  int length = ftell(file);
  fseek(file, 0, SEEK_SET);
  char* buffer = new_memory(multiply(sizeof(char), length), "read_file");
  fread(buffer, 1, length, file);
  fclose(file);

  return buffer;
}


void* popuate_symbol ()
{

  return NULL;
}


void* popuate_string ()
{

  return NULL;
}


void* popuate_list ()
{

  return NULL;
}


void* popuate_number ()
{

  return NULL;
}


void* popuate_whitespaces ()
{

  return NULL;
}


struct Thing* read_lisp (const char* content)
{
  int x = 0;
  x = 5;

  return NULL;
}


int run ()
{
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window* win = SDL_CreateWindow("huhu", 20, 20, 500, 500, SDL_WINDOW_RESIZABLE);
  SDL_Renderer* blender = SDL_CreateRenderer(win, -1, bit_or(SDL_RENDERER_ACCELERATED, SDL_RENDERER_PRESENTVSYNC));
  SDL_Event eve = create_sdl_event();
  while(1)
  {
    while(SDL_PollEvent(&eve))
    {
      if(equals(eve.type, SDL_QUIT))
      {

        return 0;
      }
    }
    SDL_RenderClear(blender);
    SDL_RenderPresent(blender);
  }
}

