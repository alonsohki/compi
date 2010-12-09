#include "CRules.h"

// Las siguientes tuplas son demasiado repetitivas a lo largo del código, en
// las definiciones de primero y siguiente, así que nos creamos unas constantes
// que las definen para claridad del código.
#define OPREL_TUPLES    ( OPERATOR, "<" ),  ( OPERATOR, ">" ), \
                        ( OPERATOR, "<=" ), ( OPERATOR, ">=" ), \
                        ( OPERATOR, "==" ), ( OPERATOR, "/=" )
#define OPL1_TUPLES     ( OPERATOR, "*" ),  ( OPERATOR, "/" )
#define OPL2_TUPLES     ( OPERATOR, "+" ),  ( OPERATOR, "-" )

DEFINE_RULE (programa,
            FIRST(
                    ( RESERVED, "programa" )
                 ),
            NEXT(
                    ( END_OF_FILE )
                )
)
{
    MATCH ( RESERVED,  "programa" );
    TOKEN ID = MATCH ( IDENTIFIER );
    { ADD_INST ( "prog " || ID.value ); }
    RULE  ( declaraciones )();
    RULE  ( decl_de_subprogs )();
    MATCH ( RESERVED, "comienzo" );
    RULE  ( lista_de_sentencias_prima )();
    MATCH ( RESERVED, "fin" );
    MATCH ( SEPARATOR, ";" );
    { ADD_INST ( "halt" ); }
}

DEFINE_RULE(declaraciones,
            FIRST(
                    ( RESERVED, "variables" ),
                    ( EMPTY )
                 ),
            NEXT(
                    ( RESERVED, "procedimiento"),
                    ( RESERVED, "funcion"),
                    ( RESERVED, "comienzo")
                )
)
{
    if ( IS_FIRST ( RESERVED, "variables" ) )
    {
        MATCH ( RESERVED, "variables" );
        RULE  ( lista_de_ident, ls_ident )();
        MATCH ( SEPARATOR, ":" );
        RULE  ( tipo, t ) ();
        MATCH ( SEPARATOR, ";" );
        {
            if ( IS_INTEGER(t.tipo) || IS_REAL(t.tipo) || IS_BOOLEAN(t.tipo) )
            {
                FOREACH ( ls_ident.ids AS ident )
                {
                    ADD_INST(TYPE_OF(t.tipo) || " " || ident );
                }
            }
            else if ( IS_ARRAY(t.tipo) )
            {
                FOREACH ( ls_ident.ids AS ident )
                {
                    ADD_INST("array_" || TYPE_OF(ARRAY_CONTENT(t.tipo))
                             || " " || ident || "," || ARRAY_SIZE(t.tipo) );
                }
            }
        }
        RULE  ( declaraciones )();
    }
    else
    {
        // Vacío
    }
}

DEFINE_RULE(lista_de_ident,
            FIRST(
                    ( IDENTIFIER )
                 ),
            NEXT(
                    ( RESERVED, ":" )
                )
)
{
    TOKEN ID = MATCH ( IDENTIFIER );
    RULE  ( resto_lista_ident, resto )();
    { THIS.ids = JOIN(INIT_LIST(ID.value), resto.ids); }
}

DEFINE_RULE(resto_lista_ident,
            FIRST(
                    ( SEPARATOR, "," ),
                    ( EMPTY )
                 ),
            NEXT(
                    ( RESERVED, ":" )
                )
)
{
    if ( IS_FIRST ( SEPARATOR, "," ) )
    {
        MATCH ( SEPARATOR, "," );
        TOKEN ID = MATCH ( IDENTIFIER );
        RULE  ( resto_lista_ident, resto )();
        { THIS.ids = JOIN(INIT_LIST(ID.value), resto.ids); }
    }
    else
    {
        { THIS.ids = EMPTY_LIST(); }
    }
}

DEFINE_RULE(tipo,
            FIRST(
                    ( RESERVED, "entero" ),
                    ( RESERVED, "real" ),
                    ( RESERVED, "booleano" ),
                    ( RESERVED, "array" )
                 ),
            NEXT(
                    ( SEPARATOR, ";" ),
                    ( RESERVED, "variables" ),
                    ( RESERVED, "comienzo" ),
                    ( SEPARATOR, ")" )
                )
)
{
    if ( IS_FIRST ( RESERVED, "entero" ) )
    {
        MATCH ( RESERVED, "entero" );
        { THIS.tipo = NEW_BASIC_TYPE(INTEGER); }
    }
    else if ( IS_FIRST ( RESERVED, "real" ) )
    {
        MATCH ( RESERVED, "real" );
        { THIS.tipo = NEW_BASIC_TYPE(REAL); }
    }
    else if ( IS_FIRST ( RESERVED, "booleano" ) )
    {
        MATCH ( RESERVED, "booleano" );
        { THIS.tipo = NEW_BASIC_TYPE(BOOLEAN); }
    }
    else if ( IS_FIRST ( RESERVED, "array" ) )
    {
        MATCH ( RESERVED, "array" );
        MATCH ( SEPARATOR, "[" );
        RULE  ( lista_de_enteros, ls )();
        MATCH ( SEPARATOR, "]" );
        MATCH ( RESERVED, "de" );
        RULE  ( tipo, t )();
        { THIS.tipo = NEW_ARRAY_TYPE(ls.ints, t.tipo); }
    }
    else PANIC();
}

