#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
//#include <stdio.h>
#include "scheme.h"

char *strdup(const char *s) {
	char *r = malloc(strlen(s) + 1);
	
	if (r == NULL) return r;
	
	return strcpy(r, s);
}

val_t alloc_val(enum type type, union data data) {
	val_t val = malloc(sizeof(struct val));
	
	if (val == NULL) return val;
	
	val -> ref = 0;
	val -> type = type;
	val -> data = data;
	
	//printf("alloc(p%p).\ntype(p%p, %i).\n", val, val, val -> type);
	return val;
}

void free_val(val_t val) {
	if (val -> ref == 0) {
		switch (val -> type) {
			case STRING:
				free(val -> data.string);
				break;
			case STRUCT:
				for (struct_size_t i = 0; i < val -> data.struct_ -> size; i++)
					deref(val -> data.struct_ -> data[i]);
				free(val -> data.struct_);
				break;
			case SB:
				free(val -> data.sb);
				break;
			default:
				break;
		}
		
		//printf("free(p%p).\n", val);
		free(val);
	}
}

val_t ref(val_t val) {
	//printf("ref(p%p).\n", val);
	val -> ref++;
	return val;
}

void deref(val_t val) {
	//printf("deref(p%p).\n", val);
	val -> ref--;
	free_val(val);
}

val_t deref_nofree(val_t val) {
	val -> ref--;
	return val;
}

val_t alloc_nil() {
	return alloc_val(NIL, (union data) (bool) false);
}

val_t alloc_boolean(bool v) {
	return alloc_val(BOOLEAN, (union data) v);
}

val_t alloc_integer(int v) {
	return alloc_val(INTEGER, (union data) v);
}

val_t alloc_number(float v) {
	return alloc_val(NUMBER, (union data) v);
}

val_t alloc_character(char v) {
	return alloc_val(CHARACTER, (union data) v);
}

val_t alloc_string(const char *v) {
	return alloc_val(STRING, (union data) strdup(v));
}

val_t alloc_string_nocopy(char *v) {
	return alloc_val(STRING, (union data) v);
}

val_t alloc_symbol(const char *v) {
	return alloc_val(SYMBOL, (union data) v);
}

val_t alloc_struct(const char *type, struct_size_t size, val_t *data) {
	struct_t *v = malloc(sizeof(struct_t) + sizeof(val_t[size]));
	
	if (v == NULL) return NULL;
	
	v -> size = size;
	v -> type = type;
	
	for (struct_size_t i = 0; i < size; i++)
		v -> data[i] = ref(data[i]);
	
	return alloc_val(STRUCT, (union data) v);
}

val_t alloc_sb(unsigned char *data) {
	sb_t *v = malloc(sizeof(sb_t) + BUFFER_SIZE * sizeof(unsigned char)); /* this might need to be changed to another example of an unsigned char */

	if (v == NULL) return NULL;

	v -> size = BUFFER_SIZE;

	// memcpy
	for(sb_size_t i = 0; i < BUFFER_SIZE; i++)
		v -> data[i] = data[i];

	return alloc_val(SB, (union data) v);
}

val_t alloc_function(function_t *v) {
	return alloc_val(FUNCTION, (union data) v);
}

bool boolean_val(val_t val) {
	bool ret = true;
	if (val -> type == BOOLEAN) ret = val -> data.boolean;
	free_val(val);
	return ret;
}

int integer_val(val_t val) {
	int ret = 0;
	if (val -> type == INTEGER) ret = val -> data.integer;
	free_val(val);
	return ret;
}

float number_val(val_t val) {
	float ret;
	
	switch (val -> type) {
		case NUMBER:
			ret = val -> data.number;
			break;
		case INTEGER:
			ret = val -> data.integer;
			break;
		default:
			ret = 0;
			break;
	}
	
	free_val(val);
	return ret;
}

char character_val(val_t val) {
	char ret = '\0';
	if (val -> type == CHARACTER) ret = val -> data.character;
	free_val(val);
	return ret;
}

char *string_val(val_t val) {
	char *ret = "";
	if (val -> type == STRING) ret = val -> data.string;
	free_val(val);
	return ret;
}

const char *symbol_val(val_t val) {
	const char *ret = "";
	if (val -> type == SYMBOL) ret = val -> data.symbol;
	free_val(val);
	return ret;
}

struct_t *struct_val(val_t val) {
	struct_t *ret = & (struct_t) { 0, "" };
	if (val -> type == STRUCT) ret = val -> data.struct_;
	free_val(val);
	return ret;
}

val_t elem_struct(struct_t *s, struct_size_t i) {
	if (i < s -> size) return s -> data[i];
	else return alloc_boolean(false);
}

struct_size_t size_struct(struct_t *s) {
	return s -> size;
}

const char *type_struct(struct_t *s) {
	return s -> type;
}

sb_t *sb_val(val_t val) {
	sb_t *ret = & (sb_t) { 0 };
	if (val -> type == SB) ret = val -> data.sb;
	free_val(val);
	return ret;
}

enum type type_val(val_t val) {
	enum type ret = val -> type;
	free_val(val);
	return ret;
}

FUN(dummy) {
	START();
	RETURN(alloc_nil());
}

function_t *function_val(val_t val) {
	function_t *ret = dummy;
	if (val -> type == FUNCTION) ret = val -> data.function;
	free_val(val);
	return ret;
}

bool _equal(val_t a, val_t b) {
	if (_eqv(a, b)) return true;
	
	if (a -> type == b -> type)
		switch (a -> type) {
			case STRING:
				return (strcmp(a -> data.string, b -> data.string) == 0);
			case STRUCT: {
				struct_t *sa = a -> data.struct_, *sb = b -> data.struct_;
				if (sa -> type == sb -> type && sa -> size == sb -> size) {
					for (int i = 0; i < sa -> size; i++)
						if (!_equal(sa -> data[i], sb -> data[i])) return false;
					return true;
				}
				else return false;
			}
			case SB: {
				sb_t *sba = a -> data.sb, *sbb = b -> data.sb;
				if( sba -> size == sbb -> size) /* Technically, this test isn't needed but it is included for consistency's sake */ {
					for (int i = 0; i < sba -> size; i++)
						if(sba -> data[i] != sbb -> data[i]) return false;
					return true;
				}
				else return false;
		   }
			default:
				return false;
		}
	else return false;
}

bool _eq(val_t a, val_t b) {
	return _eqv(a, b);
}

bool _eqv(val_t a, val_t b) {
	if (a == b) return true;
	
	if (a -> type == b -> type)
		switch (a -> type) {
			case NIL:
				return true;
			case BOOLEAN:
				return (a -> data.boolean = b -> data.boolean);
			case INTEGER:
				return (a -> data.integer == b -> data.integer);
			case NUMBER:
				return (a -> data.number == b -> data.number);
			case CHARACTER:
				return (a -> data.character == b -> data.character);
			case STRING:
				return (a -> data.string == b -> data.string);
			case SYMBOL:
				return (a -> data.string == b -> data.string);
			case STRUCT:
				return (a -> data.struct_ == b -> data.struct_);
			case SB:
				return (a -> data.sb == b -> data.sb);
			default:
				return false;
		}
	else return false;
}

val_t error(const char *err) {
	// TODO
	return alloc_nil();
}
