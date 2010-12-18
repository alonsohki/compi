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
                    ( RESERVED, "procedimiento" ),
                    ( RESERVED, "funcion" ),
                    ( RESERVED, "comienzo" )
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
        RULE ( decl_de_procedimiento, proc )();
        {
            ST_ADD ( proc.nombre, NEW_PROCEDURE_TYPE(proc.args, proc.classes) );
        }
        RULE ( decl_de_subprogs )();
    }
    else if ( IS_RULE_FIRST ( decl_de_funcion ) )
    {
        RULE ( decl_de_funcion, func )();
        {
            ST_ADD ( func.nombre, NEW_FUNCTION_TYPE(func.args, func.classes, func.tipoRetorno) );
        }
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
    RULE  ( cabecera_procedimiento, c )();
    RULE  ( declaraciones )();
    MATCH ( RESERVED, "comienzo" );
    RULE  ( lista_de_sentencias_prima )();
    MATCH ( RESERVED, "fin" );
    { ADD_INST ( "finproc" ); }
    MATCH ( SEPARATOR, ";" );
    {
        ST_POP();
        THIS.args = c.args;
        THIS.classes = c.classes;
        THIS.nombre = c.nombre;
    }
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
        ADD_INST ( "ret " || TYPECAST( e.nombre, e.tipo, c.tipoRetorno, e.gtrue, e.gfalse ) );
        ADD_INST ( "finfunc" );
    }
    MATCH ( SEPARATOR, ";" );
    MATCH ( RESERVED, "fin" );
    MATCH ( SEPARATOR, ";" );
    {
        ST_POP();
        THIS.args = c.args;
        THIS.classes = c.classes;
        THIS.nombre = c.nombre;
        THIS.tipoRetorno = c.tipoRetorno;
    }
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
    RULE  ( argumentos, args )();
    {
        THIS.nombre = ID.value;
        THIS.args = args.args;
        THIS.classes = args.classes;
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
    TOKEN ID = MATCH ( IDENTIFIER );
    { ADD_INST ( "func " || ID.value ); }
    RULE  ( argumentos, args )();
    MATCH ( RESERVED, "retorna" );
    RULE  ( tipo , t )();
    {
        THIS.tipoRetorno = t.tipo;
        THIS.nombre = ID.value;
        THIS.args = args.args;
        THIS.classes = args.classes;
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
        RULE  ( lista_de_param, ls )();
        MATCH ( SEPARATOR, ")" );
        {
            THIS.args = ls.args;
            THIS.classes = ls.classes;
        }
    }
    else
    {
        THIS.args = EMPTY_LIST ();
        THIS.classes = EMPTY_LIST ();
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
    {
        THIS.args = EMPTY_LIST ();
        THIS.classes = EMPTY_LIST ();
    	FOREACH ( ls_ident.ids AS id )
        {
            ADD_INST( clase.clase || "_" || TYPE_OF(t.tipo) || " " || id );
            ST_ADD ( id, t.tipo );
            THIS.args = JOIN(THIS.args, INIT_LIST(t.tipo));
            THIS.classes = JOIN(THIS.classes, INIT_LIST(clase.clase));
        }
    }
    RULE  ( resto_lis_de_param, resto )();
    {
        THIS.args = JOIN(THIS.args, resto.args);
        THIS.classes = JOIN(THIS.classes, resto.classes);
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
        {
            THIS.args = EMPTY_LIST();
            THIS.classes = EMPTY_LIST();
            FOREACH ( ls_ident.ids AS id )
            {
                ADD_INST( clase.clase || "_" || TYPE_OF(t.tipo) || " " || id );
                ST_ADD ( id, t.tipo );
                THIS.args = JOIN(THIS.args, INIT_LIST(t.tipo));
                THIS.classes = JOIN(THIS.classes, INIT_LIST(clase.clase));
            }
        }
        RULE  ( resto_lis_de_param, resto )();
        {
            THIS.args = JOIN(THIS.args, resto.args);
            THIS.classes = JOIN(THIS.classes, resto.classes);
        }
    }
    else
    {
        THIS.args = EMPTY_LIST ();
        THIS.classes = EMPTY_LIST ();
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
        { ls.hinloop = THIS.hinloop; }
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
        {
            a.hident = id.value;
        }
        a();
        MATCH ( SEPARATOR, ";" );
        { THIS.salir_si = EMPTY_LIST(); }
    }
    else if (IS_FIRST ( RESERVED, "si" ) )
    {
        MATCH ( RESERVED, "si" );
        RULE  ( expresion , e )();
        {
           TYPECAST( e.nombre, e.tipo, NEW_BASIC_TYPE(BOOLEXPR), e.gtrue, e.gfalse);
        }
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
        TOKEN ID = MATCH ( IDENTIFIER );
        RULE  ( id_o_array, ioa );
        { ioa.hident = ID.value; }
        ioa ();
        MATCH ( SEPARATOR, ")" );
        MATCH ( SEPARATOR, ";" );
        {
            THIS.salir_si = EMPTY_LIST ();
            ADD_INST ( "read " || ioa.nombre );
        }
    }
    else if (IS_FIRST ( RESERVED, "put_line"))
    {
        MATCH ( RESERVED, "put_line" );
        MATCH ( SEPARATOR, "(" );
        RULE  ( expresion , e )();
        MATCH ( SEPARATOR, ")");
        MATCH ( SEPARATOR, ";");
        {
            THIS.salir_si = EMPTY_LIST ();
            ADD_INST ( "write " || e.nombre );
            ADD_INST ( "writeln" );
        }
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
        RULE ( acceso_a_array, a );
        {
            a.hident = THIS.hident;
            a.htipo = NEW_BASIC_TYPE(UNKNOWN);
        }
        a();
        {
            THIS.nombre = THIS.hident || "[" || a.offset || "]";
        }
    }
    else
    {
        if ( ST_EXISTS(THIS.hident) == false )
            ERROR("Unknown identifier '" || THIS.hident || "'");
        THIS.nombre = THIS.hident;
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
            VAR tipo;
            if ( ST_EXISTS (THIS.hident))
            	tipo = ST_GET_TYPE(THIS.hident);
            else
            {
            	tipo = NEW_BASIC_TYPE(UNKNOWN);
            	ERROR ( "Identifier '" || THIS.hident || "' not found" );
            }
            ADD_INST( THIS.hident || " := " || TYPECAST( e.nombre, e.tipo, tipo, e.gtrue, e.gfalse ) );
        }
    }
    else if (IS_RULE_FIRST( acceso_a_array ))
    {
        RULE  ( acceso_a_array , a );
        {
            a.hident = THIS.hident;
            a.htipo = NEW_BASIC_TYPE(UNKNOWN);
        }
        a();
        MATCH ( OPERATOR, "=" );
        RULE  ( expresion , e )();
        {
            ADD_INST ( THIS.hident || "[" || a.offset || "]" ||
                       " := " || TYPECAST(e.nombre, e.tipo, a.tipo) );
        }
    }
    else if (IS_RULE_FIRST( parametros_llamadas ))
    {
        RULE  ( parametros_llamadas , p );
        {
            p.hident = THIS.hident;
            p.hrequireFunc = false;
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
    RULE  ( lista_de_expr, ls )();
    MATCH ( SEPARATOR, "]" );
    RULE  ( acceso_a_array_prima, otros )();
    {
        THIS.offset = NEW_IDENT ();
        ADD_INST ( THIS.offset || " := 0" );

        VAR exprs = JOIN(ls.exprs, otros.exprs);
        VAR tipos = JOIN(ls.tipos, otros.tipos);
        
        if ( IS_UNKNOWN(THIS.htipo) == true && ST_EXISTS(THIS.hident) == false )
        {
            ERROR ( "Identifier '" || THIS.hident || "' not found." );
            THIS.tipo = NEW_BASIC_TYPE ( UNKNOWN );
        }
        else
        {
            VAR tipo;
            if ( IS_UNKNOWN(THIS.htipo) == true )
                tipo = ST_GET_TYPE ( THIS.hident );
            else
                tipo = THIS.htipo;
            if ( IS_ARRAY(tipo) == false )
            {
                ERROR( "Identifier '" || THIS.hident || "' is not of type 'array'.");
                THIS.tipo = NEW_BASIC_TYPE ( UNKNOWN );
            }
            else
            {
                THIS.tipo = ARRAY_CONTENT(tipo);
                if ( LIST_SIZE(exprs) > ARRAY_DEPTH(tipo) )
                    ERROR ( "Too many subscripts for array '" || THIS.hident || "'" );
                else if ( LIST_SIZE(exprs) < ARRAY_DEPTH(tipo) )
                    ERROR ( "Too few subscripts for array '" || THIS.hident || "'" );
                else
                {
                    VAR i = 0;
                    VAR curDimension = ARRAY_DEPTH(tipo) - 1;
                    FOREACH ( exprs AS nombre )
                    {
                        ADD_INST ( THIS.offset || " := " || THIS.offset || " * " || ARRAY_DIMENSION(tipo, curDimension) );
                        ADD_INST ( THIS.offset || " := " || THIS.offset || " + " ||
                        		   TYPECAST(nombre, LIST_ITEM(tipos,i), NEW_BASIC_TYPE(INTEGER)) );
                        curDimension = curDimension - 1;
                        i = i + 1;
                    }
                }
            }
        }
    }
}
DEFINE_RULE(acceso_a_array_prima,
            FIRST(
                ( SEPARATOR, "[" ),
                ( EMPTY )
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
    if ( IS_FIRST ( SEPARATOR, "[" ) )
    {
        MATCH ( SEPARATOR, "[" );
        RULE  ( lista_de_expr, ls )();
        MATCH ( SEPARATOR, "]" );
        RULE  ( acceso_a_array_prima, otros )();
        {
            THIS.exprs = JOIN(ls.exprs, otros.exprs);
            THIS.tipos = JOIN(ls.tipos, otros.tipos);
        }
    }
    else
    {
        // Vacío
        THIS.exprs = EMPTY_LIST();
        THIS.tipos = EMPTY_LIST();
    }
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
                    ( SEPARATOR, "]" ),
                    ( SEPARATOR, "," )
                )
)
{
    MATCH ( SEPARATOR, "(" );
    RULE  ( lista_de_expr, ls )();
    MATCH ( SEPARATOR, ")" );
    {
        if ( ST_EXISTS ( THIS.hident ) == false )
        {
            ERROR ( "Unknown identifier '" || THIS.hident || "'" );
            THIS.tipoRetorno = NEW_BASIC_TYPE ( UNKNOWN );
        }
        else
        {
            VAR tipo = ST_GET_TYPE ( THIS.hident );
            if ( ( IS_FUNCTION ( tipo ) || IS_PROCEDURE ( tipo ) ) == false )
            {
                ERROR ( "Attempting to call '" || THIS.hident || "', which is not a function or procedure." );
                THIS.tipoRetorno = NEW_BASIC_TYPE ( UNKNOWN );
            }
            else if ( THIS.hrequireFunc == true && IS_PROCEDURE ( tipo ) )
            {
                ERROR ( "You cannot call '" || THIS.hident || "' from here. It must be a function." );
                THIS.tipoRetorno = NEW_BASIC_TYPE ( UNKNOWN );
            }
            else
            {
                if ( IS_PROCEDURE ( tipo ) )
                {
                    THIS.tipoRetorno = NEW_BASIC_TYPE ( UNKNOWN );
                    if ( THIS.hrequireFunc == true )
                        ERROR ( "You cannot call '" || THIS.hident || "' from here. It must be a function." );
                }
                else
                    THIS.tipoRetorno = FUNCTION_RETURN( tipo );

                if ( LIST_SIZE(ls.exprs) != SUBPROG_NUM_PARAMS(tipo) )
                {
                    ERROR ( "Wrong number of arguments for '" || THIS.hident || "'" );
                }
                else
                {
                    VAR i = 0;
                    FOREACH ( ls.exprs AS expr )
                    {
                        VAR paramTipo = LIST_ITEM(ls.tipos, i);
                        VAR reqTipo = SUBPROG_PARAM(tipo, i);
                        if ( SUBPROG_PARAM_CLASS(tipo, i) == "ref" && LIST_ITEM(ls.literales, i) == true )
                            ERROR ( "You cannot pass literal values as out parameters." );
                        else if ( SUBPROG_PARAM_CLASS(tipo, i) == "ref" && paramTipo != reqTipo )
                            ERROR ( "Cannot make type conversion for parameter #" || i || " of '" || THIS.hident || "' (Out parameter types must match)" );
                        else
                            ADD_INST ( "param_" || SUBPROG_PARAM_CLASS(tipo, i) || " " || TYPECAST(expr, paramTipo, reqTipo) );
                        i = i + 1;
                    }
                }
            }
        }
    }
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
        THIS.literal = d.literal;
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
        d.hliteral = c.literal;
    }
    d();
    {
        THIS.nombre = d.nombre;
        THIS.tipo = d.tipo;
        THIS.gfalse = d.gfalse;
        THIS.gtrue = d.gtrue;
        THIS.literal = d.literal;
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
            TYPECAST( c.nombre, c.tipo, NEW_BASIC_TYPE(BOOLEXPR), c.gtrue , c.gfalse );
            TYPECAST( THIS.hnombre, THIS.htipo, NEW_BASIC_TYPE(BOOLEXPR), THIS.hgtrue , THIS.hgfalse );
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
            THIS.literal = false;
        }
    } else {
        THIS.nombre = THIS.hnombre;
        THIS.tipo = THIS.htipo;
        THIS.gfalse = THIS.hgfalse;
        THIS.gtrue = THIS.hgtrue;
        THIS.literal = THIS.hliteral;
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
        c.hliteral = r.literal;
    }
    c();
    {
        THIS.nombre = c.nombre;
        THIS.tipo = c.tipo;
        THIS.gfalse = c.gfalse;
        THIS.gtrue = c.gtrue;
        THIS.literal = c.literal;
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
            TYPECAST( r.nombre, r.tipo, NEW_BASIC_TYPE(BOOLEXPR), r.gtrue , r.gfalse );
            TYPECAST( THIS.hnombre, THIS.htipo, NEW_BASIC_TYPE(BOOLEXPR), THIS.hgtrue , THIS.hgfalse );
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
            THIS.literal = false;
        }
    } else {
        // Si es vacio
        THIS.nombre = THIS.hnombre;
        THIS.tipo = THIS.htipo;
        THIS.gfalse = THIS.hgfalse;
        THIS.gtrue = THIS.hgtrue;
        THIS.literal = THIS.hliteral;
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
        r.hliteral = a.literal;
    }
    r();
    {
        THIS.nombre = r.nombre;
        THIS.tipo = r.tipo;
        THIS.gfalse = r.gfalse;
        THIS.gtrue = r.gtrue;
        THIS.literal = r.literal;
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
            if ( ! IS_NUMERIC(THIS.htipo) ) {
            	ERROR("Relational operand '" || THIS.hnombre || "' is not scalar");
            }
            else if ( ! IS_NUMERIC(a.tipo) )
            {
            	ERROR("Relational operand '" || a.nombre || "' is not scalar");
            }
            else
            {

                if ( IS_REAL(THIS.htipo) || IS_REAL(a.tipo) )
                {
                    TYPECAST( THIS.hnombre, THIS.htipo, NEW_BASIC_TYPE(REAL) );
                    TYPECAST( a.nombre, a.tipo, NEW_BASIC_TYPE(REAL) );
                }

                r.hgtrue = INIT_LIST(GET_REF());
                ADD_INST( "if " || THIS.hnombre || " " || op.op || " " || a.nombre || " goto ");
                r.hgfalse = INIT_LIST(GET_REF());
                ADD_INST( "goto ");

                r.htipo = NEW_BASIC_TYPE(BOOLEXPR);

            }
        }
        r();
        {
            THIS.nombre = r.hnombre;
            THIS.tipo = r.htipo;
            THIS.gfalse = r.hgfalse;
            THIS.gtrue = r.hgtrue;
            THIS.literal = false;
        }
    } else {
        // Si es vacio
        THIS.nombre = THIS.hnombre;
        THIS.tipo = THIS.htipo;
        THIS.gfalse = THIS.hgfalse;
        THIS.gtrue = THIS.hgtrue;
        THIS.literal = THIS.hliteral;
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
        a.hliteral = t.literal;
    }
    a();
    {
        THIS.nombre = a.nombre;
        THIS.tipo = a.tipo;
        THIS.gfalse = a.gfalse;
        THIS.gtrue = a.gtrue;
        THIS.literal = a.literal;
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
    if (IS_RULE_FIRST ( opl2 ))
    {
        RULE ( opl2 , op )();
        RULE ( termino , t )();
        RULE ( aritmetica_prima , a );
        {
            if ( IS_NUMERIC(THIS.htipo) && IS_NUMERIC(t.tipo) ) {

                if ( IS_REAL(THIS.htipo) || IS_REAL(t.tipo) )
                {
                    TYPECAST( THIS.hnombre, THIS.htipo, NEW_BASIC_TYPE(REAL) );
                    TYPECAST( t.nombre, t.tipo, NEW_BASIC_TYPE(REAL) );
                    a.htipo = NEW_BASIC_TYPE(REAL);
                }
                else
                {
                    a.htipo = NEW_BASIC_TYPE(INTEGER);
                }

                a.hnombre = NEW_IDENT();
                ADD_INST( a.hnombre || " := " || THIS.hnombre || " " || op.op || " " || t.nombre );
                
            } else {
                ERROR("Type mismatch error");
                a.htipo = NEW_BASIC_TYPE(UNKNOWN);
            }
        }
        a();
        {
            THIS.nombre = a.hnombre;
            THIS.tipo = a.htipo;
            THIS.gfalse = a.hgfalse;
            THIS.gtrue = a.hgtrue;
            THIS.literal = false;
        }
    }
    else
    {
        // Si es vacio
        THIS.nombre = THIS.hnombre;
        THIS.tipo = THIS.htipo;
        THIS.gfalse = THIS.hgfalse;
        THIS.gtrue = THIS.hgtrue;
        THIS.literal = THIS.hliteral;
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
        t.hliteral = n.literal;
    }
    t();
    {
        THIS.nombre = t.nombre;
        THIS.tipo = t.tipo;
        THIS.gfalse = t.gfalse;
        THIS.gtrue = t.gtrue;
        THIS.literal = t.literal;
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

                if ( IS_REAL(THIS.htipo) || IS_REAL(f.tipo) )
                {
                    TYPECAST( THIS.hnombre, THIS.htipo, NEW_BASIC_TYPE(REAL) );
                    TYPECAST( f.nombre, f.tipo, NEW_BASIC_TYPE(REAL) );
                    t.htipo = NEW_BASIC_TYPE(REAL);
                }
                else
                {
                    t.htipo = NEW_BASIC_TYPE(INTEGER);
                }
                
                t.hnombre = NEW_IDENT();
                ADD_INST( t.hnombre || " := " || THIS.hnombre || " " || op.op || " " || f.nombre );
                
            } else {
                ERROR("Type mismatch error");
                t.htipo = NEW_BASIC_TYPE(UNKNOWN);
            }
        }
        t();
        {
            THIS.nombre = t.hnombre;
            THIS.tipo = t.htipo;
            THIS.gfalse = t.hgfalse;
            THIS.gtrue = t.hgtrue;
            THIS.literal = false;
        }
    } else {
        // Si es vacio
        THIS.nombre = THIS.hnombre;
        THIS.tipo = THIS.htipo;
        THIS.gfalse = THIS.hgfalse;
        THIS.gtrue = THIS.hgtrue;
        THIS.literal = THIS.hliteral;
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

            THIS.literal = false;
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
           THIS.literal = f.literal;
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
                ADD_INST( THIS.nombre || " := 0 - " || f.nombre );
                THIS.tipo = f.tipo;
            }
            else
            {
                ERROR("Type mismatch error");
            }
            THIS.literal = f.literal;
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
           THIS.literal = f.literal;
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
            THIS.literal = false;
            THIS.gtrue = EMPTY_LIST();
            THIS.gfalse = EMPTY_LIST();
        }
    }
    else if (IS_FIRST ( INTEGER ))
    {
        TOKEN token = MATCH ( INTEGER );
        {
            THIS.tipo = NEW_BASIC_TYPE(INTEGER);
            THIS.nombre = token.value;
            THIS.literal = true;
            THIS.gtrue = EMPTY_LIST();
            THIS.gfalse = EMPTY_LIST();
        }
    }
    else if (IS_FIRST ( REAL ))
    {
        TOKEN token = MATCH ( REAL );
        {
            THIS.tipo = NEW_BASIC_TYPE(REAL);
            THIS.nombre = token.value;
            THIS.literal = true;
            THIS.gtrue = EMPTY_LIST();
            THIS.gfalse = EMPTY_LIST();
        }
    }
    else if (IS_RULE_FIRST ( booleano ))
    {
        RULE ( booleano , token )();
        {
            THIS.tipo = NEW_BASIC_TYPE(BOOLEAN);
            THIS.nombre = token.value;
            THIS.literal = true;
            THIS.gtrue = EMPTY_LIST();
            THIS.gfalse = EMPTY_LIST();
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
            THIS.gfalse = e.gfalse;
            THIS.gtrue = e.gtrue;
            THIS.literal = e.literal;
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
        RULE  ( parametros_llamadas , p );
        {
            p.hident = THIS.hident;
            p.hrequireFunc = true;
        }
        p();
        RULE  ( acceso_a_array_opcional, arr );
        {
            ADD_INST ( "call " || THIS.hident );
            arr.hnombre = NEW_IDENT();
            arr.htipo = p.tipoRetorno;
            ADD_INST ( "store_function_ret " || arr.hnombre );
        }
        arr();
        {
            THIS.nombre = arr.nombre;
            THIS.tipo = arr.tipo;
        }
    }
    else if (IS_RULE_FIRST ( acceso_a_array )) {
        RULE  ( acceso_a_array , a );
        {
            a.hident = THIS.hident;
            a.htipo = NEW_BASIC_TYPE(UNKNOWN);
        }
        a();
        {
            THIS.nombre = NEW_IDENT ();
            ADD_INST ( THIS.nombre || " := " || THIS.hident || "[" || a.offset || "]" );
            THIS.tipo = a.tipo;
        }
    }
    else
    {
        // Variable
        THIS.nombre = THIS.hident;

        if ( ST_EXISTS(THIS.hident) == false )
        {
            ERROR( "Unknow identifier " || THIS.hident );
            THIS.tipo = NEW_BASIC_TYPE ( UNKNOWN );
        }
        else
        {
            THIS.tipo = ST_GET_TYPE ( THIS.hident );
        }
    }
}