DEFINE_RULE(lista_de_enteros,
            FIRST(
                ( INTEGER )
            ),
            NEXT(
                ( SEPARATOR, "]" )
            )
)
{
    TOKEN INT = MATCH ( INTEGER );
    RULE ( resto_lista_enteros, resto )();
    { THIS.ints = JOIN(INIT_LIST(INT.value), resto.ints); }
}

DEFINE_RULE(resto_lista_enteros,
            FIRST(
                ( SEPARATOR, "," ),
                ( EMPTY )
            ),
            NEXT(
                ( SEPARATOR, "]" )
            )
)
{
    if ( IS_FIRST ( SEPARATOR, "," ) )
    {
        MATCH ( SEPARATOR, "," );
        TOKEN INT = MATCH ( INTEGER );
        RULE ( resto_lista_enteros, resto )();
        { THIS.ints = JOIN(INIT_LIST(INT.value), resto.ints); }
    }
    else
    {
        { THIS.ints = EMPTY_LIST(); }
    }
}

DEFINE_RULE(decl_de_subprogs,
            FIRST(
                   ( RESERVED, "procedimiento" ),
                   ( RESERVED, "funcion" ),
                   ( EMPTY )
                 ),
            NEXT(
                   ( RESERVED, "comienzo" )
                )
)
{
    if ( IS_RULE_FIRST ( decl_de_procedimiento ) )
    {
        RULE ( decl_de_procedimiento )();
        RULE ( decl_de_subprogs )();
    }
    else if ( IS_RULE_FIRST ( decl_de_funcion ) )
    {
        RULE ( decl_de_funcion )();
        RULE ( decl_de_subprogs )();
    }
    else
    {
        // Vacío.
    }
}

DEFINE_RULE(decl_de_procedimiento,
            FIRST(
                    ( RESERVED, "procedimiento" )
                 ),
            NEXT(
                    ( RESERVED, "procedimiento" ),
                    ( RESERVED, "funcion" ),
                    ( RESERVED, "comienzo" )
                )
)
{
	{ ST_PUSH(); }
    RULE  ( cabecera_procedimiento )();
    RULE  ( declaraciones )();
    MATCH ( RESERVED, "comienzo" );
    RULE  ( lista_de_sentencias_prima )();
    MATCH ( RESERVED, "fin" );
    { ADD_INST ( "finproc "); }
    MATCH ( SEPARATOR, ";" );
    { ST_POP(); }
}

DEFINE_RULE(decl_de_funcion,
            FIRST(
                    ( RESERVED, "funcion" )
                 ),
            NEXT(
                    ( RESERVED, "procedimiento" ),
                    ( RESERVED, "funcion" ),
                    ( RESERVED, "comienzo" )
                )
)
{
	{ ST_PUSH(); }
    RULE  ( cabecera_funcion , c )();
    RULE  ( declaraciones )();
    MATCH ( RESERVED, "comienzo" );
    RULE  ( lista_de_sentencias_prima )();
    MATCH ( RESERVED, "retornar" );
    RULE  ( expresion , e )();
    {
    	// FIXME macro comprobar compatibilidad de tipos
        if ( e.tipo == c.tipo )
        {
        	ADD_INST ( "ret " || e.nombre );
            ADD_INST ( "finfunc ");
        }
        else
        {
            ERROR("Function return type mismatch");
        }
    }
    MATCH ( RESERVED, "fin" );
    MATCH ( SEPARATOR, ";" );
    { ST_POP(); }
}

DEFINE_RULE(cabecera_procedimiento,
            FIRST(
                    ( RESERVED, "procedimiento" )
                 ),
            NEXT(
                    ( RESERVED, "variables"),
                    ( RESERVED, "comienzo")
                )
)
{
    MATCH ( RESERVED, "procedimiento" );
    TOKEN ID = MATCH ( IDENTIFIER );
    { ADD_INST ( "proc " || ID.value ); }
    RULE  ( argumentos )();
}

DEFINE_RULE(cabecera_funcion,
            FIRST(
                    ( RESERVED, "funcion" )
                 ),
            NEXT(
                    ( RESERVED, "variables"),
                    ( RESERVED, "comienzo")
                )
)
{
    MATCH ( RESERVED, "funcion" );
    TOKEN ID = MATCH ( IDENTIFIER );
    { ADD_INST ( "func " || ID.value ); }
    RULE  ( argumentos )();
    MATCH ( RESERVED, "retorna" );
    RULE  ( tipo , t)();
    { THIS.tipo = t.tipo; }
}

