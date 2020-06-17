(define o (use "tier1.lisp" o))

(define (init)
  (sdl-init)
  (img-init))

(define (create-state textures)
  (list #t textures '()))

(define get-quit car)
(define get-textures (o car cdr))
(define get-keys (o car cdr cdr))

(define (render! state blender)
  (sdl-renderer-clear blender)
  (map
   (lambda (texture)
     (apply
      sdl-renderer-copy
      (cons
       blender
       texture)))
   (get-textures state))
  (sdl-renderer-present blender))

(define (idle state)
  state)

(define (handle-events state eve)
  state)
