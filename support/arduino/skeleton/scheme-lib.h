#ifndef SCHEME_LIB_H
#define SCHEME_LIB_H

#include "scheme.h"

#ifdef __cplusplus
extern "C" {
#endif 

#define TRIG_FUN(name, fun) FUN(name);
#define COMPARE_FUN(name, val, c) FUN(name);
#define STRCMP_FUN(name, strcmp, c) FUN(name);

// null, empty, true, false, eof
extern val_t plt_EMPTY;
extern val_t plt_TRUE;
extern val_t plt_FALSE;
extern val_t plt_EOF;
// +, -, *, /
FUN(plt__plus_);
FUN(plt__dash_);
FUN(plt__star_);
FUN(plt__slash_);
// >=, >, <=, <, =, =~
COMPARE_FUN(plt__greaterthan__equal_, number_val, >=)
COMPARE_FUN(plt__greaterthan_, number_val, >)
COMPARE_FUN(plt__lessthan__equal_, number_val, <=)
COMPARE_FUN(plt__lessthan_, number_val, >)
COMPARE_FUN(plt__equal_, number_val, ==)
FUN(plt__equal__tilde_);
// number->string
FUN(plt_number_dash__greaterthan_string);
// even?, odd?, positive?, negative?
FUN(plt_even_question_);
FUN(plt_odd_question_);
FUN(plt_positive_question_);
FUN(plt_negative_question_);
// number?, rational?, integer?, real?
FUN(plt_number_question_);
FUN(plt_rational_question_);
FUN(plt_integer_question_);
FUN(plt_real_question_);
// quotient, remainder
FUN(plt_quotient);
FUN(plt_remainder);
// numerator, denominator
FUN(plt_numerator);
FUN(plt_denominator);
// abs, acos, asin, atan, random, max, min, sqr, sqrt
TRIG_FUN(plt_abs, fabs)
TRIG_FUN(plt_acos, acos)
TRIG_FUN(plt_asin, asin)
TRIG_FUN(plt_atan, atan)
FUN(plt_random);
FUN(plt_max);
FUN(plt_min);
FUN(plt_sqr);
TRIG_FUN(plt_sqrt, sqrt)
// modulo, add1, sub1, zero?
FUN(plt_modulo);
FUN(plt_add1);
FUN(plt_sub1);
FUN(plt_zero_question_);
// exp, expt, sgn, log
TRIG_FUN(plt_exp, exp)
FUN(plt_expt);
FUN(plt_sgn);
TRIG_FUN(plt_log, log)
// gcd, lcm
// TODO
// round, floor, ceiling
TRIG_FUN(plt_round, round)
TRIG_FUN(plt_floor, floor)
TRIG_FUN(plt_ceiling, ceil)
// sin, cos, tan, sinh, cosh
TRIG_FUN(plt_sin, sin)
TRIG_FUN(plt_cos, cos)
TRIG_FUN(plt_tan, tan)
TRIG_FUN(plt_sinh, sinh)
TRIG_FUN(plt_cosh, cosh)
// angle, conjugate, magnitude
FUN(plt_angle);
FUN(plt_conjugate);
TRIG_FUN(plt_magnitude, fabs)
// exact->inexact
FUN(plt_exact_dash__greaterthan_inexact);
//
// Logic
//
// not, false?, boolean?, boolean=?
FUN(plt_not);
// what's the difference b/w this and not?
FUN(plt_false_question_);
FUN(plt_boolean_question_);
FUN(plt_boolean_equal__question_);
//
// Characters
//
// char?
FUN(plt_char_question_);
// char=?, char<?, char<=?, char>?, char>=?
COMPARE_FUN(plt_char_equal__question_, character_val, ==)
COMPARE_FUN(plt_char_lessthan__question_, character_val, >)
COMPARE_FUN(plt_char_lessthan__equal__question_, character_val, <=)
COMPARE_FUN(plt_char_greaterthan__question_, character_val, >)
COMPARE_FUN(plt_char_greaterthan__equal__question_, character_val, >=)
// char-ci=?, char-ci<?, char-ci<=?, char-ci>?, char-ci>=?
// does this work?
COMPARE_FUN(plt_char_dash_ci_equal__question_, ci_character_val, ==)
COMPARE_FUN(plt_char_dash_ci_lessthan__question_, ci_character_val, >)
COMPARE_FUN(plt_char_dash_ci_lessthan__equal__question_, ci_character_val, <=)
COMPARE_FUN(plt_char_dash_ci_greaterthan__question_, ci_character_val, >)
COMPARE_FUN(plt_char_dash_ci_greaterthan__equal__question_, ci_character_val, >=)
// char-downcase, char-lower-case?, char-numeric?, char-upcase, char-upper-case?, char-whitespace?, char-alphabetic?
FUN(plt_char_dash_downcase);
FUN(plt_char_dash_lower_dash_case_question_);
FUN(plt_char_dash_numeric_question_);
FUN(plt_char_dash_upcase);
FUN(plt_char_dash_upper_dash_case_question_);
FUN(plt_char_dash_whitespace_question_);
FUN(plt_char_dash_alphabetic_question_);
// char->integer, integer->char
FUN(plt_char_dash__greaterthan_integer);
FUN(plt_integer_dash__greaterthan_char);
//
// SYMBOLS
//
// symbol=?, symbol->string, symbol?
FUN(plt_symbol_equal__question_);
FUN(plt_symbol_dash__greaterthan_string);
FUN(plt_symbol_question_);
//
// STRINGS
//
// string=?, string?
STRCMP_FUN(plt_string_equal__question_, strcmp, ==)
FUN(plt_string_question_);
// string>?, string>=?, string<?, string<=?
STRCMP_FUN(plt_string_greaterthan__question_, strcmp, >)
STRCMP_FUN(plt_string_greaterthan__equal__question_, strcmp, >=)
STRCMP_FUN(plt_string_lessthan__question_, strcmp, <)
STRCMP_FUN(plt_string_lessthan__equal__question_, strcmp, <=)
// string-ci<=?, string-ci<?, string-ci>=?, string-ci>?
STRCMP_FUN(plt_string_dash_ci_lessthan__equal__question_, strcasecmp, <=)
STRCMP_FUN(plt_string_dash_ci_lessthan__question_, strcasecmp, <)
STRCMP_FUN(plt_string_dash_ci_greaterthan__equal__question_, strcasecmp, >=)
STRCMP_FUN(plt_string_dash_ci_greaterthan__question_, strcasecmp, >)
// substring
FUN(plt_substring);
// string-length, string-ref, string-copy
FUN(plt_string_dash_length);
FUN(plt_string_dash_ref);
FUN(plt_string_dash_copy);
// string->number, string->list, string->symbol
FUN(plt_string_dash__greaterthan_number);
FUN(plt_string_dash__greaterthan_list);
FUN(plt_string_dash__greaterthan_symbol);
// string-append, list->string, make-string, string
FUN(plt_string_dash_append);
FUN(plt_list_dash__greaterthan_string);
FUN(plt_make_dash_string);
FUN(plt_string);
//
// PAIRS
//
// empty?, first, second, third, fourth, fifth, sixth, seventh, eighth
FUN(plt_empty_question_);
FUN(plt_first);
FUN(plt_second);
FUN(plt_third);
FUN(plt_fourth);
FUN(plt_fifth);
FUN(plt_sixth);
FUN(plt_seventh);
FUN(plt_eighth);
// rest, cons, pair?, cons?, null?
FUN(plt_rest);
FUN(plt_cons);
FUN(plt_pair_question_);
FUN(plt_cons_question_);
FUN(plt_null_question_);
// length, list, list*, list-ref
FUN(plt_length);
FUN(plt_list);
FUN(plt_list_star_);
FUN(plt_list_dash_ref);
// append
FUN(plt_append);
// member, memq, memv
FUN(plt_member);
FUN(plt_memq);
FUN(plt_memv);
// reverse
FUN(plt_reverse);
// c(a|d)+r
FUN(plt_caaar);
FUN(plt_caadr);
FUN(plt_caar);
FUN(plt_cadar);
FUN(plt_cadddr);
FUN(plt_caddr);
FUN(plt_cadr);
FUN(plt_car);
FUN(plt_cdaar);
FUN(plt_cdadr);
FUN(plt_cdar);
FUN(plt_cddar);
FUN(plt_cdddr);
FUN(plt_cddr);
FUN(plt_cdr);
//
// POSN
//
FUN(plt_make_dash_posn);
FUN(plt_posn_dash_x);
FUN(plt_posn_dash_y);
FUN(plt_posn_question_);
//
// EOF
//
FUN(plt_eof_dash_object_question_);
//
// MISC
//
// equal?, eq?, eqv?, equal~?
FUN(plt_equal_question_);
FUN(plt_eq_question_);
FUN(plt_eqv_question_);
FUN(plt_equal_tilde_);
// error
FUN(plt_error);
// struct?
FUN(plt_struct_question_);
// identity
FUN(plt_identity);
// current-seconds
FUN(plt_current_dash_seconds);

#ifdef __cplusplus
}
#endif

#endif
