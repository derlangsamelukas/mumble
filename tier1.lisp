(let ((fold
       (lambda (fn x lst)
         (let ((rec
                (lambda (cc x lst)
                  ((if*
                     pair?
                     (lambda (_)
                       (cc cc (fn x (car lst)) (cdr lst)))
                     (lambda (_) x))
                   lst))))
           (rec rec x lst)))))
  (let ((append
         (lambda (lsta lstb)
           (let ((loop
                    (lambda (loop lst)
                      ((if*
                        null?
                        (lambda (_) lstb)
                        (lambda (lst)
                          (cons
                           (car lst)
                           (loop loop (cdr lst)))))
                       lst))))
             (loop loop lsta))))
        (quasiquote
         (if*
          pair?
          (if*
           (lambda (lst) (equal? 'unquote (car lst)))
           (lambda (lst) (car (cdr lst)))
           (lambda (lst) (cons 'list (cons (list 'quasiquote (car lst)) (map (lambda (x) (list 'quasiquote x)) (cdr lst))))))
          (lambda (x) (list 'quote x))))
        (export
         (lambda args
           (cons
            (map (lambda (fn) (list 'cons (list 'quote fn) fn)) ((if* pair? (lambda (x) (car x)) (lambda () '())) (cdr args)))
            (map (lambda (macro) (list 'cons (list 'quote macro) macro)) (car args)))))
        (define-macro
         (lambda (def body)
           (let ((var-val
                  (if (pair? (car (car def)))
                      (list
                       ((o car car car) def)
                       (cons
                        'lambda
                        (cons
                         ((o cdr car car) def)
                         ((o cdr car) def))))
                      (list
                       ((o car car) def)
                       ((o car cdr car) def)))))
             (cons
              'let
              (cons
               (list var-val)
               body)))))
        (ignore-args (lambda (fn) (lambda _ (fn))))
        (not (lambda (x) (equal? #f x)))
        (o (lambda args
             (lambda (x)
               (fold (lambda (x f) (f x)) x (reverse args))))))
    ((macros
      (quasiquote)
      (let ((if-macro
             (lambda args
               `((if*
                  identity
                  (,ignore-args (lambda () ,(car (cdr args))))
                  (,ignore-args (lambda () ,(car (cdr (cdr args))))))
                 ,(car args))))
            (when-macro
             (lambda args
               `(if ,(car args)
                    (,(cons 'lambda (cons '() (cdr args))))
                    #f)))
            (unless-macro
             (lambda args
               (cons
                'when
                (cons
                 (list 'not (car args))
                 (cdr args)))))
            (and-macro
             (lambda (a b)
               `(if ,a ,b #f))))
        (cons
         (list (cons 'quasiquote quasiquote)
               (cons 'if if-macro)
               (cons 'when when-macro)
               (cons 'unless unless-macro)
               (cons 'and and-macro)
               (cons 'export export)
               (cons 'define define-macro))
         (list (cons 'append append)
               (cons 'not not)
               (cons 'fold fold)
               (cons 'o o)))))
     quasiquote)))
