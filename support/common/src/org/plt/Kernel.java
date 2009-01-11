package org.plt;
// The Kernel class contains all of the builtins except for the world
// primitives, which lives under the j2me tree.


import org.plt.types.*;

public class Kernel {

    static private java.util.Random randgen = new java.util.Random();


    // no-op: void -> void
    public static Object no_op() {
	return null;
    }

    public static Object no_op_worldEvent(Object x) {
	return x;
    }

    public static Object no_op_stopWhen(Object x) {
	return Logic.FALSE;
    }

    public static Object no_op_keyEvent(Object x, Object key) {
	return x;
    }

    //////////////////////////////////////////////////////////////////////

    public static org.plt.types.Number pi = FloatPoint.PI;
    public static org.plt.types.Number e = FloatPoint.E;



    public static Object identity(Object o) {
	return o;
    }

    // Numerics

    // >=
    public static Logic _greaterthan__equal_(Object _n1, Object _n2) {
	return toLogic(NumberTower.greaterThanEqual((org.plt.types.Number) _n1,
						    (org.plt.types.Number) _n2));
    }
    
    // >
    public static Logic _greaterthan_(Object _n1, Object _n2) {
	return toLogic(NumberTower.greaterThan((org.plt.types.Number) _n1,
					       (org.plt.types.Number) _n2));
    }
    
    // <=
    public static Logic _lessthan__equal_(Object _n1, Object _n2) {
	return toLogic(NumberTower.lessThanEqual((org.plt.types.Number) _n1,
						 (org.plt.types.Number) _n2));
    }
    
    // <
    public static Logic _lessthan_(Object _n1, Object _n2) {
	return toLogic(NumberTower.lessThan((org.plt.types.Number)_n1, 
					    (org.plt.types.Number)_n2));
    }


    // =
    public static Logic _equal_(Object _n1, Object _n2) {
	return toLogic(NumberTower.equal((org.plt.types.Number)_n1, 
					 (org.plt.types.Number)_n2));
    }


    // =~
    public static Logic _equal__tilde_(Object _n1, Object _n2, Object _n3) {
	return toLogic(NumberTower.approxEqual((org.plt.types.Number)_n1,
					       (org.plt.types.Number)_n2,
					       (org.plt.types.Number)_n3));
    }


    // +
    public static org.plt.types.Number _plus_(Object _n1, Object _n2) {
	return NumberTower.plus((org.plt.types.Number)_n1, (org.plt.types.Number)_n2);
    }


    // -
    public static org.plt.types.Number _dash_(Object _n1, Object _n2) {
	return NumberTower.minus((org.plt.types.Number)_n1, (org.plt.types.Number)_n2);
    }


    // *
    public static org.plt.types.Number _star_(Object _n1, Object _n2) {
	return NumberTower.multiply((org.plt.types.Number)_n1, (org.plt.types.Number)_n2);
    }

    // /
    public static org.plt.types.Number _slash_(Object _n1, Object _n2) {
	return NumberTower.divide((org.plt.types.Number)_n1, (org.plt.types.Number)_n2);
    }


    public static org.plt.types.Number abs(Object n) {
	return ((org.plt.types.Number)n).abs();
    }


    public static org.plt.types.Number acos(Object n) {
	return ((org.plt.types.Number)n).acos();
    }


    public static org.plt.types.Number sqrt(Object _n) {
	org.plt.types.Number n = (org.plt.types.Number) _n;
	return ((org.plt.types.Number)n).sqrt();
    }

    public static org.plt.types.Number modulo(Object _n1, Object _n2) {
	org.plt.types.Number n1 = (org.plt.types.Number) _n1;
	org.plt.types.Number n2 = (org.plt.types.Number) _n2;

	if (NumberTower.coerseLeft(n1, Rational.ONE) != null) {
	    n1 = NumberTower.coerseLeft(n1, Rational.ONE);
	}
	if (NumberTower.coerseLeft(n2, Rational.ONE) != null) {
	    n2 = NumberTower.coerseLeft(n2, Rational.ONE);
	}
	
	return n1.modulo(n2);
    }


    public static org.plt.types.Number floor(Object _n1) {
	return ((org.plt.types.Number)_n1).floor();
    }

    public static org.plt.types.Number ceiling(Object _n1) {
	return ((org.plt.types.Number)_n1).ceiling();
    }

    public static org.plt.types.Number sin(Object _n1) {
	return ((org.plt.types.Number)_n1).sin();
    }

    public static org.plt.types.Number asin(Object _n1) {
	return ((org.plt.types.Number)_n1).asin();
    }

    public static org.plt.types.Number atan(Object _n1) {
	return ((org.plt.types.Number)_n1).atan();
    }


    public static org.plt.types.Number cos(Object _n1) {
	return ((org.plt.types.Number)_n1).cos();
    }

    public static String number_dash__greaterthan_string(Object _n1) {
	return ((org.plt.types.Number)_n1).toString();
    }

    public static Logic equal_question_(Object _o1, Object _o2) {
	if (_o1 instanceof org.plt.types.Number && _o2 instanceof org.plt.types.Number) {
	    return _equal_(_o1, _o2);
	}
	return toLogic(_o1.equals(_o2));
    }


    //////////////////////////////////////////////////////////////////////
    public static org.plt.types.Number random(Object n) {
	int result = randgen.nextInt() % ((org.plt.types.Number)n).toInt();
	// Oddity, but modulo can return negative if dividend is negative.
	if (result < 0) {
	    return new Rational(result + ((org.plt.types.Number)n).toInt(),
				1);
	} else {
	    return new Rational(result, 1);
	}
    }

