((lambda (map append)
   ((lambda (let-macro quasiquote std-env)
      (cons
       (list (cons 'let let-macro)
             (cons 'quasiquote quasiquote))
       (list (cons 'map map)
             (cons 'append append))))
    (lambda args
      (cons (cons 'lambda (cons (map car (car args)) (cdr args)))
            (map (lambda (def) (car (cdr def))) (car args))))
    (lambda (x)
      ((if*
         (lambda (x) (equal? "cons" (typeof x)))
         (if*
          (lambda (lst) (equal? 'unquote (car lst)))
          (lambda (lst) (car (cdr lst)))
          (lambda (lst) (cons 'list (cons (list 'quasiquote (car lst)) (map (lambda (x) (list 'quasiquote x)) (cdr lst))))))
         (lambda (x) (list 'quote x)))
       x))
    (build-std-env)))
 ((lambda (map)
    (lambda (f lst)
      (map map f lst)))
  (lambda (map f lst)
    ((if* null?
          (lambda (lst) (list))
          (lambda (lst)
            (cons
             (f (car lst))
             (map map f (cdr lst)))))
     lst)))
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
       lst)))))
