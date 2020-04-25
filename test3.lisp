(let ((name "lukas")
      (o (lambda (f g) (lambda (x) (f (g x))))))
  (let ((cadr (o car cdr)))
    '(2 3 ,(+ 2 3))))
