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
                    if ( ST_ADD(ident, t.tipo) )
                    {
                        ADD_INST(TYPE_OF(t.tipo) || " " || ident );
                    }
                }
            }
            else if ( IS_ARRAY(t.tipo) )
            {
                FOREACH ( ls_ident.ids AS ident )
                {
                    if ( ST_ADD(ident, t.tipo) )
                    {
                        ADD_INST("array_" || TYPE_OF(ARRAY_CONTENT(t.tipo))
                                 || " " || ident || "," || ARRAY_SIZE(t.tipo) );
                    }
                }
            }
        }
        RULE( declaraciones )();
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
    { ADD_INST ( "finproc"); }
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
            ADD_INST ( "finfunc");
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
    TOKEN id = MATCH ( IDENTIFIER );
    { ADD_INST ( "proc " || id.value ); }
    RULE  ( argumentos , a )();
    {
        CString tipo = NEW_FUNCTION_TYPE(a.listaTipos, a.listaClase,0);
        ST_ADD(id.value,tipo);
    }
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
    TOKEN id = MATCH ( IDENTIFIER );
    { ADD_INST ( "func " || id.value ); }
    RULE  ( argumentos , a )();
    MATCH ( RESERVED, "retorna" );
    RULE  ( tipo , t )();
    {
        CString tipo = NEW_FUNCTION_TYPE(a.listaTipos, a.listaClase, t.tipo);
        ST_ADD(id.value,tipo);

        THIS.tipo = t.tipo;
    }
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
    RULE  ( resto_lis_de_param , p );
    {
    	p.hlistaTipos = EMPTY_LIST();
    	p.hlistaClase = EMPTY_LIST();

    	FOREACH ( ls_ident.ids AS id )
        {
            if ( ST_ADD(id, t.tipo) )
            {
                ADD_INST( clase.clase || "_" || TYPE_OF(t.tipo) || " " || id );
                p.hlistaTipos = JOIN(p.hlistaTipos, INIT_LIST(t.tipo));
                p.hlistaClase = JOIN(p.hlistaClase, INIT_LIST(clase.clase));
            }
        }
    }
    p();
    {
    	THIS.listaTipos = p.listaTipos;
    	THIS.listaClase = p.listaClase;
    }
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
        RULE  ( resto_lis_de_param , p );
        {
        	p.hlistaTipos = EMPTY_LIST();
        	p.hlistaClase = EMPTY_LIST();

        	FOREACH ( ls_ident.ids AS id )
            {
                if ( ST_ADD(id, t.tipo) )
                {
                    ADD_INST( clase.clase || "_" || TYPE_OF(t.tipo) || " " || id );
                    p.hlistaTipos = JOIN(p.hlistaTipos, INIT_LIST(t.tipo));
                    p.hlistaClase = JOIN(p.hlistaClase, INIT_LIST(clase.clase));
                }
            }
        }
        p();
        {
        	THIS.listaTipos = p.listaTipos;
        	THIS.listaClase = p.listaClase;
        }
    }
    else
    {
        // Vacío
    	THIS.listaTipos = THIS.hlistaTipos;
    	THIS.listaClase = THIS.hlistaClase;
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
        RULE  ( asignacion_o_llamada , a );
        { a.hident = id.value; }
        a();
        MATCH ( SEPARATOR, ";" );
        { THIS.salir_si = EMPTY_LIST(); }
    }
    else if (IS_FIRST ( RESERVED, "si" ) )
    {
        MATCH ( RESERVED, "si" );
        RULE  ( expresion , e )();
        MATCH ( RESERVED, "entonces" );
        RULE  ( M , m1 )();
        RULE  ( lista_de_sentencias, ls );
        {
            ls.hinloop = THIS.hinloop;
        }
        ls();
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
        {
            ls.hinloop = true;
        }
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
        RULE ( acceso_a_array )();
    }
    else
    {
        // Vacío.
    }
}

