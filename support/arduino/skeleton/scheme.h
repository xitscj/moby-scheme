#ifndef SCHEME_H
#define SCHEME_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdbool.h>
#include <stdint.h>

char *strdup(const char *s);

enum type {
	NIL,
	BOOLEAN,
	INTEGER, // DON'T USE... experimental
	NUMBER,
	CHARACTER,
	STRING,
	SYMBOL,
	STRUCT,
	FUNCTION
};

typedef uint8_t struct_size_t;

typedef struct {
	struct_size_t size;
	const char *type;
	struct val *data[];
} struct_t;

typedef struct val *function_t(struct val *);

union data {
	bool boolean;
	int integer;
	float number;
	char character;
	char *string;
	const char *symbol;
	struct_t *struct_;
	function_t *function;
};

struct val {
	unsigned char ref;
	enum type type;
	union data data;
};

typedef struct val *val_t;

val_t alloc_nil(void);
val_t alloc_boolean(bool v);
val_t alloc_integer(int v);
val_t alloc_number(float v);
val_t alloc_character(char v);
val_t alloc_string(const char *v);
val_t alloc_string_nocopy(char *v);
val_t alloc_symbol(const char *v);
val_t alloc_struct(const char *type, struct_size_t size, val_t *data);
val_t alloc_function(function_t *function);

void free_val(val_t val);
val_t ref(val_t val);
void deref(val_t val);
val_t deref_nofree(val_t val);

bool boolean_val(val_t val);
int integer_val(val_t val);
float number_val(val_t val);
char character_val(val_t val);
char *string_val(val_t val);
const char *symbol_val(val_t val);
struct_t *struct_val(val_t val);
val_t elem_struct(struct_t *struct_, struct_size_t i);
const char *type_struct(struct_t *struct_);
struct_size_t size_struct(struct_t *struct_);
enum type type_val(val_t val);
function_t *function_val(val_t val);

bool _equal(val_t a, val_t b);
bool _eq(val_t a, val_t b);
bool _eqv(val_t a, val_t b);

val_t error(const char *err);

#define FUN(name) val_t name(val_t _args)
#define START() (ref(_args))
#define ARG(i) (elem_struct(struct_val(_args), (i)))
#define ARGC() (size_struct(struct_val(_args)))
#define RETURN(r) ({ val_t _ret = r; deref(_args); return _ret; })
#define CALL(fun, n, args...) ((fun)(alloc_struct("", (n), (val_t []){ args })))
#define IGNORE(x) free(x)

#define REREF(var, val) ({ val_t _temp = ref(val); deref(var); (var) = _temp; })

#define alloc_cons(car, cdr) alloc_struct("cons", 2, (val_t []){ car, cdr })
#define car(cons) elem_struct(struct_val(cons), 0)
#define cdr(cons) elem_struct(struct_val(cons), 1)

#ifdef __cplusplus
}
#endif

#endif