DEFINE_RULE(argumentos,
            FIRST(
                    ( SEPARATOR, "(" ),
                    ( EMPTY )
                 ),
            NEXT(
                    ( RESERVED, "variables"),
                    ( RESERVED, "comienzo"),
                    ( RESERVED, "retorna")
                )
)
{
    if ( IS_FIRST ( SEPARATOR, "(" ) )
    {
        MATCH ( SEPARATOR, "(" );
        RULE  ( lista_de_param )();
        MATCH ( SEPARATOR, ")" );
    }
    else
    {
        // Vacío
    }
}

DEFINE_RULE(lista_de_param,
            FIRST(
                    ( IDENTIFIER )
                 ),
            NEXT(
                    ( SEPARATOR, ")" )
                )
)
{
    RULE  ( lista_de_ident , ls_ident )();
    MATCH ( SEPARATOR, ":" );
    RULE  ( clase_param , clase )();
    RULE  ( tipo , t )();
    { FOREACH ( ls_ident.ids AS id )
        {
            ADD_INST( clase.clase || "_" || TYPE_OF(t.tipo) || " " || id );
        }
    }
    RULE  ( resto_lis_de_param )();

}

DEFINE_RULE(resto_lis_de_param,
            FIRST(
                    ( SEPARATOR, ";" ),
                    ( EMPTY )
                 ),
            NEXT(
                    ( SEPARATOR, ")" )
                )
)
{
    if ( IS_FIRST ( SEPARATOR, ";" ) )
    {
        MATCH ( SEPARATOR, ";" );
        RULE  ( lista_de_ident, ls_ident )();
        MATCH ( SEPARATOR, ":" );
        RULE  ( clase_param, clase )();
        RULE  ( tipo , t )();
        { FOREACH ( ls_ident.ids AS id )
            {
                ADD_INST( clase.clase || "_" || TYPE_OF(t.tipo) || " " || id );
            }
        }
        RULE  ( resto_lis_de_param )();
    }
    else
    {
        // Vacío
    }
}

DEFINE_RULE(clase_param,
            FIRST(
                    ( RESERVED, "entrada" ),
                    ( RESERVED, "salida" )
                 ),
            NEXT(
                    ( RESERVED, "entero" ),
                    ( RESERVED, "real"),
                    ( RESERVED, "booleano"),
                    ( RESERVED, "array" )
                )
)
{
    if ( IS_FIRST( RESERVED, "entrada" ) )
    {
        MATCH ( RESERVED, "entrada" );
        RULE  ( clase_param_prima , c_p )();
        { THIS.clase = c_p.clase; }
    }
    else if ( IS_FIRST( RESERVED, "salida" ) )
    {
        MATCH ( RESERVED, "salida" );
        { THIS.clase = "ref"; }
    }
    else PANIC();
}

DEFINE_RULE(clase_param_prima,
            FIRST(
                    ( RESERVED, "salida" ),
                    ( EMPTY )
                 ),
            NEXT(
                    ( RESERVED, "entero" ),
                    ( RESERVED, "real"),
                    ( RESERVED, "booleano"),
                    ( RESERVED, "array" )
                )
)
{
    if ( IS_FIRST( RESERVED, "salida" ) )
    {
            MATCH ( RESERVED, "salida" );
            { THIS.clase = "ref"; }
    }
    else
    {
            { THIS.clase = "val"; }
    }
}

DEFINE_RULE(lista_de_sentencias_prima,
            FIRST(
                    ( IDENTIFIER ),
                    ( RESERVED, "si" ),
                    ( RESERVED, "hacer" ),
                    ( RESERVED, "salir" ),
                    ( RESERVED, "get" ),
                    ( RESERVED, "put_line" ),
                    ( EMPTY )
                 ),
            NEXT(
                    ( RESERVED, "fin" ),
                    ( RESERVED, "retorna" )
                )
)
{
    RULE  ( lista_de_sentencias, ls );
    { ls.hinloop = false; }
    ls();
}

DEFINE_RULE(lista_de_sentencias,
            FIRST(
                    ( IDENTIFIER ),
                    ( RESERVED, "si" ),
                    ( RESERVED, "hacer" ),
                    ( RESERVED, "salir" ),
                    ( RESERVED, "get" ),
                    ( RESERVED, "put_line" ),
                    ( EMPTY )
                 ),
            NEXT(
                    ( RESERVED, "fin" ),
                    ( RESERVED, "retorna" ),
                    ( RESERVED, "mientras" )
                )
)
{
    if ( IS_RULE_FIRST(sentencia) )
    {
        RULE  ( sentencia, s );
        { s.hinloop = THIS.hinloop; }
        s();
        RULE  ( lista_de_sentencias, ls );
        { ls.hinloop = s.hinloop; }
        ls();
        { THIS.salir_si = JOIN ( s.salir_si , ls.salir_si ); }
    }
    else
    {
    	{ THIS.salir_si = EMPTY_LIST(); }
    }
}

