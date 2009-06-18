#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "scheme.h"

// hack: declare this here so we don't tie ourselves to a particular system
unsigned long millis(void);

#define TRIG_FUN(name, fun) FUN(name) { START(); RETURN(alloc_number(fun(number_val(ARG(0))))); }
#define COMPARE_FUN(name, val, c) FUN(name) { START(); for (struct_size_t i = 0; i < ARGC() - 1; i++) \
	if (!(val(ARG(i)) c val(ARG(i + 1)))) RETURN(alloc_boolean(false)); \
	RETURN(alloc_boolean(true)); }
#define STRCMP_FUN(name, strcmp, c) FUN(name) { START(); for (struct_size_t i = 0; i < ARGC() - 1; i++) \
	if (!(strcmp(string_val(ARG(i)), string_val(ARG(i + 1))) c 0)) RETURN(alloc_boolean(false)); \
	RETURN(alloc_boolean(true)); }

// null, empty, true, false, eof

val_t plt_EMPTY = & (struct val) { STATIC(1), NIL };
val_t plt_TRUE = & (struct val) { STATIC(1), BOOLEAN, (union data) true };
val_t plt_FALSE = & (struct val) { STATIC(1), BOOLEAN, (union data) false };
val_t plt_EOF = & (struct val) { STATIC(1), NIL };


// +, -, *, /

FUN(plt__plus_) {
	START();
	float ret = 0;
	for (struct_size_t i = 0; i < ARGC(); i++)
		ret += number_val(ARG(i));
	RETURN(alloc_number(ret));
}

FUN(plt__dash_) {
	START();
	float ret = number_val(ARG(0));
	for (struct_size_t i = 1; i < ARGC(); i++)
		ret -= number_val(ARG(i));
	RETURN(alloc_number(ret));
}

FUN(plt__star_) {
	START();
	float ret = 1;
	for (struct_size_t i = 0; i < ARGC(); i++)
		ret *= number_val(ARG(i));
	RETURN(alloc_number(ret));
}

FUN(plt__slash_) {
	START();
	float ret = number_val(ARG(0));
	for (struct_size_t i = 1; i < ARGC(); i++)
		ret /= number_val(ARG(i));
	RETURN(alloc_number(ret));
}

// >=, >, <=, <, =, =~

COMPARE_FUN(plt__greaterthan__equal_, number_val, >=)
COMPARE_FUN(plt__greaterthan_, number_val, >)
COMPARE_FUN(plt__lessthan__equal_, number_val, <=)
COMPARE_FUN(plt__lessthan_, number_val, >)
COMPARE_FUN(plt__equal_, number_val, ==)

FUN(plt__equal__tilde_) {
	START();
	RETURN(alloc_boolean(abs(number_val(ARG(0)) - number_val(ARG(1))) <= number_val(ARG(2))));
}

// number->string

FUN(plt_number_dash__greaterthan_string) {
	START();
	// TODO
	RETURN(alloc_string("FIXME"));
}

// even?, odd?, positive?, negative?

FUN(plt_even_question_) {
	START();
	RETURN(alloc_boolean(fmod(number_val(ARG(0)), 2) == 0));
}

FUN(plt_odd_question_) {
	START();
	RETURN(alloc_boolean(fabs(fmod(number_val(ARG(0)), 2)) == 1));
}

FUN(plt_positive_question_) {
	START();
	RETURN(alloc_boolean(number_val(ARG(0)) > 0));
}

FUN(plt_negative_question_) {
	START();
	RETURN(alloc_boolean(number_val(ARG(0)) < 0));
}

// number?, rational?, integer?, real?

FUN(plt_number_question_) {
	START();
	RETURN(alloc_boolean(type_val(ARG(0)) == NUMBER));
}

FUN(plt_rational_question_) {
	START();
	RETURN(alloc_boolean(false));
}

FUN(plt_integer_question_) {
	START();
	// TODO: can we pass NULL to modf here?
	RETURN(alloc_boolean(type_val(ARG(0)) == NUMBER && modf(number_val(ARG(0)), NULL) == 0));
}

FUN(plt_real_question_) {
	START();
	RETURN(alloc_boolean(type_val(ARG(0)) == NUMBER));
}

// quotient, remainder

FUN(plt_quotient) {
	START();
	double q;
	modf(number_val(ARG(0)) / number_val(ARG(1)), &q);
	RETURN(alloc_number(q));
}

FUN(plt_remainder) {
	START();
	RETURN(alloc_number(fmod(number_val(ARG(0)), number_val(ARG(1)))));
}

