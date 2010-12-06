#include "CRules.h"

DEFINE_RULE (programa,
            FIRST(
                 ),
            NEXT(
                )
)
{
    MATCH ( RESERVED,  "programa" );
    MATCH ( IDENTIFIER );
    RULE  ( declaraciones )();
    RULE  ( decl_de_subprogs )();
    MATCH ( RESERVED, "comienzo" );
    RULE  ( lista_de_sentencias )();
    MATCH ( RESERVED, "fin" );
    MATCH ( SEPARATOR, ";" );
}

DEFINE_RULE(declaraciones,
            FIRST(
                 ),
            NEXT(
                )
)
{
    if ( IS_FIRST ( RESERVED, "variables" ) )
    {
        MATCH ( RESERVED, "variables" );
        RULE  ( lista_de_ident )();
        MATCH ( SEPARATOR, ":" );
        RULE  ( tipo )();
        MATCH ( SEPARATOR, ";" );
        /* Falta semántica */
        RULE  ( declaraciones )();
    }
    else
    {
        // Esto ES vacio
    }
}

DEFINE_RULE(lista_de_ident,
            FIRST(
                 ),
            NEXT(
                )
)
{
    MATCH ( IDENTIFIER );
    RULE  ( resto_lista_ident )();
}

DEFINE_RULE(resto_lista_ident,
            FIRST(
                 ),
            NEXT(
                )
)
{
    if ( IS_FIRST ( SEPARATOR, "," ) )
    {
        MATCH ( SEPARATOR, "," );
        MATCH ( IDENTIFIER );
        RULE  ( resto_lista_ident )();
    }
    else
    {
        // vacio
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
                )
)
{
    if ( IS_FIRST ( RESERVED, "entero" ) )
    {
        MATCH ( RESERVED, "entero" );
    }
    else if ( IS_FIRST ( RESERVED, "real" ) )
    {
        MATCH ( RESERVED, "real" );
    }
    else if ( IS_FIRST ( RESERVED, "booleano" ) )
    {
        MATCH ( RESERVED, "booleano" );
    }
    else if ( IS_FIRST ( RESERVED, "array" ) )
    {
        MATCH ( RESERVED, "array" );
        MATCH ( RESERVED, "de" );
        RULE  ( tipo )();
    }
    else
    {
        // Error.
    }
}

DEFINE_RULE(decl_de_subprogs,
            FIRST(
                 ),
            NEXT(
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
                )
)
{
    RULE  ( cabecera_procedimiento )();
    RULE  ( declaraciones )();
    MATCH ( RESERVED, "comienzo" );
    RULE  ( lista_de_sentencias )();
    MATCH ( RESERVED, "fin" );
    MATCH ( SEPARATOR, ";" );
}

DEFINE_RULE(decl_de_funcion,
            FIRST(
                    ( RESERVED, "funcion" )
                 ),
            NEXT(
                )
)
{
    RULE  ( cabecera_funcion )();
    RULE  ( declaraciones )();
    MATCH ( RESERVED, "comienzo" );
    RULE  ( lista_de_sentencias )();
    MATCH ( RESERVED, "retorna" );
    RULE  ( expresion )();
    MATCH ( RESERVED, "fin" );
    MATCH ( SEPARATOR, ";" );
}

DEFINE_RULE(cabecera_procedimiento,
            FIRST(
                 ),
            NEXT(
                )
)
{
    MATCH ( RESERVED, "procedimiento" );
    MATCH ( IDENTIFIER );
    RULE  ( argumentos )();
}

DEFINE_RULE(cabecera_funcion,
            FIRST(
                 ),
            NEXT(
                )
)
{
    MATCH ( RESERVED, "funcion" );
    MATCH ( IDENTIFIER );
    RULE  ( argumentos )();
    MATCH ( RESERVED, "retorna" );
    RULE  ( tipo )();
}

DEFINE_RULE(argumentos,
            FIRST(
                 ),
            NEXT(
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
                 ),
            NEXT(
                )
)
{
    RULE  ( lista_de_ident )();
    MATCH ( SEPARATOR, ":" );
    RULE  ( clase_par )();
    RULE  ( tipo )();
    RULE  ( resto_lis_de_param )();
}

DEFINE_RULE(resto_lis_de_param,
            FIRST(
                 ),
            NEXT(
                )
)
{
    if ( IS_FIRST ( SEPARATOR, ";" ) )
    {
        MATCH ( SEPARATOR, ";" );
        RULE  ( lista_de_ident )();
        MATCH ( SEPARATOR, ":" );
        RULE  ( clase_par )();
        RULE  ( tipo )();
        RULE  ( resto_lis_de_param )();
    }
    else
    {
    }
}

DEFINE_RULE(clase_par,
            FIRST(
                 ),
            NEXT(
                )
)
{
    if ( IS_FIRST( RESERVED, "entrada" ) )
    {
        MATCH ( RESERVED, "entrada" );
        RULE  ( clase_par_prima )();
    }
    else if ( IS_FIRST( RESERVED, "salida" ) )
    {
        MATCH ( RESERVED, "salida" );
    }
    else
    {
        // FAIL SIGNIFICA ERROR
    }
}

DEFINE_RULE(clase_par_prima,
            FIRST(
                 ),
            NEXT(
                )
)
{
    if ( IS_FIRST( RESERVED, "salida" ) )
    {
            MATCH ( RESERVED, "salida" );
    }
    else
    {
        // Else asumir clase_par_prima -> epsilon
    }
}

DEFINE_RULE(lista_de_sentencias_prima,
            FIRST(
                 ),
            NEXT(
                )
)
{
    /* RULE  ( lista_de_sentencias, ls )();
    ls.hinloop = FALSE;
    ls(); */
    RULE  ( lista_de_sentencias )();
}

DEFINE_RULE(lista_de_sentencias,
            FIRST(
                 ),
            NEXT(
                )
)
{
    if ( IS_RULE_FIRST(sentencia) )
    {
        RULE  ( sentencia );
        MATCH ( SEPARATOR, ";" );
        RULE  ( lista_de_sentencias );
    }
    else
    {
        // Vacio
    }
}

DEFINE_RULE(sentencia,
            FIRST(
                 ),
            NEXT(
                )
)
{
    // por ahora vacia
}

DEFINE_RULE(expresion,
            FIRST(
                 ),
            NEXT(
                )
)
{
    // por ahora vacia
}
























