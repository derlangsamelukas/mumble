(include "SDL.h")
(include "helper.h")
(include <stdio.h>)

((read-file char*) ((filename const char*))
 ((file FILE*) (fopen filename "r"))
 (if (ptr-equals file *null*)
     (return *null*))
 (fseek file 0 *seek-end*)
 ((length int) (ftell file))
 (fseek file 0 *seek-set*)
 ((buffer char*) (malloc (multiply (sizeof char) length)))
 (fread buffer 1 length file)
 (fclose file)
 (return buffer))

((popuate-symbol void*) ()
 (return NULL))

((popuate-string void*) ()
 (return NULL))

((popuate-list void*) ()
 (return NULL))

((popuate-number void*) ()
 (return NULL))

((popuate-whitespaces void*) ()
 (return NULL))

((read-lisp struct Thing*) ((content const char*))
 ((x int) 0)
 (set! x 5)
 (return *null*)
 )

((run int) ()
 (sdl-init *sdl-init-everything*)
 ((win sdl-window*) (sdl-create-window "huhu" 20 20 500 500 *sdl-window-resizable*))
 ((blender sdl-renderer*) (sdl-create-renderer win -1 (bit-or *sdl-renderer-accelerated* *sdl-renderer-presentvsync*)))
 ((eve sdl-event) (create-sdl-event))
 (while #t
   (while (sdl-poll-event &eve)
     (if (equals eve.type *sdl-quit*)
         (return 0)))
   (sdl-render-clear blender)
   (sdl-render-present blender)))
