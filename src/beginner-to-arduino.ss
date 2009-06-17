#lang scheme/base

;; This program translates beginner-level languages into Arduino source.
;; We pattern match against the language given by:
;;
;; http://docs.plt-scheme.org/htdp-langs/beginner.html


(require scheme/match
         scheme/list
         scheme/string
         scheme/contract
         "env.ss"
         "toplevel-arduino.ss"
         "pinfo-arduino.ss"
         "helpers.ss")


;; A compiled program is a:
(define-struct compiled-program
  (defns           ;; (listof string)
    toplevel-exprs ;; (listof string)
    pinfo          ;; pinfo
    ))

; copy & pasted here because I don't feel like figuring out provide/contract type syntax
;; mapi: (a -> b) -> list[a] -> list[b]
(define (mapi f l) (mapi_ f 0 l))
(define (mapi_ f i l)
  (cond
    [(null? l) '()]
    [(pair? l) (cons (f i (car l)) (mapi_ f (+ i 1) (cdr l)))]))



;; program->arduino-string: program -> compiled-program
;; Consumes a program and returns a compiled program.
(define (program->arduino-string program)
  (let* ([a-pinfo (program-analyze program)]
         [toplevel-env (pinfo-env a-pinfo)])
    
    (let loop ([program program]
               [decls ""]
               [defns ""]
               [tops ""])
      (cond [(empty? program)
             (make-compiled-program (string-append decls "\n" defns) tops a-pinfo)]
            [else
             (cond [(defn? (first program))
                    (let-values ([(decl-string defn-string expr-string)
                                  (definition->arduino-strings 
                                           (first program) 
                                           toplevel-env)])
                                         
                    (loop (rest program)
                          (string-append decls
                                         "\n"
                                         decl-string)
                          (string-append defns
                                         "\n"
                                         defn-string)
                          (string-append tops
                                         "\n"
                                         expr-string)))]
                   
                   [(test-case? (first program))
                    (loop (rest program)
                          decls
                          (string-append defns
                                         "\n"
                                         "// Test case erased\n")
                          tops)]
                   
                   [(library-require? (first program))
                    (loop (rest program)
                          decls
                          (string-append defns
                                         "\n"
                                         "// Module require erased\n")
                          tops)] 
                   
                   [(expression? (first program))
                    (loop (rest program)
                          decls
                          defns
                          (string-append tops
                                         "\nIGNORE("
                                         (expression->arduino-string 
                                          (first program) 
                                          toplevel-env)
                                         ");"))])]))))



;; definition->arduino-strings: definition env -> (values string string string)
;; Consumes a definition (define or define-struct) and produces three strings.
;; The first is a declaration.
;; The second maps a definitions string.
;; The third value is the expression that will be evaluated at the toplevel.
;;
;; Structure definitions map to static inner classes with transparent fields.
(define (definition->arduino-strings defn env)
  (match defn
    [(list 'define (list fun args ...) body)
     (values (function-declaration->arduino-string fun)
             (function-definition->arduino-string fun args body env)
             "")]
    [(list 'define (? symbol? fun) (list 'lambda (list args ...) body))
     (values (function-declaration->arduino-string fun)
             (function-definition->arduino-string fun args body env)
             "")]
    [(list 'define (? symbol? id) body)
     (variable-definition->arduino-strings id body env)]

    [(list 'define-struct id (list fields ...))
     (values (struct-definition->arduino-string id fields env)
             ""
             "")]))


;; function-declaration->arduino-string: symbol -> string
;; Converts the function declaration into a static function declaration.
(define (function-declaration->arduino-string fun)
  (let* ([munged-fun-id
          (identifier->munged-arduino-identifier fun)])
    (format "static FUN(~a);" munged-fun-id)))

;; function-definition->arduino-string: symbol (listof symbol) expr env -> string
;; Converts the function definition into a static function declaration whose
;; return value is an object.
(define (function-definition->arduino-string fun args body env)
  (let* ([munged-fun-id
          (identifier->munged-arduino-identifier fun)]
         [munged-arg-ids
          (map identifier->munged-arduino-identifier args)]
         [new-env 
          (env-extend env (make-binding:function fun #f (length args) #f
                                                 (symbol->string munged-fun-id)
                                                 empty))]
         [new-env
          (foldl (lambda (arg-id env) 
                   (env-extend env (make-binding:constant arg-id 
                                                          (symbol->string
                                                           (identifier->munged-arduino-identifier arg-id))
                                                          empty)))
                 new-env
                 args)])
    (format "static FUN(~a) { START(); val_t ~a; RETURN(~a); }"
            munged-fun-id
            (string-join (mapi (lambda (i arg-id)
                                (format "~a = ARG(~a)" (symbol->string arg-id) i))
                              munged-arg-ids)
                         ", ")
            (expression->arduino-string body new-env))))


;; variable-definition->arduino-strings: symbol expr env -> (values string string)
;; Converts the variable definition into a static variable declaration and its
;; initializer at the toplevel.
(define (variable-definition->arduino-strings id body env)
  (let* ([munged-id (identifier->munged-arduino-identifier id)]
         [new-env (env-extend env (make-binding:constant id 
                                                         (symbol->string munged-id)
                                                         empty))])
    (values (format "static val_t ~a; "
                    munged-id)
            ""
            (format "~a = ref(~a);" 
                    munged-id
                    (expression->arduino-string body new-env)))))


;; struct-definition->arduino-string: symbol (listof symbol) env -> string
(define (struct-definition->arduino-string id fields env)
  ;; field->accessor-name: symbol symbol -> symbol
  ;; Given a structure name and a field, return the accessor.
  (define (field->accessor-name struct-name field-name)
    (string->symbol
     (string-append (symbol->string struct-name)
                    "-"
                    (symbol->string field-name))))
  
  (format "~a\n~a\n~a"
          ;; make-id
          (format "static FUN(~a) { START(); RETURN(alloc_struct(~s, ~a, (val_t []) { ~a })); }"
                  (let ([make-id (string->symbol 
                                  (string-append "make-" (symbol->string id)))])
                    (identifier->munged-arduino-identifier  make-id))
                  (symbol->string id)
                  (length fields)
                  (string-join (build-list (length fields) (lambda (i) (format "ARG(~a)" i)))
                               ", "))
          
          ;; accessors
          (string-join 
           (mapi (lambda (i a-field)
                  (format "static FUN(~a) { START(); RETURN(elem_struct(struct_val(ARG(0)), ~a)); }"
                          (identifier->munged-arduino-identifier (field->accessor-name id a-field))
                          i))
                fields)
           "\n")
          
          ;; predicate
          (format "static FUN(~a) { START(); RETURN(alloc_boolean(strcmp(type_struct(struct_val(ARG(0))), ~s) == 0)); }"
                  (identifier->munged-arduino-identifier (string->symbol (format "~a?" id)))
                  (symbol->string id))))



;; expression->arduino-string: expr env -> string
;; Translates an expression into an Arduino expression string whose evaluation
;; should produce a var_t.
(define (expression->arduino-string expr env)
  (match expr
    [(list 'cond [list questions answers] ... [list 'else answer-last])
     (let loop ([questions questions]
                [answers answers])
       (cond
         [(empty? questions)
          (expression->arduino-string answer-last env)]
         [else
          (format "boolean_val(~a) ? (~a) : (~a)"
                  (expression->arduino-string (first questions) env)
                  (expression->arduino-string (first answers) env)
                  (loop (rest questions) (rest answers)))]))]
    
    
    [(list 'cond [list questions answers] ... [list question-last answer-last])
     (let loop ([questions questions]
                [answers answers])
       (cond
         [(empty? questions)
          (format "boolean_val(~a) ? (~a) : error(\"Fell out of cond\")"
                  (expression->arduino-string question-last env)
                  (expression->arduino-string answer-last env))]
         [else
          (format "boolean_val(~a) ? (~a) : (~a)"
                  (expression->arduino-string (first questions) env)
                  (expression->arduino-string (first answers) env)
                  (loop (rest questions) (rest answers)))]))]
    
    [(list 'if test consequent alternative)
     (format "boolean_val(~a) ? (~a) : (~a)"
             (expression->arduino-string test env)
             (expression->arduino-string consequent env)
             (expression->arduino-string alternative env))]
    
    [(list 'and expr ...)
     (string-append "alloc_boolean(true && "
                    (string-join (map (lambda (e)
                                        (format "boolean_val(~a)"
                                                (expression->arduino-string e env)))
                                      expr) 
                                 "&&")
                    ")")]
    
    [(list 'or expr ...)
     (string-append "alloc_boolean(false || "
                    (string-join (map (lambda (e)
                                        (format "boolean_val(~a)"
                                                (expression->arduino-string e env)))
                                      expr) 
                                 "||")
                    ")")]
    
    ;; Numbers
    [(? number?)
     (number->arduino-string expr)]
    
    ;; Strings
    [(? string?)
     (format "alloc_string(~s)" expr)]
    
    ;; Characters  TODO
;    [(? char?)
;     (string-append "(new Character(\""
;                    (if (char=? expr #\") "\\" (string expr))
;                    "\"))")]
    
    ;; Identifiers
    [(? symbol?)
     (identifier-expression->arduino-string expr env)]
    
    ;; Quoted symbols  TODO
    [(list 'quote datum)
     (format "alloc_symbol(\"~a\")"
             datum)]
    
    ;; Function call/primitive operation call
    [(list (? symbol? id) exprs ...)
     (application-expression->arduino-string id exprs env)]))



;; application-expression->arduino-string: symbol (listof expr) env -> string
;; Converts the function application to a string.
(define (application-expression->arduino-string id exprs env)
  (let ([operator-binding (env-lookup env id)]
        [operand-strings 
         (map (lambda (e) 
                             (expression->arduino-string e env))
                           exprs)])
    (match operator-binding
      ['#f
       (error 'application-expression->arduino-string
              "Moby doesn't know about ~s" id)]
      
      [(struct binding:constant (name arduino-string permissions))
       (format "CALL(function_val(~a), ~a~a)" 
               arduino-string
               (length operand-strings)
               (if (empty? operand-strings)
                   ""
                   (string-append ", " (string-join operand-strings ", "))))]
      
      [(struct binding:function (name module-path min-arity var-arity? 
                                      arduino-string permissions))
       (unless (>= (length exprs)
                   min-arity)
         (error 'application-expression->arduino-string
                "Minimal arity of ~s not met.  Operands were ~s"
                id
                exprs))
       (format "CALL(~a, ~a~a)" 
               arduino-string
               (length operand-strings)
               (if (empty? operand-strings)
                   ""
                   (string-append ", " (string-join operand-strings ", "))))])))



;; identifier-expression->arduino-string: symbol -> string
;; Translates the use of a toplevel identifier to the appropriate
;; arduino code.
(define (identifier-expression->arduino-string an-id an-env)
  (match (env-lookup an-env an-id)
    ['#f
     (error 'translate-toplevel-id "Moby doesn't know about ~s." an-id)]
    [(struct binding:constant (name arduino-string permissions))
     arduino-string]
    [(struct binding:function (name module-path min-arity var-arity? arduino-string permissions))
     (format "alloc_function(~a)" arduino-string)]))
;     (error 'translate-toplevel-id
;              "Moby doesn't yet allow higher-order use of ~s." an-id)]))




;; number->arduino-string: number -> string
(define (number->arduino-string a-num)
  (cond [(integer? a-num)
         ;; TODO: we need to handle exact/vs/inexact issue.
         ;; We probably need the numeric tower.
         (format "alloc_number(~a)" a-num)]
        [(real? a-num)
         (format "alloc_number(~a)" a-num)]
        [else
         (error 'number->java-string "Don't know how to handle ~s yet" a-num)]))




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;





(provide/contract [struct compiled-program ([defns 
                                              string?]
                                            [toplevel-exprs 
                                             string?]
                                            [pinfo pinfo?])]
                  
                  [program->arduino-string 
                   (program? . -> . compiled-program?)]
                  
                  [expression->arduino-string 
                   (expression? env? . -> . string?)])