DEFINE_RULE(acceso_a_array_opcional,
            FIRST(
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
    if ( IS_RULE_FIRST(acceso_a_array) )
    {
        RULE ( acceso_a_array, arr );
        {
            arr.hident = "`function return value`";
            arr.htipo = THIS.htipo;
        }
        arr ();
        {
            THIS.nombre = NEW_IDENT ();
            ADD_INST ( THIS.nombre || " := " || THIS.hnombre || "[" || arr.offset || "]" );
            THIS.tipo = arr.tipo;
        }
    }
    else
    {
        // Vacío.
        THIS.nombre = THIS.hnombre;
        THIS.tipo = THIS.htipo;
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
    RULE  ( resto_lista_expr , resto )();
    {
        THIS.exprs = JOIN(INIT_LIST(e.nombre), resto.exprs);
        THIS.tipos = JOIN(INIT_LIST(e.tipo), resto.tipos);
        THIS.literales = JOIN(INIT_LIST(e.literal), resto.literales);
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
        RULE  ( expresion , e )();
        RULE  ( resto_lista_expr , resto ) ();
        {
            THIS.exprs = JOIN(INIT_LIST(e.nombre), resto.exprs);
            THIS.tipos = JOIN(INIT_LIST(e.tipo), resto.tipos);
            THIS.literales = JOIN(INIT_LIST(e.literal), resto.literales);
        }
    }
    else
    {
        THIS.exprs = EMPTY_LIST();
        THIS.tipos = EMPTY_LIST();
        THIS.literales = EMPTY_LIST();
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
    if (IS_FIRST ( RESERVED, "true" ))
    {
        TOKEN token = MATCH ( RESERVED, "true" );
        THIS.value = token.value;
    }
    else if (IS_FIRST ( RESERVED, "false" ))
    {
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
