(lambda args
  (cons
   (cons
    'lambda
    (cons
     (map car (car args))
     (cdr args)))
   (map
    (lambda (def)
      (car (cdr def)))
    (car args))))