DEFINE_RULE(sentencia,
            FIRST(
                    ( IDENTIFIER ),
                    ( RESERVED, "si" ),
                    ( RESERVED, "hacer" ),
                    ( RESERVED, "salir" ),
                    ( RESERVED, "get" ),
                    ( RESERVED, "put_line" )
                 ),
            NEXT(
                    ( IDENTIFIER ),
                    ( RESERVED, "si" ),
                    ( RESERVED, "hacer" ),
                    ( RESERVED, "salir" ),
                    ( RESERVED, "get" ),
                    ( RESERVED, "put_line" ),
                    ( RESERVED, "fin" ),
                    ( RESERVED, "retorna" ),
                    ( RESERVED, "mientras" )
                )
)
{
    if (IS_FIRST ( IDENTIFIER ) )
    {
        TOKEN id = MATCH ( IDENTIFIER );
        RULE  ( expresiones , e );
        { e.hident = id.value; }
        MATCH ( SEPARATOR, ";" );
        { THIS.salir_si = EMPTY_LIST(); }
    }
    else if (IS_FIRST ( RESERVED, "si" ) )
    {
        MATCH ( RESERVED, "si" );
        RULE  ( expresion , e )();
        MATCH ( RESERVED, "entonces" );
        RULE  ( M , m1 )();
        RULE  ( lista_de_sentencias , ls )();
        MATCH ( RESERVED, "fin");
        MATCH ( RESERVED, "si");
        RULE  ( M , m2 )();
        MATCH ( SEPARATOR, ";");
        {
            COMPLETE( e.gtrue , m1.ref );
            COMPLETE( e.gfalse, m2.ref );
            THIS.salir_si = ls.salir_si;
        }
    }
    else if (IS_FIRST ( RESERVED, "hacer" ) )
    {
        MATCH ( RESERVED, "hacer" );
        RULE  ( M , m1 )();
        RULE  ( lista_de_sentencias, ls );
        ls.hinloop = true;
        ls();
        MATCH ( RESERVED, "mientras" );
        RULE  ( expresion , e )();
        MATCH ( RESERVED, "fin" );
        MATCH ( RESERVED, "hacer" );
        RULE  ( M , m2 )();
        MATCH ( SEPARATOR, ";");
        {
            COMPLETE( e.gtrue , m1.ref );
            COMPLETE( e.gfalse, m2.ref );
            COMPLETE( ls.salir_si, m2.ref );
            THIS.salir_si = EMPTY_LIST();
        }
    }
    else if (IS_FIRST ( RESERVED, "salir"))
    {
        MATCH ( RESERVED, "salir" );
        MATCH ( RESERVED, "si" );
        RULE  ( expresion , e )();
        RULE  ( M , m )();
        {
            COMPLETE( e.gfalse, m.ref );
            THIS.salir_si = e.gtrue;
        }
        MATCH ( SEPARATOR, ";");
    }
    else if (IS_FIRST ( RESERVED, "get"))
    {
        MATCH ( RESERVED, "get" );
        MATCH ( SEPARATOR, "(" );
        MATCH ( IDENTIFIER );
        RULE  ( id_o_array )();
        MATCH ( SEPARATOR, ")" );
        MATCH ( SEPARATOR, ";" );
    }
    else if (IS_FIRST ( RESERVED, "put_line"))
    {
        MATCH ( RESERVED, "put_line" );
        MATCH ( SEPARATOR, "(" );
        RULE  ( expresion )();
        MATCH ( SEPARATOR, ")");
        MATCH ( SEPARATOR, ";");
    }
    else PANIC();
}

DEFINE_RULE(id_o_array,
            FIRST(
                    ( SEPARATOR, "[" ),
                    ( EMPTY )
                 ),
            NEXT(
                    ( SEPARATOR, ")" )
                )
)
{
    if ( IS_RULE_FIRST ( acceso_a_array ) )
    {
        RULE ( acceso_a_array ) ();
    }
    else
    {
        // Vacío.
    }
}