// numerator, denominator

FUN(plt_numerator) {
	START();
	RETURN(alloc_boolean(false));
}

FUN(plt_denominator) {
	START();
	RETURN(alloc_boolean(false));
}

// abs, acos, asin, atan, random, max, min, sqr, sqrt

TRIG_FUN(plt_abs, fabs)
TRIG_FUN(plt_acos, acos)
TRIG_FUN(plt_asin, asin)
TRIG_FUN(plt_atan, atan)

FUN(plt_random) {
	START();
	RETURN(alloc_number(rand() % ((int) number_val(ARG(0)))));
}

FUN(plt_max) {
	START();
	float ret = number_val(ARG(0));
	for (struct_size_t i = 1; i < ARGC(); i++)
		ret = fmax(ret, number_val(ARG(i)));
	RETURN(alloc_number(ret));
}

FUN(plt_min) {
	START();
	float ret = number_val(ARG(0));
	for (struct_size_t i = 1; i < ARGC(); i++)
		ret = fmin(ret, number_val(ARG(i)));
	RETURN(alloc_number(ret));
}

FUN(plt_sqr) {
	START();
	RETURN(alloc_number(number_val(ARG(0)) * number_val(ARG(0))));
}

TRIG_FUN(plt_sqrt, sqrt)

// modulo, add1, sub1, zero?

FUN(plt_modulo) {
	START();
	double r = fmod(number_val(ARG(0)), number_val(ARG(1)));
	if (r < 0) r += number_val(ARG(1));
	RETURN(alloc_number(r));
}

FUN(plt_add1) {
	START();
	RETURN(alloc_number(number_val(ARG(0)) + 1));
}

FUN(plt_sub1) {
	START();
	RETURN(alloc_number(number_val(ARG(0)) - 1));
}

FUN(plt_zero_question_) {
	START();
	RETURN(alloc_boolean(number_val(ARG(0)) == 0));
}

// exp, expt, sgn, log

TRIG_FUN(plt_exp, exp)

FUN(plt_expt) {
	START();
	RETURN(alloc_number(pow(number_val(ARG(0)), number_val(ARG(1)))));
}

FUN(plt_sgn) {
	START();
	float n = number_val(ARG(0));
	RETURN(alloc_number((n > 0) ? 1 : (n < 0) ? -1 : 0));
}

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

FUN(plt_angle) {
	START();
	float n = number_val(ARG(0));
	RETURN(alloc_number((n > 0) ? 0 : (n < 0) ? M_PI : NAN));
}

FUN(plt_conjugate) {
	START();
	RETURN(ARG(0));
}

TRIG_FUN(plt_magnitude, fabs)

// exact->inexact

FUN(plt_exact_dash__greaterthan_inexact) {
	START();
	RETURN(ARG(0));
}

//
// Logic
//

// not, false?, boolean?, boolean=?

FUN(plt_not) {
	START();
	RETURN(alloc_boolean(!boolean_val(ARG(0))));
}

// what's the difference b/w this and not?
FUN(plt_false_question_) {
	START();
	RETURN(alloc_boolean(!boolean_val(ARG(0))));
}

FUN(plt_boolean_question_) {
	START();
	RETURN(alloc_boolean(type_val(ARG(0)) == BOOLEAN));
}

FUN(plt_boolean_equal__question_) {
	START();
	RETURN(alloc_boolean(boolean_val(ARG(0)) == boolean_val(ARG(1))));
}

//
// Characters
//

// char?

FUN(plt_char_question_) {
	START();
	RETURN(alloc_boolean(type_val(ARG(0)) == CHARACTER));
}

// char=?, char<?, char<=?, char>?, char>=?

COMPARE_FUN(plt_char_equal__question_, character_val, ==)
COMPARE_FUN(plt_char_lessthan__question_, character_val, >)
COMPARE_FUN(plt_char_lessthan__equal__question_, character_val, <=)
COMPARE_FUN(plt_char_greaterthan__question_, character_val, >)
COMPARE_FUN(plt_char_greaterthan__equal__question_, character_val, >=)

// char-ci=?, char-ci<?, char-ci<=?, char-ci>?, char-ci>=?

// does this work?
#define ci_character_val(x) tolower(character_val(x))
COMPARE_FUN(plt_char_dash_ci_equal__question_, ci_character_val, ==)
COMPARE_FUN(plt_char_dash_ci_lessthan__question_, ci_character_val, >)
COMPARE_FUN(plt_char_dash_ci_lessthan__equal__question_, ci_character_val, <=)
COMPARE_FUN(plt_char_dash_ci_greaterthan__question_, ci_character_val, >)
COMPARE_FUN(plt_char_dash_ci_greaterthan__equal__question_, ci_character_val, >=)

