(use "misc")

(print
 (misc:map
  (lambda (x)
    (+ x 1))
  (list 1 2 3 4)))
