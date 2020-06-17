(use
 "tier1.lisp"
 (let ((for-all-events
        (lambda (cc eve f x)
          (if (sdl-event-poll eve)
              (cc cc eve f (f x eve))
              x))))
   (cons
    '()
    (list
     (cons 'for-all-events (lambda (eve f x) (for-all-events for-all-events eve f x)))))))