DEFINE_RULE(expresiones,
            FIRST(
                    ( OPERATOR, "=" ),
                    ( OPERATOR, "[" ),
                    ( OPERATOR, "(" )
                 ),
            NEXT(
                    ( SEPARATOR, ";" ),
                    ( SEPARATOR, ")" )
                )
)
{
    if (IS_FIRST( OPERATOR, "=" ))
    {
        MATCH ( OPERATOR, "=" );
        RULE  ( expresion , e )();
        {
        	// FIXME Comprobar que los tipos son compatibles
        	if (true)
        	    ADD_INST( THIS.hident || ":=" || e.nombre );
        	else
        	    ERROR("Type mismatch");
        }
    }
    else if (IS_RULE_FIRST( acceso_a_array ))
    {
        RULE  ( acceso_a_array )();
        MATCH ( OPERATOR, "=" );
        RULE  ( expresion , e )();
        {
        	// FIXME Comprobar que los tipos son compatibles
        	if (true)
        	    ADD_INST( THIS.hident || ":=" || e.nombre );
        	else
        	    ERROR("Type mismatch");
        }
    }
    else if (IS_RULE_FIRST( parametros_llamadas ))
    {
        RULE  ( parametros_llamadas , p )();
        {
        	// FIXME Comprobar que la función existe en la tabla de simbolos y que los parametros coinciden
            //{ FOREACH ( p.parametros AS par )
            //    {
            //        ADD_INST( "param_" || par.clase || " " || TYPE_OF(par.tipo) || " " || par.nombre );
            //    }
            //}
        	//ADD_INST( "call " || THIS.hident );
        }
    }
    else PANIC();
}

DEFINE_RULE(acceso_a_array,
            FIRST(
                    ( SEPARATOR, "[" )
                 ),
            NEXT(
                    ( OPERATOR, "=" ),
                    OPL1_TUPLES,
                    OPL2_TUPLES,
                    OPREL_TUPLES,
                    ( RESERVED, "and" ),
                    ( RESERVED, "or" ),
                    ( RESERVED, "entonces" ),
                    ( RESERVED, "fin" ),
                    ( SEPARATOR, ";" ),
                    ( SEPARATOR, ")" ),
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, "," )
                )
)
{
    MATCH ( SEPARATOR, "[" );
    RULE  ( lista_de_expr )();
    MATCH ( SEPARATOR, "]" );
}

DEFINE_RULE(parametros_llamadas,
            FIRST(
                    ( SEPARATOR, "(" )
                 ),
            NEXT(
                    OPL1_TUPLES,
                    OPL2_TUPLES,
                    OPREL_TUPLES,
                    ( RESERVED, "and" ),
                    ( RESERVED, "or" ),
                    ( RESERVED, "entonces" ),
                    ( RESERVED, "fin" ),
                    ( SEPARATOR, ";" ),
                    ( SEPARATOR, ")" ),
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, "," )
                )
)
{
    MATCH ( SEPARATOR, "(" );
    RULE  ( lista_de_expr )();
    MATCH ( SEPARATOR, ")" );
}

DEFINE_RULE(expresion,
            FIRST(
                    ( RESERVED, "not" ),
                    ( OPERATOR, "-" ),
                    ( IDENTIFIER ),
                    ( INTEGER ),
                    ( REAL ),
                    ( RESERVED, "true" ),
                    ( RESERVED, "false" ),
                    ( SEPARATOR, "(" )
                 ),
            NEXT(
                    ( RESERVED, "entonces" ),
                    ( RESERVED, "fin" ),
                    ( SEPARATOR, ";" ),
                    ( SEPARATOR, ")" ),
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, "," )
                )
)
{
    RULE ( disyuncion )();
}

DEFINE_RULE(disyuncion,
            FIRST(
                    ( RESERVED, "not" ),
                    ( OPERATOR, "-" ),
                    ( IDENTIFIER ),
                    ( INTEGER ),
                    ( REAL ),
                    ( RESERVED, "true" ),
                    ( RESERVED, "false" ),
                    ( SEPARATOR, "(" )
                 ),
            NEXT(
                    ( RESERVED, "entonces" ),
                    ( RESERVED, "fin" ),
                    ( SEPARATOR, ";" ),
                    ( SEPARATOR, ")" ),
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, "," )
                )
)
{
    RULE ( conjuncion )();
    RULE ( disyuncion_prima )();
}

DEFINE_RULE(disyuncion_prima,
            FIRST(
                    ( RESERVED, "or" ),
                    ( EMPTY )
                 ),
            NEXT(
                    ( RESERVED, "entonces" ),
                    ( RESERVED, "fin" ),
                    ( SEPARATOR, ";" ),
                    ( SEPARATOR, ")" ),
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, "," )
                )
)
{
    if (IS_FIRST (RESERVED, "or")) {
        MATCH ( RESERVED, "or");
        RULE  ( conjuncion )();
        RULE  ( disyuncion_prima )();
    } else {
        // vacio
    }
}

DEFINE_RULE(conjuncion,
            FIRST(
                    ( RESERVED, "not" ),
                    ( OPERATOR, "-" ),
                    ( IDENTIFIER ),
                    ( INTEGER ),
                    ( REAL ),
                    ( RESERVED, "true" ),
                    ( RESERVED, "false" ),
                    ( SEPARATOR, "(" )
                 ),
            NEXT(
                    ( RESERVED, "or" ),
                    ( RESERVED, "entonces" ),
                    ( RESERVED, "fin" ),
                    ( SEPARATOR, ";" ),
                    ( SEPARATOR, ")" ),
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, "," )
                )
)
{
    RULE ( relacional )();
    RULE ( conjuncion_prima )();
}

