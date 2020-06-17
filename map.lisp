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
