(lambda (file)
  ((lambda (load define-macro rec env)
     ((lambda (map define)
        ;; (print
        ;;  (define
        ;;      '((print "ey")
        ;;        (define (show name)
        ;;         (print name))
        ;;        (print "oy"))))
        (apply
         (load "continue.lisp"
               (cons
                (car env)
                (cons
                 (cons 'load load)
                 (cons
                  (cons 'map map)
                  (cons
                   (cons 'parse-define define)
                   (cons
                    (cons 'define-macro define-macro)
                    (cons
                     (cons 'rec rec)
                     (cdr env)))))))
               (lambda (x) x))
         (list file)))
      (load "map.lisp" env (lambda (x) x))
      (load "define.lisp" env (lambda (x) x))))
   (lambda (file env transform)
     (eval
      (list
       (cons
        'lambda
        (cons
         '()
         (transform
          (read
           (read-file-as-string file))))))
      env))
   (lambda (def body)
     ((lambda (var-val)
        (cons
         'let
         (cons
          (list (list (car var-val) (cons 'rec var-val)))
          body)))
      ((if*
        (lambda (def) (pair? (car def)))
        (lambda (_)
          (list
           (car (car def))
           (cons
            'lambda
            (cons
             (cdr (car def))
             (cdr def)))))
        (lambda (_)
          (list
           (car def)
           (car (cdr def)))))
       def)))
   (lambda (name def)
     (list
      '(lambda (f) (f f))
      (list
       'lambda
       (list name)
       (list
        'let
        (list (list name (list 'lambda 'args (list 'apply (list name name) 'args))))
        def))))
   (build-std-env)))