DEFINE_RULE(conjuncion_prima,
            FIRST(
                    ( RESERVED, "and" ),
                    ( EMPTY )
                 ),
            NEXT(
                    ( RESERVED, "or" ),
                    ( RESERVED, "entonces" ),
                    ( RESERVED, "fin" ),
                    ( SEPARATOR, ";" ),
                    ( SEPARATOR, ")" ),
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, "," )
                )
)
{
    if (IS_FIRST (RESERVED, "and")) {
        MATCH ( RESERVED, "and");
        RULE  ( relacional )();
        RULE  ( conjuncion_prima )();
    } else {
        // vacio
    }
}

DEFINE_RULE(relacional,
            FIRST(
                    ( RESERVED, "not" ),
                    ( OPERATOR, "-" ),
                    ( IDENTIFIER ),
                    ( INTEGER ),
                    ( REAL ),
                    ( RESERVED, "true" ),
                    ( RESERVED, "false" ),
                    ( SEPARATOR, "(" )
                 ),
            NEXT(
                    ( RESERVED, "and" ),
                    ( RESERVED, "or" ),
                    ( RESERVED, "entonces" ),
                    ( RESERVED, "fin" ),
                    ( SEPARATOR, ";" ),
                    ( SEPARATOR, ")" ),
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, "," )
                )
)
{
    RULE ( aritmetica )();
    RULE ( relacional_prima )();
}

DEFINE_RULE(relacional_prima,
            FIRST(
                    OPREL_TUPLES,
                    ( EMPTY )
                 ),
            NEXT(
                    ( RESERVED, "and" ),
                    ( RESERVED, "or" ),
                    ( RESERVED, "entonces" ),
                    ( RESERVED, "fin" ),
                    ( SEPARATOR, ";" ),
                    ( SEPARATOR, ")" ),
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, "," )
                )
)
{
    if (IS_RULE_FIRST ( oprel )) {
        RULE ( oprel )();
        RULE ( aritmetica )();
        RULE ( relacional_prima )();
    } else {
        // vacio
    }
}

DEFINE_RULE(aritmetica,
            FIRST(
                    ( RESERVED, "not" ),
                    ( OPERATOR, "-" ),
                    ( IDENTIFIER ),
                    ( INTEGER ),
                    ( REAL ),
                    ( RESERVED, "true" ),
                    ( RESERVED, "false" ),
                    ( SEPARATOR, "(" )
                 ),
            NEXT(
                    OPREL_TUPLES,
                    ( RESERVED, "and" ),
                    ( RESERVED, "or" ),
                    ( RESERVED, "entonces" ),
                    ( RESERVED, "fin" ),
                    ( SEPARATOR, ";" ),
                    ( SEPARATOR, ")" ),
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, "," )
                )
)
{
    RULE ( termino )();
    RULE ( aritmetica_prima )();
}

DEFINE_RULE(aritmetica_prima,
            FIRST(
                    OPL2_TUPLES,
                    ( EMPTY )
                 ),
            NEXT(
                    OPREL_TUPLES,
                    ( RESERVED, "and" ),
                    ( RESERVED, "or" ),
                    ( RESERVED, "entonces" ),
                    ( RESERVED, "fin" ),
                    ( SEPARATOR, ";" ),
                    ( SEPARATOR, ")" ),
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, "," )
                )
)
{
    if (IS_RULE_FIRST ( opl2 )) {
        RULE ( opl2 )();
        RULE ( termino )();
        RULE ( aritmetica_prima )();
    } else {
        // vacio
    }
}

DEFINE_RULE(termino,
            FIRST(
                    ( RESERVED, "not" ),
                    ( OPERATOR, "-" ),
                    ( IDENTIFIER ),
                    ( INTEGER ),
                    ( REAL ),
                    ( RESERVED, "true" ),
                    ( RESERVED, "false" ),
                    ( SEPARATOR, "(" )
                 ),
            NEXT(
                    OPL2_TUPLES,
                    OPREL_TUPLES,
                    ( RESERVED, "and" ),
                    ( RESERVED, "or" ),
                    ( RESERVED, "entonces" ),
                    ( RESERVED, "fin" ),
                    ( SEPARATOR, ";" ),
                    ( SEPARATOR, ")" ),
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, "," )
                )
)
{
    RULE ( negacion )();
    RULE ( termino_prima )();
}

