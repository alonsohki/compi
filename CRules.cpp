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
    MATCH ( IDENTIFIER );
    RULE  ( declaraciones )();
    RULE  ( decl_de_subprogs )();
    MATCH ( RESERVED, "comienzo" );
    RULE  ( lista_de_sentencias_prima )();
    MATCH ( RESERVED, "fin" );
    MATCH ( SEPARATOR, ";" );
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
                    ( IDENTIFIER )
                 ),
            NEXT(
                    ( RESERVED, ":" )
                )
)
{
    MATCH ( IDENTIFIER );
    RULE  ( resto_lista_ident )();
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
        MATCH ( SEPARATOR, "[" );
        RULE  ( lista_de_enteros )();
        MATCH ( SEPARATOR, "]" );
        MATCH ( RESERVED, "de" );
        RULE  ( tipo )();
    }
    else
    {
        // Error.
    }
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
    MATCH ( INTEGER );
    RULE ( resto_lista_enteros )();
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
        MATCH ( INTEGER );
        RULE ( resto_lista_enteros )();
    }
    else
    {
        // Vacío.
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
    RULE  ( cabecera_procedimiento )();
    RULE  ( declaraciones )();
    MATCH ( RESERVED, "comienzo" );
    RULE  ( lista_de_sentencias_prima )();
    MATCH ( RESERVED, "fin" );
    MATCH ( SEPARATOR, ";" );
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
    RULE  ( cabecera_funcion )();
    RULE  ( declaraciones )();
    MATCH ( RESERVED, "comienzo" );
    RULE  ( lista_de_sentencias_prima )();
    MATCH ( RESERVED, "retornar" );
    RULE  ( expresion )();
    MATCH ( RESERVED, "fin" );
    MATCH ( SEPARATOR, ";" );
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
    MATCH ( IDENTIFIER );
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
    MATCH ( IDENTIFIER );
    RULE  ( argumentos )();
    MATCH ( RESERVED, "retorna" );
    RULE  ( tipo )();
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
    RULE  ( lista_de_ident )();
    MATCH ( SEPARATOR, ":" );
    RULE  ( clase_param )();
    RULE  ( tipo )();
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
        RULE  ( lista_de_ident )();
        MATCH ( SEPARATOR, ":" );
        RULE  ( clase_param )();
        RULE  ( tipo )();
        RULE  ( resto_lis_de_param )();
    }
    else
    {
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
        RULE  ( clase_param_prima )();
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
    }
    else
    {
        // Else asumir clase_param_prima -> epsilon
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
    /* RULE  ( lista_de_sentencias, ls )();
    ls.hinloop = FALSE;
    ls(); */
    RULE  ( lista_de_sentencias )();
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
        RULE  ( sentencia )();
        RULE  ( lista_de_sentencias )();
    }
    else
    {
        // Vacio
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
        MATCH ( IDENTIFIER );
        RULE  ( expresiones )();
        MATCH ( SEPARATOR, ";" );
    }
    else if (IS_FIRST ( RESERVED, "si" ) )
    {
        MATCH ( RESERVED, "si" );
        RULE  ( expresion )();
        MATCH ( RESERVED, "entonces" );
        RULE  ( M )();
        RULE  ( lista_de_sentencias )();
        MATCH ( RESERVED, "fin");
        MATCH ( RESERVED, "si");
        RULE  ( M )();
        MATCH ( SEPARATOR, ";");
    }
    else if (IS_FIRST ( RESERVED, "hacer" ) )
    {
        MATCH ( RESERVED, "hacer" );
        RULE  ( M )();
        RULE  ( lista_de_sentencias, ls );
        ls.hinloop = true;
        ls();
        MATCH ( RESERVED, "mientras" );
        RULE  ( expresion )();
        MATCH ( RESERVED, "fin" );
        MATCH ( RESERVED, "hacer" );
        RULE  ( M )();
        MATCH ( SEPARATOR, ";");
    }
    else if (IS_FIRST ( RESERVED, "salir"))
    {
        MATCH ( RESERVED, "salir" );
        MATCH ( RESERVED, "si" );
        RULE  ( expresion )();
        RULE  ( M )();
        MATCH ( SEPARATOR, ";");
    }
    else if (IS_FIRST ( RESERVED, "get"))
    {
        MATCH ( RESERVED, "get" );
        MATCH ( SEPARATOR, "(" );
        MATCH ( IDENTIFIER );
        RULE  ( expresiones )();
        MATCH ( SEPARATOR, ")");
        MATCH ( SEPARATOR, ";");
    }
    else if (IS_FIRST ( RESERVED, "put_line"))
    {
        MATCH ( RESERVED, "put_line" );
        MATCH ( SEPARATOR, "(" );
        RULE  ( expresion )();
        MATCH ( SEPARATOR, ")");
        MATCH ( SEPARATOR, ";");
    }
    else {
        // error
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
        RULE  ( expresion )();
    }
    else if (IS_RULE_FIRST( acceso_a_array ))
    {
        RULE  ( acceso_a_array )();
        MATCH ( OPERATOR, "=" );
        RULE  ( expresion )();
    }
    else if (IS_RULE_FIRST( parametros_llamadas ))
    {
        RULE  ( parametros_llamadas )();
    }
    else {
        // error
    }
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
    else
    {
        // error
    }
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
    else
    {
        // error
    }
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
    else
    {
        // error
    }
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
        // vacioooooo
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
    else
    {
        // error
    }
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
    else
    {
        // error
    }
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
    else
    {
        // fail
    }
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
    else
    {
        // error
    }
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
