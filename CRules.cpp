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
    MATCH ( RESERVED, "variables" );
    RULE  ( lista_de_ident )();
    MATCH ( SEPARATOR, ":" );
    RULE  ( tipo )();
    MATCH ( SEPARATOR, ";" );
    /* Falta semántica */
    RULE  ( declaraciones )();
    // Esto puede ser vacio
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
    MATCH ( SEPARATOR, "," );
    MATCH ( IDENTIFIER );
    RULE  ( resto_lista_ident )();
    // vacio
}

DEFINE_RULE(tipo,
            FIRST(
                 ),
            NEXT(
                )
)
{
    // Falta arreglar <- decidir un camino u otro
    MATCH ( RESERVED, "entero" );
    MATCH ( RESERVED, "real" );
    MATCH ( RESERVED, "booleano" );
    MATCH ( RESERVED, "array" );
    MATCH ( RESERVED, "de" );
    RULE  ( tipo )();
}

DEFINE_RULE(decl_de_subprogs,
            FIRST(
                 ),
            NEXT(
                )
)
{
    RULE  ( decl_de_procedimiento )();
    RULE  ( decl_de_funcion )();
    RULE  ( decl_de_subprogs )();
    // Vacio ?
}

DEFINE_RULE(decl_de_procedimiento,
            FIRST(
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
    MATCH ( RESERVED, "prodecimiento" );
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
    MATCH ( SEPARATOR, "(" );
    RULE  ( lista_de_param )();
    MATCH ( SEPARATOR, ")" );
    // Vacío ?
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
    RULE  (clase_par)();
    RULE  (tipo)();
    RULE  (resto_lis_de_param)();
}

DEFINE_RULE(resto_lis_de_param,
            FIRST(
                 ),
            NEXT(
                )
)
{
    RULE  ( lista_de_ident )();
    MATCH ( SEPARATOR, ";" );
    RULE  (  clase_par )();
    RULE  ( tipo  )();
    if ( IS_FIRST ( IDENTIFIER ) )
    {
        RULE  ( resto_lis_de_param )();
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
