DEFINE_RULE(termino_prima,
            FIRST(
                    OPL1_TUPLES,
                    ( EMPTY )
                 ),
            NEXT(
                    OPL2_TUPLES,
                    OPREL_TUPLES,
                    ( RESERVED, "and" ),
                    ( RESERVED, "or" ),
                    ( RESERVED, "entonces" ),
                    ( RESERVED, "fin" ),
                    ( SEPARATOR, ";" ),
                    ( SEPARATOR, ")" ),
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, "," )
                )
)
{
    if (IS_RULE_FIRST ( opl1 )) {
        RULE ( opl1 )();
        RULE ( negacion )();
        RULE ( termino_prima )();
    } else {
        // vacio
    }
}


DEFINE_RULE(negacion,
            FIRST(
                    ( RESERVED, "not" ),
                    ( OPERATOR, "-" ),
                    ( IDENTIFIER ),
                    ( INTEGER ),
                    ( REAL ),
                    ( RESERVED, "true" ),
                    ( RESERVED, "false" ),
                    ( SEPARATOR, "(" )
                 ),
            NEXT(
                    OPL1_TUPLES,
                    OPL2_TUPLES,
                    OPREL_TUPLES,
                    ( RESERVED, "and" ),
                    ( RESERVED, "or" ),
                    ( RESERVED, "entonces" ),
                    ( RESERVED, "fin" ),
                    ( SEPARATOR, ";" ),
                    ( SEPARATOR, ")" ),
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, "," )
                )
)
{
    if (IS_FIRST ( RESERVED, "not"))
    {
        MATCH ( RESERVED, "not");
        RULE  ( factor )();
    }
    else if (IS_RULE_FIRST ( factor ))
    {
        RULE  ( factor )();
    }
    else PANIC();
}

DEFINE_RULE(factor,
            FIRST(
                    ( OPERATOR, "-" ),
                    ( IDENTIFIER ),
                    ( INTEGER ),
                    ( REAL ),
                    ( RESERVED, "true" ),
                    ( RESERVED, "false" ),
                    ( SEPARATOR, "(" )
                 ),
            NEXT(
                    OPL1_TUPLES,
                    OPL2_TUPLES,
                    OPREL_TUPLES,
                    ( RESERVED, "and" ),
                    ( RESERVED, "or" ),
                    ( RESERVED, "entonces" ),
                    ( RESERVED, "fin" ),
                    ( SEPARATOR, ";" ),
                    ( SEPARATOR, ")" ),
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, "," )
                )
)
{
    if (IS_FIRST ( OPERATOR, "-" )) {
        MATCH ( OPERATOR, "-" );
        RULE ( factor_prima )();
    }
    else if (IS_RULE_FIRST ( factor_prima ))
    {
        RULE ( factor_prima )();
    }
    else PANIC();
}

DEFINE_RULE(factor_prima,
            FIRST(
                    ( IDENTIFIER ),
                    ( INTEGER ),
                    ( REAL ),
                    ( RESERVED, "true" ),
                    ( RESERVED, "false" ),
                    ( SEPARATOR, "(" )
                 ),
            NEXT(
                    OPL1_TUPLES,
                    OPL2_TUPLES,
                    OPREL_TUPLES,
                    ( RESERVED, "and" ),
                    ( RESERVED, "or" ),
                    ( RESERVED, "entonces" ),
                    ( RESERVED, "fin" ),
                    ( SEPARATOR, ";" ),
                    ( SEPARATOR, ")" ),
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, "," )
                )
)
{
    if (IS_FIRST ( IDENTIFIER )) {
        MATCH ( IDENTIFIER);
        RULE  ( array_o_llamada )();
    }
    else if (IS_FIRST ( INTEGER ))
    {
        MATCH ( INTEGER );
    }
    else if (IS_FIRST ( REAL ))
    {
        MATCH ( REAL );
    }
    else if (IS_RULE_FIRST ( booleano ))
    {
        RULE  ( booleano )();
    }
    else if (IS_FIRST ( SEPARATOR, "(" ))
    {
        MATCH ( SEPARATOR, "(" );
        RULE  ( expresion )();
        MATCH ( SEPARATOR, ")" );
    }
    else PANIC();
}

DEFINE_RULE(array_o_llamada,
            FIRST(
                    ( SEPARATOR, "(" ),
                    ( SEPARATOR, "[" ),
                    ( EMPTY )
                 ),
            NEXT(
                    OPL1_TUPLES,
                    OPL2_TUPLES,
                    OPREL_TUPLES,
                    ( RESERVED, "and" ),
                    ( RESERVED, "or" ),
                    ( RESERVED, "entonces" ),
                    ( RESERVED, "fin" ),
                    ( SEPARATOR, ";" ),
                    ( SEPARATOR, ")" ),
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, "," )
                )
)
{
    if (IS_RULE_FIRST ( parametros_llamadas )) {
        RULE  ( parametros_llamadas )();
    }
    else if (IS_RULE_FIRST ( acceso_a_array )) {
        RULE  ( acceso_a_array )();
    }
    else
    {
        // vacío
    }
}