DEFINE_RULE(asignacion_o_llamada,
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
            if (THIS.hident)
                ADD_INST( THIS.hident || ":=" || e.nombre );
            else
                ERROR("Type mismatch");
        }
    }
    else if (IS_RULE_FIRST( acceso_a_array ))
    {
        RULE  ( acceso_a_array , a )();
        MATCH ( OPERATOR, "=" );
        RULE  ( expresion , e )();
        {
            if (a.tipo == e.tipo)
            {
                ADD_INST( THIS.hident || ":=" || e.nombre );
            }
            else if ( IS_NUMERIC(a.tipo) && IS_BOOLEAN(e.tipo) )
            {
                CString next_ref;
                next_ref = GET_REF().intValue() + 2;
                ADD_INST ( "if " || e.nombre || " >  0 goto " || next_ref );
                next_ref = GET_REF().intValue() + 2;
                ADD_INST ( "goto " || next_ref );
                ADD_INST( THIS.hident || ":= 1 ");
                ADD_INST( THIS.hident || ":= 0 ");
            }
            else
            {
                ERROR("Type mismatch");
            }
        }
    }
    else if (IS_RULE_FIRST( parametros_llamadas ))
    {
        RULE  ( parametros_llamadas , p );
        {
            p.hident = THIS.hident;

        }
        p();
        {
            ADD_INST( "call " || THIS.hident );
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
    RULE ( disyuncion , d )();
    {
        THIS.nombre = d.nombre;
        THIS.tipo = d.tipo;
        THIS.gfalse = d.gfalse;
        THIS.gtrue = d.gtrue;
    }
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
    RULE ( conjuncion , c )();
    RULE ( disyuncion_prima , d );
    {
        d.hnombre = c.nombre;
        d.htipo = c.tipo;
        d.hgfalse = c.gfalse;
        d.hgtrue = c.gtrue;
    }
    d();
    {
        THIS.nombre = d.nombre;
        THIS.tipo = d.tipo;
        THIS.gfalse = d.gfalse;
        THIS.gtrue = d.gtrue;
    }
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
        RULE  ( conjuncion , c )();
        RULE  ( M , m )();
        RULE  ( disyuncion_prima , d );
        {
            if ( IS_NUMERIC(THIS.htipo) )
            {
                THIS.hgtrue = INIT_LIST(GET_REF());
                ADD_INST( "if " || THIS.hnombre || " > 0 goto ");
                THIS.hgfalse = INIT_LIST(GET_REF());
                ADD_INST( "goto ");
            }
            if ( IS_NUMERIC(c.tipo) )
            {
                c.gtrue = INIT_LIST(GET_REF());
                ADD_INST( "if " || c.nombre || " > 0 goto ");
                c.gfalse = INIT_LIST(GET_REF());
                ADD_INST( "goto ");
            }
            COMPLETE( THIS.hgfalse, m.ref );
            d.hgtrue = JOIN( THIS.hgtrue, c.gtrue );
            d.hgfalse = c.gfalse;
        }
        d();
        {
            THIS.nombre = d.nombre;
            THIS.tipo = d.tipo;
            THIS.gfalse = d.gfalse;
            THIS.gtrue = d.gtrue;
        }
    } else {
        THIS.nombre = THIS.hnombre;
        THIS.tipo = THIS.htipo;
        THIS.gfalse = THIS.hgfalse;
        THIS.gtrue = THIS.hgtrue;
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
    RULE ( relacional , r )();
    RULE ( conjuncion_prima , c );
    {
        c.hnombre = r.nombre;
        c.htipo = r.tipo;
        c.hgfalse = r.gfalse;
        c.hgtrue = r.gtrue;
    }
    c();
    {
        THIS.nombre = c.nombre;
        THIS.tipo = c.tipo;
        THIS.gfalse = c.gfalse;
        THIS.gtrue = c.gtrue;
    }
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
        RULE  ( M , m );
        RULE  ( relacional , r )();
        RULE  ( conjuncion_prima , c );
        {
            if ( IS_NUMERIC(THIS.htipo) )
            {
                THIS.hgtrue = INIT_LIST(GET_REF());
                ADD_INST( "if " || THIS.hnombre || " > 0 goto ");
                THIS.hgfalse = INIT_LIST(GET_REF());
                ADD_INST( "goto ");
            }
            if ( IS_NUMERIC(r.tipo) )
            {
                r.gtrue = INIT_LIST(GET_REF());
                ADD_INST( "if " || r.nombre || " > 0 goto ");
                r.gfalse = INIT_LIST(GET_REF());
                ADD_INST( "goto ");
            }
            COMPLETE( THIS.hgtrue, m.ref );
            c.hgfalse = JOIN( THIS.hgfalse, r.gfalse );
            c.hgtrue = r.gtrue;
        }
        c();
        {
            THIS.nombre = c.nombre;
            THIS.tipo = c.tipo;
            THIS.gfalse = c.gfalse;
            THIS.gtrue = c.gtrue;
        }
    } else {
        // Si es vacio
        THIS.nombre = THIS.hnombre;
        THIS.tipo = THIS.htipo;
        THIS.gfalse = THIS.hgfalse;
        THIS.gtrue = THIS.hgtrue;
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
    RULE ( aritmetica , a )();
    RULE ( relacional_prima , r );
    {
        r.hnombre = a.nombre;
        r.htipo = a.tipo;
        r.hgfalse = a.gfalse;
        r.hgtrue = a.gtrue;
    }
    r();
    {
        THIS.nombre = r.nombre;
        THIS.tipo = r.tipo;
        THIS.gfalse = r.gfalse;
        THIS.gtrue = r.gtrue;
    }
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
        RULE ( oprel , op )();
        RULE ( aritmetica , a )();
        RULE ( relacional_prima , r );
        {
            if ( IS_NUMERIC(THIS.htipo) && IS_NUMERIC(a.tipo)) {

                r.hgtrue = INIT_LIST(GET_REF());
                ADD_INST( "if" || THIS.hnombre || op.op || a.nombre || " goto ");
                r.hgfalse = INIT_LIST(GET_REF());
                ADD_INST( "goto ");

                r.htipo = NEW_BASIC_TYPE(BOOLEAN);

            } else {
                ERROR("Type mismatch error");
            }
        }
        r();
        {
            THIS.nombre = r.hnombre;
            THIS.tipo = r.htipo;
            THIS.gfalse = r.hgfalse;
            THIS.gtrue = r.hgtrue;
        }
    } else {
        // Si es vacio
        THIS.nombre = THIS.hnombre;
        THIS.tipo = THIS.htipo;
        THIS.gfalse = THIS.hgfalse;
        THIS.gtrue = THIS.hgtrue;
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
    RULE ( termino , t )();
    RULE ( aritmetica_prima , a );
    {
        a.hnombre = t.nombre;
        a.htipo = t.tipo;
        a.hgfalse = t.gfalse;
        a.hgtrue = t.gtrue;
    }
    a();
    {
        THIS.nombre = a.nombre;
        THIS.tipo = a.tipo;
        THIS.gfalse = a.gfalse;
        THIS.gtrue = a.gtrue;
    }
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
        RULE ( opl2 , op )();
        RULE ( termino , t )();
        RULE ( aritmetica_prima , a );
        {
            if ( IS_NUMERIC(THIS.htipo) && IS_NUMERIC(t.tipo) ) {

                a.hnombre = NEW_IDENT();
                ADD_INST( a.hnombre || ":=" || THIS.hnombre || op.op || t.nombre );

                if ( IS_REAL(THIS.htipo) || IS_REAL(t.tipo) )
                    a.htipo = NEW_BASIC_TYPE(REAL);
                else
                    a.htipo = NEW_BASIC_TYPE(INTEGER);

            } else {
                ERROR("Type mismatch error");
            }
        }
        a();
        {
            THIS.nombre = a.hnombre;
            THIS.tipo = a.htipo;
            THIS.gfalse = a.hgfalse;
            THIS.gtrue = a.hgtrue;
        }
    } else {
        // Si es vacio
        THIS.nombre = THIS.hnombre;
        THIS.tipo = THIS.htipo;
        THIS.gfalse = THIS.hgfalse;
        THIS.gtrue = THIS.hgtrue;
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
    RULE ( negacion , n )();
    RULE ( termino_prima , t );
    {
        t.hnombre = n.nombre;
        t.htipo = n.tipo;
        t.hgfalse = n.gfalse;
        t.hgtrue = n.gtrue;
    }
    t();
    {
        THIS.nombre = t.nombre;
        THIS.tipo = t.tipo;
        THIS.gfalse = t.gfalse;
        THIS.gtrue = t.gtrue;
    }
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
        RULE ( opl1 , op )();
        RULE ( negacion , f )();
        RULE ( termino_prima , t );
        {
            if ( IS_NUMERIC(THIS.htipo) && IS_NUMERIC(f.tipo) ) {

                t.hnombre = NEW_IDENT();
                ADD_INST( t.hnombre || ":=" || THIS.hnombre || op.op || f.nombre );

                if ( IS_REAL(THIS.htipo) || IS_REAL(f.tipo) )
                    t.htipo = NEW_BASIC_TYPE(REAL);
                else
                    t.htipo = NEW_BASIC_TYPE(INTEGER);

            } else {
                ERROR("Type mismatch error");
            }
        }
        t();
        {
            THIS.nombre = t.hnombre;
            THIS.tipo = t.htipo;
            THIS.gfalse = t.hgfalse;
            THIS.gtrue = t.hgtrue;
        }
    } else {
        // Si es vacio
        THIS.nombre = THIS.hnombre;
        THIS.tipo = THIS.htipo;
        THIS.gfalse = THIS.hgfalse;
        THIS.gtrue = THIS.hgtrue;
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
        RULE  ( factor , f )();
        {
            if ( IS_BOOLEAN(f.tipo) )
            {
                if (f.nombre == true)
                {
                    THIS.nombre = false;
                }
                else if (f.nombre == false)
                {
                    THIS.nombre = true;
                }
                else
                {
                    THIS.gfalse = f.gtrue;
                    THIS.gtrue = f.gfalse;
                }
                THIS.tipo = f.tipo;
            }
            else
            {
                ERROR("Type mismatch error");
            }
        }
    }
    else if (IS_RULE_FIRST ( factor ))
    {
        RULE  ( factor , f )();
        {
           THIS.nombre = f.nombre;
           THIS.tipo = f.tipo;
           THIS.gfalse = f.gfalse;
           THIS.gtrue = f.gtrue;
        }
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
        RULE ( factor_prima , f )();
        {
            if ( IS_REAL(f.tipo) || IS_INTEGER(f.tipo) )
            {
                THIS.nombre = NEW_IDENT();
                ADD_INST( THIS.nombre || ":=" || " - " || f.nombre );
                THIS.tipo = f.tipo;
            }
            else
            {
                ERROR("Type mismatch error");
            }
        }
    }
    else if (IS_RULE_FIRST ( factor_prima ))
    {
        RULE ( factor_prima , f )();
        {
           THIS.nombre = f.nombre;
           THIS.tipo = f.tipo;
           THIS.gfalse = f.gfalse;
           THIS.gtrue = f.gtrue;
        }
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
        TOKEN id = MATCH ( IDENTIFIER);
        RULE  ( array_o_llamada , a );
        {
            a.hident = id.value;
        }
        a();
        {
        	THIS.nombre = a.nombre;
            THIS.tipo = a.tipo;
        }
    }
    else if (IS_FIRST ( INTEGER ))
    {
        TOKEN token = MATCH ( INTEGER );
        {
            THIS.tipo = NEW_BASIC_TYPE(INTEGER);
            THIS.nombre = token.value;
        }
    }
    else if (IS_FIRST ( REAL ))
    {
        TOKEN token = MATCH ( REAL );
        {
            THIS.tipo = NEW_BASIC_TYPE(REAL);
            THIS.nombre = token.value;
        }
    }
    else if (IS_RULE_FIRST ( booleano ))
    {
        RULE ( booleano , token )();
        {
            THIS.tipo = NEW_BASIC_TYPE(BOOLEAN);
            THIS.nombre = token.value;
        }
    }
    else if (IS_FIRST ( SEPARATOR, "(" ))
    {
        MATCH ( SEPARATOR, "(" );
        RULE  ( expresion , e )();
        MATCH ( SEPARATOR, ")" );
        {
            THIS.tipo = e.tipo;
            THIS.nombre = e.nombre;
            THIS.tipo = e.tipo;
            THIS.gfalse = e.gfalse;
            THIS.gtrue = e.gtrue;
        }
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
        RULE  ( parametros_llamadas , p);
        {
            p.hident = THIS.hident;
            p.htipo = ST_GET_TYPE(THIS.hident);

            if ( IS_FUNCTION(p.htipo) == false)
            {
                ERROR("Unknow function " || THIS.hident);
            }
        }
        p();
    }
    else if (IS_RULE_FIRST ( acceso_a_array )) {
        RULE  ( acceso_a_array , a )();
        {
            a.hident = THIS.hident;
            a.htipo = ST_GET_TYPE(THIS.hident);

            if ( IS_ARRAY(a.htipo) == false )
            {
                ERROR("Unknow array " || THIS.hident);
            }
        }
        a();
        {
            THIS.nombre = a.nombre;
            THIS.tipo = a.tipo;
        }
    }
    else
    {
        // Variable
        THIS.nombre = THIS.hident;
        THIS.tipo = ST_GET_TYPE(THIS.hident);

        if ( IS_VARIABLE(THIS.tipo) == false )
        {
            ERROR("Unknow variable " || THIS.hident);
        }
    }
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
    RULE  ( expresion , e )();
    RULE  ( resto_lista_expr , resto );
    {
        if ( IS_INTEGER(e.tipo) )
        {
            resto.hoffset = e.nombre;
            resto.hdepth = 0;
        }
        else
        {
            ERROR("Invalid array index: Only integer expressions allowed");
        }
    }
    resto();
    {
        THIS.nombre = resto.nombre;
    }
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
        {
            unsigned int depth = ARRAY_DEPTH(THIS.htipo);
            if (THIS.hdepth >= depth - 1)
                ERROR("Array access out of bound");
        }
        RULE  ( expresion , e )();
        RULE  ( resto_lista_expr , resto );
        {
            if ( IS_INTEGER(e.tipo) )
            {
                unsigned int dimension = ARRAY_DIMENSION(THIS.htipo, THIS.hdepth.intValue());

                CString tmp = NEW_IDENT();
                ADD_INST( tmp || ":=" || THIS.hoffset || " * " || CString(dimension) );

                resto.hoffset = NEW_IDENT();
                ADD_INST( resto.hoffset || ":=" || tmp || " + " || e.nombre );

                resto.hdepth = THIS.hdepth + 1;
            }
            else
            {
                ERROR("Invalid array index: Only integer expressions allowed");
            }
        }
        resto();
    }
    else
    {
    	// Si es vacio devolvemos el acceso al array.
    	THIS.nombre = THIS.hident || "[" || THIS.hoffset || "]";
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
        TOKEN token = MATCH ( OPERATOR, "*" );
        THIS.op = token.value;
    }
    else if (IS_FIRST ( OPERATOR, "/" )) {
        TOKEN token = MATCH ( OPERATOR, "/" );
        THIS.op = token.value;
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
        TOKEN token = MATCH ( OPERATOR, "-" );
        THIS.op = token.value;
    }
    else if (IS_FIRST ( OPERATOR, "+" )) {
        TOKEN token = MATCH ( OPERATOR, "+" );
        THIS.op = token.value;
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
        TOKEN token = MATCH ( OPERATOR, "<" );
        THIS.op = token.value;
    }
    else if (IS_FIRST ( OPERATOR, ">" )) {
        TOKEN token = MATCH ( OPERATOR, ">" );
        THIS.op = token.value;
    }
    else if (IS_FIRST ( OPERATOR, "<=" )) {
        TOKEN token = MATCH ( OPERATOR, "<=" );
        THIS.op = token.value;
    }
    else if (IS_FIRST ( OPERATOR, ">=" )) {
        TOKEN token = MATCH ( OPERATOR, ">=" );
        THIS.op = token.value;
    }
    else if (IS_FIRST ( OPERATOR, "==" )) {
        TOKEN token = MATCH ( OPERATOR, "==" );
        THIS.op = token.value;
    }
    else if (IS_FIRST ( OPERATOR, "/=" )) {
        TOKEN token = MATCH ( OPERATOR, "/=" );
        THIS.op = token.value;
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
        TOKEN token = MATCH ( RESERVED, "true" );
        THIS.value = token.value;
    }
    else if (IS_FIRST ( RESERVED, "false" )) {
        TOKEN token = MATCH ( RESERVED, "false" );
        THIS.value = token.value;
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
