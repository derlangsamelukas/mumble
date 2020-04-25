((lambda (eval-file)
   ((lambda (std-env)
      ((lambda (tier-one)
         ((lambda (append map)
            (print ((cdr (assoc 'quasiquote (car tier-one))) (list "aa")))
            (print
             (eval-file
              "test3.lisp"
              (cons
               (apply append (map car (list tier-one std-env)))
               (apply append (map cdr (list tier-one std-env)))))))
          (cdr (assoc 'append tier-one))
          (cdr (assoc 'map tier-one))))
       (eval-file "tier1.lisp" std-env)))
    (build-std-env)))
 (lambda (file env)
   (eval
    (list
     (cons
      'lambda
      (cons
       '()
       (read
        (read-file-as-string file)))))
    env)))
