(use
 "tier1.lisp"
 (use
  "handler.lisp"
  (use
   "main-loop.lisp"
   (let ((pos-set (lambda (x pos) (cons pos (cdr x))))
         (pos-get car)
         (key-set (lambda (x key) (list (car x) key (car (cdr (cdr x))))))
         (key-get (o car cdr))
         (quit-set (lambda (x quit) (reverse (cons quit (cdr (reverse x))))))
         (quit-get (o car cdr cdr)))
     (let ((render!
            (lambda (x blender texture)
              (sdl-renderer-clear blender)
              (sdl-renderer-copy blender texture '() `(10 ,(pos-get x) 100 100))
              (sdl-renderer-present blender)))
           (idle
            (lambda (x)
              (if (key-get x)
                  (pos-set x (+ 5 (pos-get x)))
                  x)))
           (event-handler
            (lambda (x eve)
              (if (equal? *sdl-quit* (sdl-event-get-type eve))
                  (quit-set x #f)
                  (if (and (equal? *sdl-keydown* (sdl-event-get-type eve))
                           (equal? 100 (sdl-keysym eve)))
                      (key-set x #t)
                      (if (and (equal? *sdl-keyup* (sdl-event-get-type eve))
                               (equal? 100 (sdl-keysym eve)))
                          (key-set x #f)
                          x)))))
           (within
            (lambda (f x)
              (sdl-init)
              (sdl-img-init)
              (let ((win (sdl-create-window "Hejo" 100 100 500 500)))
                (let ((blender (sdl-create-renderer win))
                      (eve (sdl-create-event))
                      (identity (lambda (x) x)))
                  (main-loop quit-get (f blender eve) x)
                  (sdl-destroy-renderer blender)
                  (sdl-destroy-window win))))))
       (within
        (lambda (blender eve)
          (let ((duck (sdl-load-texture blender "Duck.jpg")))
            (lambda (x)
              (let ((x (idle (for-all-events eve event-handler x))))
                (render! x blender duck)
                x))))
        '(10 #f #t)))))))


