
;; (print "huch")

((lambda (map)
   ;; (print (map list '(1 2 3)))
   (add-macro
    'quasiquote
    (if*
     (lambda (x) (equal? "cons" (typeof x)))
     (if*
      (lambda (lst) (equal? 'unquote (car lst)))
      (lambda (lst) (car (cdr lst)))
      (lambda (lst) (cons 'list (cons (list 'quasiquote (car lst)) (map (lambda (x) (list 'quasiquote x)) (cdr lst))))))
     (lambda (x) (list 'quote x))))
   ((lambda ()
      (add-macro
       'let
       (lambda args
         (cons (cons 'lambda (cons (map car (car args)) (cdr args)))
               (map (lambda (def) (car (cdr def))) (car args)))))
      (print `(() "huhu" ,(list `(1 ,(cons 22 33) 4) 2)))
      ((lambda ()
         (let ((name "lukas")
               (age 23))
           (print age)
           (print name)))))))
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