DEFINE_RULE(opl1,
            FIRST(
                    OPL1_TUPLES
                 ),
            NEXT(
                    ( RESERVED, "not" ),
                    ( OPERATOR, "-" ),
                    ( IDENTIFIER ),
                    ( INTEGER ),
                    ( REAL ),
                    ( RESERVED, "true" ),
                    ( RESERVED, "false" ),
                    ( SEPARATOR, "(" )
                 )
)
{
    if (IS_FIRST ( OPERATOR, "*" )) {
        MATCH ( OPERATOR, "*" );
    }
    else if (IS_FIRST ( OPERATOR, "/" )) {
        MATCH ( OPERATOR, "/" );
    }
    else PANIC();
}

DEFINE_RULE(opl2,
            FIRST(
                    OPL2_TUPLES
                 ),
            NEXT(
                    ( RESERVED, "not" ),
                    ( OPERATOR, "-" ),
                    ( IDENTIFIER ),
                    ( INTEGER ),
                    ( REAL ),
                    ( RESERVED, "true" ),
                    ( RESERVED, "false" ),
                    ( SEPARATOR, "(" )
                 )
)
{
    if (IS_FIRST ( OPERATOR, "-" )) {
        MATCH ( OPERATOR, "-" );
    }
    else if (IS_FIRST ( OPERATOR, "+" )) {
        MATCH ( OPERATOR, "+" );
    }
    else PANIC();
}

DEFINE_RULE(oprel,
            FIRST(
                    OPREL_TUPLES
                 ),
            NEXT(
                    ( RESERVED, "not" ),
                    ( OPERATOR, "-" ),
                    ( IDENTIFIER ),
                    ( INTEGER ),
                    ( REAL ),
                    ( RESERVED, "true" ),
                    ( RESERVED, "false" ),
                    ( SEPARATOR, "(" )
                 )
)
{
    if (IS_FIRST ( OPERATOR, "<" )) {
        MATCH ( OPERATOR, "<" );
    }
    else if (IS_FIRST ( OPERATOR, ">" )) {
        MATCH ( OPERATOR, ">" );
    }
    else if (IS_FIRST ( OPERATOR, "<=" )) {
        MATCH ( OPERATOR, "<=" );
    }
    else if (IS_FIRST ( OPERATOR, ">=" )) {
        MATCH ( OPERATOR, ">=" );
    }
    else if (IS_FIRST ( OPERATOR, "==" )) {
        MATCH ( OPERATOR, "==" );
    }
    else if (IS_FIRST ( OPERATOR, "/=" )) {
        MATCH ( OPERATOR, "/=" );
    }
    else PANIC();
}

DEFINE_RULE(booleano,
            FIRST(
                    ( RESERVED, "true" ),
                    ( RESERVED, "false" )
                 ),
            NEXT(
                    OPL1_TUPLES,
                    OPL2_TUPLES,
                    OPREL_TUPLES,
                    ( RESERVED, "and" ),
                    ( RESERVED, "or" ),
                    ( RESERVED, "entonces" ),
                    ( RESERVED, "fin" ),
                    ( SEPARATOR, ";" ),
                    ( SEPARATOR, ")" ),
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, "," )
                )
)
{
    if (IS_FIRST ( RESERVED, "true" )) {
        MATCH ( RESERVED, "true" );
    }
    else if (IS_FIRST ( RESERVED, "false" )) {
        MATCH ( RESERVED, "false" );
    }
    else PANIC();
}

DEFINE_RULE(M,
            FIRST(
                    ( EMPTY )
                 ),
            NEXT(
                    ( IDENTIFIER ),
                    ( RESERVED, "si" ),
                    ( RESERVED, "hacer" ),
                    ( RESERVED, "salir" ),
                    ( RESERVED, "get" ),
                    ( RESERVED, "put_line" ),
                    ( RESERVED, "fin" ),
                    ( SEPARATOR, ";" )
                )
)
{
    THIS.ref = GET_REF();
}

DEFINE_RULE(lista_de_expr,
            FIRST(
                    ( RESERVED, "not" ),
                    ( OPERATOR, "-" ),
                    ( IDENTIFIER ),
                    ( INTEGER ),
                    ( REAL ),
                    ( RESERVED, "true" ),
                    ( RESERVED, "false" ),
                    ( SEPARATOR, "(" )
                 ),
            NEXT(
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, ")" )
                )
)
{
    RULE ( expresion )();
    RULE ( resto_lista_expr )();
}

DEFINE_RULE(resto_lista_expr,
            FIRST(
                    ( SEPARATOR, "," ),
                    ( EMPTY )
                 ),
            NEXT(
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, ")" )
                )
)
{
    if ( IS_FIRST ( SEPARATOR, "," ) )
    {
        MATCH ( SEPARATOR, "," );
        RULE  ( expresion )();
        RULE  ( resto_lista_expr )();
    }
    else
    {
        // Vacío.
    }
}
