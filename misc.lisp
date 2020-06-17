(use "export")

((lambda (fold)
   (export
    (-> 'fold fold)
    
    (-> 'append
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

    (-> 'map
        (lambda (fn lst)
          (reverse
           (fold
            (lambda (lst elt)
              (cons (fn elt) lst))
            (list)
            lst))))
    
    (-> 'not
        (lambda (x)
          (equal? x #f)))
    
    (-> 'just
        (lambda (x) (lambda () x)))))
 
 (lambda (fn x lst)
   ((lambda (rec)
      (rec rec x lst))
    (lambda (cc x lst)
      ((if*
        pair?
        (lambda (_)
          (cc cc (fn x (car lst)) (cdr lst)))
        (lambda (_) x))
       lst))))) ; fold
