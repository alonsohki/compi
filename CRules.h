#ifndef CRULES_H
#define	CRULES_H

#include "CRules_internal.h"

// Declaración de las reglas.
// DECLARE_RULE(nombre de la regla, atributos);
// FIRST_RULE_IS(símbolo inicial de la gramática);
DECLARE_RULE(programa);
DECLARE_RULE(declaraciones);
DECLARE_RULE(tipo, tipo);
DECLARE_RULE(lista_de_ident, ids);
DECLARE_RULE(resto_lista_ident, ids);
DECLARE_RULE(decl_de_subprogs);
DECLARE_RULE(decl_de_subprograma);
DECLARE_RULE(decl_de_procedimiento);
DECLARE_RULE(decl_de_funcion);
DECLARE_RULE(cabecera_procedimiento);
DECLARE_RULE(cabecera_funcion);
DECLARE_RULE(argumentos);
DECLARE_RULE(lista_de_param);
DECLARE_RULE(resto_lis_de_param);
DECLARE_RULE(clase_par, clase);
DECLARE_RULE(clase_par_prima, clase);
DECLARE_RULE(lista_de_sentencias_prima);
DECLARE_RULE(lista_de_sentencias, salir_si, hinloop);
DECLARE_RULE(sentencia);
DECLARE_RULE(acceso_a_array);
DECLARE_RULE(parametros_llamada);
DECLARE_RULE(expresiones);
DECLARE_RULE(variable, nombre);
DECLARE_RULE(expresion, nombre, ltrue, lfalse);
DECLARE_RULE(expresion_prima, hnombre, nombre, ltrue, lfalse);
DECLARE_RULE(opl2, op);
DECLARE_RULE(opl1, op);
DECLARE_RULE(oprel, op);
DECLARE_RULE(expresion_simple, nombre);
DECLARE_RULE(expresion_simple_prima, nombre, hnombre);
DECLARE_RULE(termino, nombre);
DECLARE_RULE(termino_prima, nombre, hnombre);
DECLARE_RULE(factor, nombre);
DECLARE_RULE(M, ref);

FIRST_RULE_IS(programa);

#endif	/* CRULES_H */

