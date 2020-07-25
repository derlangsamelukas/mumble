(use "export")
(print "12")
((lambda (fold)
   (export
    (-> 'fold fold)

    (-> 'if
        (lambda (value true false)
          ((if* identity
                (lambda (_) (true))
                (lambda (_) (false)))
           value)))

    (-> 'rec
        ((lambda (f)
           (lambda (fn x)
             (f f fn x)))
         (lambda (self fn x)
           (fn (lambda (x) (self self fn x)) x))))

    (-> 'assoc
        ((lambda (assoc)
           (lambda (key lst)
             (assoc assoc key lst)))
         (lambda (self key lst)
           ((if* (lambda (lst) (equal? #f (pair? lst)))
                 (lambda (_) #f)
                 (if* (lambda (lst)
                        (equal? (car (car lst)) key))
                      car
                      (lambda (lst)
                        (self self key (cdr lst)))))
            lst))))

    (-> 'o
        (lambda functions
          (lambda (x)
            (fold
             (lambda (x fn)
               (fn x))
             x
             (reverse functions)))))
    
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
        (lambda (x) (lambda _ x)))))
 
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
