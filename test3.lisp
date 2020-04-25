(let ((name "lukas")
      (o (lambda (f g) (lambda (x) (f (g x))))))
  (let ((cadr (o car cdr)))
    `(my name is ,(string->symbol name))))