// char-downcase, char-lower-case?, char-numeric?, char-upcase, char-upper-case?, char-whitespace?, char-alphabetic?

FUN(plt_char_dash_downcase) {
	START();
	RETURN(alloc_character(tolower(character_val(ARG(0)))));
}

FUN(plt_char_dash_lower_dash_case_question_) {
	START();
	RETURN(alloc_boolean(islower(character_val(ARG(0)))));
}

FUN(plt_char_dash_numeric_question_) {
	START();
	RETURN(alloc_boolean(isdigit(character_val(ARG(0)))));
}

FUN(plt_char_dash_upcase) {
	START();
	RETURN(alloc_character(toupper(character_val(ARG(0)))));
}

FUN(plt_char_dash_upper_dash_case_question_) {
	START();
	RETURN(alloc_boolean(isupper(character_val(ARG(0)))));
}

FUN(plt_char_dash_whitespace_question_) {
	START();
	// TODO: could be wrong
	RETURN(alloc_boolean(isspace(character_val(ARG(0)))));
}

FUN(plt_char_dash_alphabetic_question_) {
	START();
	RETURN(alloc_boolean(isalpha(character_val(ARG(0)))));
}

// char->integer, integer->char

FUN(plt_char_dash__greaterthan_integer) {
	START();
	RETURN(alloc_number((int) character_val(ARG(0))));
}

FUN(plt_integer_dash__greaterthan_char) {
	START();
	RETURN(alloc_character((int) number_val(ARG(0))));
}

//
// Symbols
//

// symbol=?, symbol->string, symbol?

FUN(plt_symbol_equal__question_) {
	START();
	RETURN(alloc_boolean(symbol_val(ARG(0)) == symbol_val(ARG(1))));
}

FUN(plt_symbol_dash__greaterthan_string) {
	START();
	RETURN(alloc_string(symbol_val(ARG(0))));
}

FUN(plt_symbol_question_) {
	START();
	RETURN(alloc_boolean(type_val(ARG(0)) == SYMBOL));
}

//
// Strings
//

// string=?, string?

STRCMP_FUN(plt_string_equal__question_, strcmp, ==)

FUN(plt_string_question_) {
	START();
	RETURN(alloc_boolean(type_val(ARG(0)) == STRING));
}

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

FUN(plt_substring) {
	START();
	int s = number_val(ARG(1)), e = number_val(ARG(2));
	char *ret = malloc(e - s + 1);
	strncpy(ret, string_val(ARG(0)) + s, e - s);
	ret[e - s] = '\0';
	RETURN(alloc_string_nocopy(ret));
}

// string-length, string-ref, string-copy

FUN(plt_string_dash_length) {
	START();
	RETURN(alloc_number(strlen(string_val(ARG(0)))));
}

FUN(plt_string_dash_ref) {
	START();
	RETURN(alloc_character(string_val(ARG(0))[(int) number_val(ARG(1))]));
}

FUN(plt_string_dash_copy) {
	START();
	RETURN(alloc_string(string_val(ARG(0))));
}

// string->number, string->list, string->symbol

FUN(plt_string_dash__greaterthan_number) {
	START();
	// TODO
	RETURN(alloc_number(0 /*strtof(string_val(ARG(0)), NULL)*/));
}

FUN(plt_string_dash__greaterthan_list) {
	START();
	const char *s = string_val(ARG(0));
	val_t list = alloc_nil();
	for (int i = strlen(s); i > 0; i--)
		list = alloc_cons(alloc_character(s[i-1]), list);
	RETURN(list);
}

FUN(plt_string_dash__greaterthan_symbol) {
	START();
	// FIXME: memory leak (symbols are never freed)
	// ...redesign how symbols are stored?
	RETURN(alloc_symbol(strdup(string_val(ARG(0)))));
}

// string-append, list->string, make-string, string

FUN(plt_string_dash_append) {
	START();
	
	int len = 0;
	
	for (int i = 0; i < ARGC() - 1; i++)
		len += strlen(string_val(ARG(i)));
	
	char *ret = malloc(len + 1);
	len = 0;
	
	for (int i = 0; i < ARGC() - 1; i++) {
		strcpy(ret + len, string_val(ARG(i)));
		len += strlen(string_val(ARG(i)));
	}
	
	RETURN(alloc_string_nocopy(ret));
}

