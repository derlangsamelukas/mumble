(lambda (file)
  ((lambda (let-macro)
     ((macros
       (let)
       (let ((std-env (build-std-env)))
         (let ((std-env
                (cons
                 (cons (cons 'let let-macro) (cons (cons 'define define-macro) (cons (cons 'rec rec) (car std-env))))
                 (cons (cons 'load (lambda (file env) (load file env parse-define))) (cons (cons 'map map) (cdr std-env))))))
           (let ((use
                  (lambda (self)
                    (let ((env
                           (cons
                            (cons (cons 'use (lambda args (apply (self self) args))) (car std-env))
                            (cdr std-env))))
                      (lambda args
                        (let ((env (load (car args) env parse-define)))
                          (cons
                           (cons
                            'macros
                            (cons
                             (map car (car env))
                             (list
                              (cons
                               (cons
                                'lambda
                                (cons
                                 (map car (cdr env))
                                 (cdr args)))
                               (map cdr (cdr env))))))
                           (map cdr (car env)))))))))
             (load file
                   (cons (cons (cons 'use (use use)) (car std-env))
                         (cdr std-env))
                   parse-define)))))
      let-macro))
   (load "let.lisp"
         (cons
          (car (build-std-env))
          (cons
           (cons 'map map)
           (cdr (build-std-env))))
         (lambda (x) x))))
