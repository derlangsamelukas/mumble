((lambda (eval-file)
   ((lambda (std-env)
      ((lambda (tier-one)
         ((lambda (append map)
            ((lambda (load)
               (eval-file
                "test3.lisp"
                (cons
                 (cons (cons 'load (lambda args (list load (car args) (list 'quote (cdr args))))) (car std-env))
                 (cdr std-env))))
             (lambda (file expr)
               ((lambda (ret)
                  (cons
                   (cons
                    'lambda
                    (cons
                     (map car (cdr ret))
                     expr))
                   (map cdr (cdr ret))))
                (eval-file file std-env))))
            ;; (print
            ;;  (eval-file
            ;;   "test3.lisp"
            ;;   (cons
            ;;    (apply append (map car (list tier-one std-env)))
            ;;    (apply append (map cdr (list tier-one std-env))))))
            )
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
