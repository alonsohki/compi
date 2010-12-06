#include "CRules.h"

DEFINE_RULE(programa)
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

DEFINE_RULE(declaraciones)
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

DEFINE_RULE(lista_de_ident)
{
	MATCH ( IDENTIFIER );
	RULE  ( resto_lista_ident )();
}

DEFINE_RULE(resto_lista_ident)
{
	MATCH ( SEPARATOR, "," );
	MATCH ( IDENTIFIER );
	RULE  ( resto_lista_ident )();
	// vacio
}

DEFINE_RULE(tipo)
{
	// Falta arreglar <- decidir un camino u otro
	MATCH ( RESERVED, "entero" );
	MATCH ( RESERVED, "real" );
	MATCH ( RESERVED, "booleano" );
	MATCH ( RESERVED, "array" );
	MATCH ( RESERVED, "de" );
	RULE  ( tipo )();
}

DEFINE_RULE(decl_de_subprogs)
{
	RULE  ( decl_de_procedimiento )();
	RULE  ( decl_de_funcion )();
	RULE  ( decl_de_subprogs )();
	// Vacio ?
}

DEFINE_RULE(decl_de_procedimiento)
{
	RULE  ( cabecera_proc edimiento )();
	RULE  ( declaraciones)();
	MATCH ( RESERVED, "comienzo" );
	RULE  ( lista_de_sentencias )();
	MATCH ( RESERVED, "fin" );
	MATCH ( SEPARATOR, ";" );
}

DEFINE_RULE(decl_de_funcion)
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

DEFINE_RULE(cabecera_procedimiento)
{
	MATCH ( RESERVED, "prodecimiento" );
	MATCH ( IDENTIFIER );
	RULE  ( argumentos )();
}

DEFINE_RULE(cabecera_funcion)
{
	MATCH ( RESERVED, "funcion" );
	MATCH ( IDENTIFIER );
	RULE  ( argumentos )();
	MATCH ( RESERVED, "retorna" );
	RULE  ( tipo )();
}

DEFINE_RULE(argumentos)
{
	MATCH ( SEPARATOR, "(" );
	RULE  ( lista_de_param )();
	MATCH ( SEPARATOR, ")" );
	// Vacío ?
}

DEFINE_RULE(lis_de_param)
{
	RULE  ( lista_de_ident )();
	MATCH ( SEPARATOR, ":" );
	RULE  (clase_par)();
	RULE  (tipo)();
	RULE  (resto_lis_de_param)();
}

DEFINE_RULE(resto_lis_de_param)
{
	RULE  ( lista_de_ident )();
	MATCH ( SEPARATOR, ";" );
	RULE  (  clase_par)();
	RULE  ( tipo  )();
	if ( IS_FIRST ( ( IDENTIFIER ) ) )
	{
		RULE  ( resto_lis_de_param )();
	}
}

DEFINE_RULE(clase_par)
{
	if ( IS_FIRST( ( RESERVED, "entrada" ) ) )
	{
		MATCH ( RESERVED, "entrada" );
		RULE  ( clase_par_prima )();
	}
	else if ( IS_FIRST( ( RESERVED, "salida" ) ) )
	{
		MATCH ( RESERVED, "salida" );
	}
	else
	{
		// FAIL SIGNIFICA ERROR
	}
}

DEFINE_RULE(clase_par_prima)
{
	if ( IS_FIRST( ( RESERVED, "salida" ) ) )
	{
			MATCH ( RESERVED, "salida" );
	}
	else
	{
		// Else asumir clase_par_prima -> epsilon
	}
}

DEFINE_RULE(lista_de_sentencias_prima)
{
	/* RULE  ( lista_de_sentencias, ls )();
	ls.hinloop = FALSE;
	ls(); */
	RULE  ( lista_de_sentencias )();

}

DEFINE_RULE(lista_de_sentencias)
{
	if ( IS_FIRST ( /* rellenar */ ) )
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

DEFINE_RULE(sentencia)
{
	// por ahora vacia
}
























