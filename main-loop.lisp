(use
 "tier1.lisp"
 (let ((mainloop
        (lambda (cc pred f a x)
          ;; (print x)
          (if (pred a)
              (cc cc pred f (f a) x ;;(+ 1 x)
                  )
              #f))))
   (cons
    '()
    (list
     (cons
      'main-loop
      (lambda (pred f a)
        (mainloop mainloop pred f a 0)))))))