    public static Logic zero_question_(java.lang.Object n) {
	return toLogic(((org.plt.types.Number)n).isZero());
    }
    
    public static org.plt.types.Number max(Object n1, Object n2) {
	if (_greaterthan__equal_(n1, n2)

	    .isTrue()) {
	    return (org.plt.types.Number) n1;
	}
	return (org.plt.types.Number) n2;
    }

    public static org.plt.types.Number min(Object n1, Object n2) {
	if (_lessthan__equal_(n1, n2)
	    .isTrue()) {
	    return (org.plt.types.Number) n1;
	}
	return (org.plt.types.Number) n2;
    }

    public static org.plt.types.Number sqr(Object n) {
	return _star_(n, n);
    }


    public static org.plt.types.Number add1(Object n) {
	return _plus_(n, Rational.ONE);
    }

    public static org.plt.types.Number sub1(Object n) {
	return _dash_(n, Rational.ONE);
    }



    //////////////////////////////////////////////////////////////////////
    public static Logic string_equal__question_(Object s1, Object s2) {
	return toLogic(((String)s1).equals(s2));
    }


    public static Logic struct_question_(Object obj) {
	return toLogic(obj instanceof org.plt.types.Struct);
    }

    //////////////////////////////////////////////////////////////////////
    // Posn stuff
    public static Posn make_dash_posn(Object x, Object y) {
	return new Posn(x, y);
    }
    public static Object posn_dash_x(Object p) {
	return ((Posn)p).getX();
    }
    public static Object posn_dash_y(Object p) {
	return ((Posn)p).getY();
    }
    public static Logic posn_question_(Object p) {
	return toLogic(p instanceof org.plt.types.Posn);
    }


    //////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////

    public static Object first(Object l) {
	return ((org.plt.types.List)l).first();
    }

    public static Object second(Object _l) {
	org.plt.types.List l = (org.plt.types.List) _l;
	return l.rest().first();
    }
    
    public static Object third(Object _l) {
	org.plt.types.List l = (org.plt.types.List) _l;
	return l.rest().rest().first();
    }
    public static Object fourth(Object _l) {
	org.plt.types.List l = (org.plt.types.List) _l;
	return l.rest().rest().rest().first();
    }
    public static Object fifth(Object _l) {
	org.plt.types.List l = (org.plt.types.List) _l;
	return l.rest().rest().rest().rest().first();
    }
    public static Object sixth(Object _l) {
	org.plt.types.List l = (org.plt.types.List) _l;
	return l.rest().rest().rest().rest().rest().first();
    }
    public static Object seventh(Object _l) {
	org.plt.types.List l = (org.plt.types.List) _l;
	return l.rest().rest().rest().rest().rest().rest().first();
    }
    public static Object eighth(Object _l) {
	org.plt.types.List l = (org.plt.types.List) _l;
	return l.rest().rest().rest().rest().rest().rest().rest().first();
    }


    public static org.plt.types.List reverse(Object _l) {
	org.plt.types.List l = (org.plt.types.List) _l;
	org.plt.types.List rev = org.plt.types.Empty.EMPTY;
	while (! l.isEmpty()) {
	    rev = cons(l.first(), rev);
	    l = l.rest();
	}
	return rev;
    }



    public static org.plt.types.List rest(Object l) {
	return ((org.plt.types.List)l).rest();
    }

    public static Object car(Object o) {
	return first(o);
    }
    public static org.plt.types.List cdr(Object o) {
	return rest(o);
    }

    public static Object caaar(Object o) {
	return car(car(car(o)));
    }
    
    public static Object caadr(Object o) {
	return car(car(cdr(o)));
    }

    public static Object caar(Object o) {
	return car(car(o));
    }

    public static Object cadar(Object o) {
	return car(cdr(car(o)));
    }
    
    public static Object cadddr(Object o) {
	return car(cdr(cdr(cdr(o))));
    }

    public static Object caddr(Object o) {
	return car(cdr(cdr(o)));
    }

    public static Object cadr(Object o) {
	return car(cdr(o));
    }

    public static org.plt.types.List cdaar(Object o) {
	return cdr(car(car(o)));
    }
    public static org.plt.types.List cdadr(Object o) {
	return cdr(car(cdr(o)));
    }

    public static org.plt.types.List cdar(Object o) {
	return cdr(car(o));
    }

    public static org.plt.types.List cddar(Object o) {
	return cdr(cdr(car(o)));
    }

    public static org.plt.types.List cdddr(Object o) {
	return cdr(cdr(cdr(o)));
    }

    public static org.plt.types.List cddr(Object o) {
	return cdr(cdr(o));
    }


    public static Logic empty_question_(Object l) {
	return toLogic(((org.plt.types.List)l).isEmpty());
    }

    public static org.plt.types.List cons(Object x, Object xs) {
	return new Pair(x, (org.plt.types.List)xs);
    }

    //////////////////////////////////////////////////////////////////////



    //////////////////////////////////////////////////////////////////////
    public static Logic symbol_equal__question_(Object s1, Object s2) {
	return toLogic(((Symbol)s1).equals(s2));
    }


    public static String symbol_dash__greaterthan_string(Object o) {
	return ((Symbol)o).toString();
    }


    public static Logic not(Object l) {
	return ((Logic) l).negate();
    }

    
    //////////////////////////////////////////////////////////////////////
    
    public static Object error(Object s, Object msg) {
	throw new RuntimeException(s + ": " + msg);
    }

    public static Object error(Object s) {
	throw new RuntimeException("" + s);
    }

    
    //////////////////////////////////////////////////////////////////////

    // Converts from boolean to Logics.
    private static Logic toLogic(boolean b) {
	return b ? Logic.TRUE : Logic.FALSE;
    }


}
