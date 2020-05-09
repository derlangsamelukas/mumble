(load
 "tier1.lisp"
 (load
  "handler.lisp"
  (let ((o (lambda (f g) (lambda (x) (f (g x))))))
    (let ((cadr (o car cdr)))
      (sdl-init)
      (sdl-img-init)
      (let ((win (sdl-create-window "Hello" 100 100 500 500)))
        (let ((blender (sdl-create-renderer win))
              (eve (sdl-create-event))
              (type-quit 256))
          (let ((loop (lambda (loop x pred f)
                        ((if*
                          pred
                          (lambda (x) (loop loop (f x) pred f))
                          (lambda (_) #f))
                         x))))
            (loop
               loop
                 '()
                 (lambda (x) (null? (assoc 'quit x)))
                 (lambda (x)
                   (sdl-event-poll eve)
                   ((if*
                     (lambda (type) (equal? type type-quit))
                     (lambda (_) (cons '(quit #t) x))
                     (lambda (_)
                       (sdl-renderer-clear blender)
                       (sdl-renderer-present blender)))
                    (sdl-event-get-type eve)))))
          (sdl-destroy-renderer blender)
          (sdl-destroy-window win)))))))
