((lambda (map)
   ((macros
     (let)
     (let ((name "lukas"))
       (print name)))
    (lambda args
      (cons
       (cons
        'lambda
        (cons
         (map car (car args))
         (cdr args)))
       (map
        (lambda (def) (car (cdr def)))
        (car args))))))
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
     lst))))
