(import (matchable))
(import (chicken port))
(import (chicken irregex))
(import srfi-13)

(define (replace-minus symbol)
  ((o string->symbol list->string)
   (map
    (lambda (char) (if (char=? char #\-) #\_ char))
    ((o string->list symbol->string) symbol))))

(define (fancy-sdl-names symbol)
  (if (irregex-match "^sdl_.*$" (symbol->string symbol))
      ((o string->symbol (cut string-append "SDL_" <>) list->string reverse cdr)
       (foldl
        (lambda (prev char)
          (if (char=? char #\_)
              (cons char-upcase (cdr prev))
              (cons identity (cons ((car prev) char) (cdr prev)))))
        (cons identity '())
        ((o string->list (cut substring <> 3) symbol->string)
         symbol)))
      symbol))

(define (lisp2c-constants symbol)
  (if (irregex-match "^\\*.*\\*$" (symbol->string symbol))
      ((o string->symbol (cut string-trim-both <> #\*) string-upcase symbol->string) symbol)
      symbol))

(define (reformat expr)
  (cond
   ((pair? expr)
    (cons (reformat (car expr))
          (reformat (cdr expr))))
   ((symbol? expr)
    ((o lisp2c-constants
        fancy-sdl-names
        replace-minus)
     expr))
   (#t expr)))

(define indent-level (make-parameter 0))
(define (indent)
  (make-string (* 2 (indent-level)) (integer->char 32)))

(define (indent! add)
  (indent-level (+ (indent-level) add)))

(define (with-indention thunk)
  (parameterize ((indent-level (+ 1 (indent-level))))
    (thunk)))

(define (variable->string name type value)
  (string-append
   (indent)
   (apply string-append (intersperse (map symbol->string type) " "))
   " "
   (symbol->string name)
   " = "
   (unknown->string value)
   ";\n"))

(define (parameters->string parameters)
  (apply
   string-append
   (append
    '("(")
    (intersperse
     (map
      (lambda (parameter)
        (apply string-append (intersperse (map symbol->string (append (cdr parameter) (list (car parameter)))) " ")))
      parameters)
     ", ")
    '(")"))))

(define (unknown->string thing)
  (cond
   ((pair? thing) (function-call->string (car thing) (cdr thing)))
   ((symbol? thing) (symbol->string thing))
   ((number? thing) (number->string thing))
   ((string? thing) (with-output-to-string (lambda () (write thing))))
   ((boolean? thing) (if thing "1" "0"))))

(define (function-call->string fn args)
  (string-append
   (symbol->string fn)
   "("
   (apply string-append (intersperse (map unknown->string args) ", "))
   ")"))

(define control-structures '(if else while for))

(define (control-structure->string control-structure pred body)
  (string-append
   (indent)
   (symbol->string control-structure)
   "("
   (unknown->string pred)
   ")\n"
   (indent)
   "{\n"
   (body->string body)
   (indent)
   "}\n"))

(define (body->string body)
  (with-indention
   (lambda ()
     (apply
      string-append
      (map
       (lambda (expr)
         (match expr
           (((name . type) value)
            (variable->string name type value))
           (('set! name value)
            (string-append
             (indent)
             (symbol->string name)
             " = "
             (unknown->string value)
             ";\n"))
           (('return value)
            (string-append
             "\n"
             (indent)
             "return "
             (unknown->string value)
             ";\n"))
           ((fn . args)
            (if (member fn control-structures)
                (control-structure->string fn (car args) (cdr args))
                (string-append
                 (indent)
                 (function-call->string fn args)
                 ";\n")))))
       body)))))

(define (function->string name type parameters body)
  (string-append
   "\n"
   (apply string-append (intersperse (map symbol->string type) " "))
   " "
   (symbol->string name)
   " "
   (parameters->string parameters)
   "\n"
   (indent)
   "{\n"
   (body->string body)
   "}\n\n"))

(define (include->string include)
  (string-append
   "#include "
   (if (symbol? include)
       (symbol->string include)
       (string-append "\"" include "\""))
   "\n"))

(define (generate-code expr)
  (map
   (lambda (expr)
     (match expr
       (('include include)
        (display (include->string include)))
       (((name . type) value)
        (display (variable->string name type value)))
       (((name . type) parameters . body)
        (display (function->string name type parameters body)))))
   expr))

(define (read-all port previous)
  (let ((expr (read port)))
    (if (eof-object? expr)
        (reverse previous)
        (read-all port (cons expr previous)))))

(define (run)
  (let* ((port (open-input-file "main.lisp"))
         (expr  (read-all port '())))
    (close-input-port port)
    (with-output-to-file "src/generated.c"
      (lambda () ((o generate-code reformat) expr)))))

(run)
(exit)
