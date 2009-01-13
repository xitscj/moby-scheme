;; The first three lines of this file were inserted by DrScheme. They record metadata
;; about the language level of this file in a form that our tools can easily process.
#reader(lib "htdp-beginner-reader.ss" "lang")((modname falling-ball-pair) (read-case-sensitive #t) (teachpacks ((lib "world.ss" "teachpack" "htdp"))) (htdp-settings #(#t constructor repeating-decimal #f #t none #f ((lib "world.ss" "teachpack" "htdp")))))
;; Simple falling ball example, with structures.

;; A world is a coord representing the x,y position of the red ball.

(define WIDTH 100)
(define HEIGHT 100)
(define RADIUS 5)

(define-struct coord (x y))

(define (tick w)
  (make-coord (+ (coord-x w) 5)
              (+ (coord-y w) 5)))

(define (hits-floor? w)
  (>= (coord-y w) HEIGHT))

(define (draw-scene w)
  (place-image (circle RADIUS "solid" "red") 
               (coord-x w)
               (coord-y w)
               (empty-scene WIDTH HEIGHT)))

(big-bang WIDTH HEIGHT 1/15 (make-coord 0 0))
(on-tick-event tick)
(on-redraw draw-scene)
(stop-when hits-floor?)