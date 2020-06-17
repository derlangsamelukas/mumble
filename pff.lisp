(lambda (lisp-dir project-dir)
  ((lambda (sexp if just not raise append)
     ((lambda (continue maybe-add-prefix)
        ((lambda (traverse)
           (traverse traverse sexp (cdr (build-std-env))))
         (lambda (self sexp state)
           (if (not (pair? sexp))
               (just state)
               (lambda ()
                 (if (not (pair? (car sexp)))
                     (lambda ()
                       (continue sexp state))
                     (lambda ()
                       (if (not (equal? 'use (car (car sexp))))
                           (lambda ()
                             (continue sexp state))
                           (lambda ()
                             (if (not (pair? (cdr (car sexp))))
                                 (lambda ()
                                   (raise "wrong usage of 'use (no proper list)"))
                                 (lambda ()
                                   (if (not (string? (car (cdr (car sexp)))))
                                       (lambda ()
                                         (raise "wrong usage of 'use (no string argument)"))
                                       (lambda ()
                                         (self
                                          self
                                          (cdr sexp)
                                          (append
                                           (maybe-add-prefix
                                            (cdr (cdr (car sexp)))
                                            (self
                                             self
                                             (read
                                              ((lambda (content)
                                                 (if (not (equal? content #f))
                                                     (just content)
                                                     (lambda ()
                                                       (read-file-as-string
                                                        (concat lisp-dir (concat (car (cdr (car sexp))) ".lisp"))))))
                                               (read-file-as-string
                                                (concat project-dir (concat (car (cdr (car sexp))) ".lisp")))))
                                             (cdr (build-std-env))))
                                           state))))))))))))))) ; traverse
      (lambda (sexp env)
        (eval
         (list
          (cons
           'lambda
           (cons
            (list)
            sexp)))
         (cons
          (car (build-std-env))
          env))) ; continue
      (lambda (expr state)
        state
        (if (not (pair? expr))
            (lambda ()
              state)
            (lambda ()
              (if (not (symbol? (car expr)))
                  (lambda ()
                    (raise "wrong usage of 'use (second argument must be a symbol)"))
                  (lambda ()
                    ((lambda (add-prefix)
                       (add-prefix add-prefix (symbol->string (car expr)) state))
                     (lambda (self prefix lst)
                       (if (not (pair? lst))
                           (just (list))
                           (lambda ()
                             (cons
                              (cons
                               (string->symbol
                                (concat
                                 prefix
                                 (symbol->string (car (car lst)))))
                               (cdr (car lst)))
                              (self self prefix (cdr lst)))))))))))))) ; maybe-add-prefix
   (read
    (read-file-as-string (concat project-dir "run.lisp"))) ; sexp
   (lambda (value true false)
     ((if* identity
           (lambda (_) (true))
           (lambda (_) (false)))
      value)) ; if
   (lambda (x) (lambda () x)) ; just
   (lambda (x) (equal? x #f)) ; not
   (lambda (string)
     (print "raise:" string)
     ("")) ; raise
   (lambda (lsta lstb)
     ((lambda (loop)
        (loop loop lsta))
      (lambda (loop lst)
        ((if*
          null?
          (lambda (_) lstb)
          (lambda (lst)
            (cons
             (car lst)
             (loop loop (cdr lst)))))
         lst)))))) ; append