FUN(plt_list_dash__greaterthan_string) {
	START();
	
	int len = 0;
	for (val_t list = ARG(0); type_val(list) != NIL; list = cdr(list))
		len++;
	
	char *ret = malloc(len + 1);
	len = 0;

	for (val_t list = ARG(0); type_val(list) != NIL; list = cdr(list))
		ret[len++] = character_val(car(list));
	ret[len] = '\0';
	
	RETURN(alloc_string_nocopy(ret));
}

FUN(plt_make_dash_string) {
	START();
	
	int n = number_val(ARG(0));
	char *ret = malloc(n + 1);
	memset(ret, character_val(ARG(1)), n);
	ret[n] = '\0';
	
	RETURN(alloc_string_nocopy(ret));
}

FUN(plt_string) {
	START();
	RETURN(alloc_string((char []){ character_val(ARG(0)), '\0' }));
}

//
// PAIRS
//

// empty?, first, second, third, fourth, fifth, sixth, seventh, eighth

FUN(plt_empty_question_) {
	START();
	RETURN(alloc_boolean(type_val(ARG(0)) == NIL));
}

FUN(plt_first) {
	START();
	RETURN(car(ARG(0)));
}

FUN(plt_second) {
	START();
	RETURN(car(cdr(ARG(0))));
}

FUN(plt_third) {
	START();
	RETURN(car(cdr(cdr(ARG(0)))));
}

FUN(plt_fourth) {
	START();
	RETURN(car(cdr(cdr(cdr(ARG(0))))));
}

FUN(plt_fifth) {
	START();
	RETURN(car(cdr(cdr(cdr(cdr(ARG(0)))))));
}

FUN(plt_sixth) {
	START();
	RETURN(car(cdr(cdr(cdr(cdr(cdr(ARG(0))))))));
}

FUN(plt_seventh) {
	START();
	RETURN(car(cdr(cdr(cdr(cdr(cdr(cdr(ARG(0)))))))));
}

FUN(plt_eighth) {
	START();
	RETURN(car(cdr(cdr(cdr(cdr(cdr(cdr(cdr(ARG(0))))))))));
}

// rest, cons, pair?, cons?, null?

FUN(plt_rest) {
	START();
	RETURN(cdr(ARG(0)));
}

FUN(plt_cons) {
	START();
	RETURN(alloc_cons(ARG(0), ARG(1)));
}

FUN(plt_pair_question_) {
	START();
	RETURN(alloc_boolean(size_struct(struct_val(ARG(0))) == 2));
}

FUN(plt_cons_question_) {
	START();
	RETURN(alloc_boolean(size_struct(struct_val(ARG(0))) == 2));
}

FUN(plt_null_question_) {
	START();
	RETURN(alloc_boolean(type_val(ARG(0)) == NIL));
}

// length, list, list*, list-ref

FUN(plt_length) {
	START();
	int len = 0;
	for (val_t l = ARG(0); type_val(l) != NIL; l = cdr(l))
		len++;
	RETURN(alloc_number(len));
}

FUN(plt_list) {
	START();
	val_t list = alloc_nil();
	for (int i = ARGC(); i > 0; i--)
		list = alloc_cons(ARG(i-1), list);
	RETURN(list);
}

FUN(plt_list_star_) {
	START();
	val_t list = ARG(ARGC() - 1);
	for (int i = ARGC() - 1; i > 0; i--)
		list = alloc_cons(ARG(i-1), list);
	RETURN(list);
}

FUN(plt_list_dash_ref) {
	START();
	val_t list = ARG(0);
	for (int i = number_val(ARG(1)); i > 0; i--)
		list = cdr(list);
	RETURN(car(list));
}

// append

FUN(plt_append) {
	START();
	
	val_t list = alloc_nil();
	for (int i = 0; i < ARGC(); i++)
		for (val_t l = ARG(i); type_val(l) != NIL; l = cdr(l))
			list = alloc_cons(car(l), list);
	
	val_t ret = alloc_nil();
	for (list = ref(list); type_val(list) != NIL; REREF(list, cdr(list)))
		ret = alloc_cons(car(list), ret);
	
	RETURN(deref_nofree(ret));
}

// member, memq, memv

FUN(plt_member) {
	START();
	
	for (val_t l = ARG(1); type_val(l) != NIL; l = cdr(l))
		if (_equal(ARG(0), car(l))) RETURN(alloc_boolean(true));
	
	RETURN(alloc_boolean(false));
}

