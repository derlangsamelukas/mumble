(load
 "tier1.lisp"
 (let ((for-all-events
        (lambda (cc eve f x)
          ((if*
            sdl-event-poll
            (lambda (_) (cc eve f (f x)))
            (lambda (_) x))
           eve))))
   (cons
    '()
    (list
     (cons 'for-all-events (lambda (f x) (for-all-events for-all-events f x)))))))
