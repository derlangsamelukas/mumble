((lambda (define)
   (lambda (x) (define define x)))
 (lambda (cc args)
   ((lambda (helper)
      (helper helper '() args))
    (lambda (helper prev next)
      (reverse
       ((if*
         null?
         (lambda (_) prev)
         (lambda (_)
           ((if*
             (lambda (x) x)
             (lambda (_)
               (cons
                (list 'define (cdr (car next)) (cc cc (cdr next)))
                ;; (cons 'define (cons (cdr (car next)) (cons (cc cc (cdr next)) '())))
                prev))
             (lambda (_)
               (helper helper (cons (car next) prev) (cdr next))))
            ((if*
              (lambda (next) (pair? (car next)))
              (lambda (next) (equal? (car (car next)) 'define))
              (lambda (_) #f))
             next))))
        next))))))