FUN(plt_memq) {
	START();
	
	for (val_t l = ARG(1); type_val(l) != NIL; l = cdr(l))
		if (_eq(ARG(0), car(l))) RETURN(alloc_boolean(true));
	
	RETURN(alloc_boolean(false));
}

FUN(plt_memv) {
	START();
	
	for (val_t l = ARG(1); type_val(l) != NIL; l = cdr(l))
		if (_eqv(ARG(0), car(l))) RETURN(alloc_boolean(true));
	
	RETURN(alloc_boolean(false));
}

// reverse

FUN(plt_reverse) {
	START();
	val_t list = alloc_nil();
	for (val_t l = ARG(0); type_val(l) != NIL; l = cdr(l))
		list = alloc_cons(car(l), list);
	RETURN(list);
}

// c(a|d)+r

FUN(plt_caaar) {
	START();
	RETURN(car(car(car(ARG(0)))));
}

FUN(plt_caadr) {
	START();
	RETURN(car(car(cdr(ARG(0)))));
}

FUN(plt_caar) {
	START();
	RETURN(car(car(ARG(0))));
}

FUN(plt_cadar) {
	START();
	RETURN(car(cdr(car(ARG(0)))));
}

FUN(plt_cadddr) {
	START();
	RETURN(car(cdr(cdr(cdr(ARG(0))))));
}

FUN(plt_caddr) {
	START();
	RETURN(car(cdr(cdr(ARG(0)))));
}

FUN(plt_cadr) {
	START();
	RETURN(car(cdr(ARG(0))));
}

FUN(plt_car) {
	START();
	RETURN(car(ARG(0)));
}

FUN(plt_cdaar) {
	START();
	RETURN(cdr(car(car(ARG(0)))));
}

FUN(plt_cdadr) {
	START();
	RETURN(cdr(car(cdr(ARG(0)))));
}

FUN(plt_cdar) {
	START();
	RETURN(cdr(car(ARG(0))));
}

FUN(plt_cddar) {
	START();
	RETURN(cdr(cdr(car(ARG(0)))));
}

FUN(plt_cdddr) {
	START();
	RETURN(cdr(cdr(cdr(ARG(0)))));
}

FUN(plt_cddr) {
	START();
	RETURN(cdr(cdr(ARG(0))));
}

FUN(plt_cdr) {
	START();
	RETURN(cdr(ARG(0)));
}

//
// POSN
//

FUN(plt_make_dash_posn) {
	START();
	RETURN(alloc_struct("posn", 2, (val_t []) { ARG(0), ARG(1) }));
}

FUN(plt_posn_dash_x) {
	START();
	RETURN(elem_struct(struct_val(ARG(0)), 0));
}

FUN(plt_posn_dash_y) {
	START();
	RETURN(elem_struct(struct_val(ARG(0)), 1));
}

FUN(plt_posn_question_) {
	START();
	RETURN(alloc_boolean(strcmp(type_struct(struct_val(ARG(0))), "posn") == 0));
}

//
// EOF
//

FUN(plt_eof_dash_object_question_) {
	START();
	RETURN(alloc_boolean(false));
}

//
// MISC
//

// equal?, eq?, eqv?, equal~?

FUN(plt_equal_question_) {
	START();
	RETURN(alloc_boolean(_equal(ARG(0), ARG(1))));
}

FUN(plt_eq_question_) {
	START();
	RETURN(alloc_boolean(_eq(ARG(0), ARG(1))));
}

FUN(plt_eqv_question_) {
	START();
	RETURN(alloc_boolean(_eqv(ARG(0), ARG(1))));
}

FUN(plt_equal_tilde_) {
	START();
	
	if (type_val(ARG(0)) == type_val(ARG(1)))
		switch (type_val(ARG(0))) {
			case NUMBER:
				RETURN(alloc_boolean(abs(number_val(ARG(0)) - number_val(ARG(1))) <= number_val(ARG(2))));
			default:
				break;
		}
	
	RETURN(alloc_boolean(_equal(ARG(0), ARG(1))));
}

// error

FUN(plt_error) {
	START();
	// TODO
	RETURN(alloc_nil());
}

// struct?

FUN(plt_struct_question_) {
	START();
	RETURN(alloc_boolean(type_val(ARG(0)) == STRUCT));
}

// identity

FUN(plt_identity) {
	START();
	RETURN(ARG(0));
}

// current-seconds

FUN(plt_current_dash_seconds) {
	START();
	RETURN(alloc_number((double) millis() / 1000));
}
