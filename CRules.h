#ifndef CRULES_H
#define	CRULES_H

#include "CRules_internal.h"

// Declaración de las reglas.
// DECLARE_RULE(nombre de la regla, atributos);
// FIRST_RULE_IS(símbolo inicial de la gramática);
DECLARE_RULE(programa);
DECLARE_RULE(declaraciones);
DECLARE_RULE(lista_de_ident, ids);
DECLARE_RULE(resto_lista_ident, ids);
DECLARE_RULE(tipo, tipo);
DECLARE_RULE(lista_de_enteros, ints);
DECLARE_RULE(resto_lista_enteros, ints);
DECLARE_RULE(decl_de_subprogs);
DECLARE_RULE(decl_de_procedimiento);
DECLARE_RULE(decl_de_funcion);
DECLARE_RULE(cabecera_procedimiento);
DECLARE_RULE(cabecera_funcion, tipo);
DECLARE_RULE(argumentos);
DECLARE_RULE(lista_de_param);
DECLARE_RULE(resto_lis_de_param);
DECLARE_RULE(clase_param, clase);
DECLARE_RULE(clase_param_prima, clase);
DECLARE_RULE(lista_de_sentencias_prima);
DECLARE_RULE(lista_de_sentencias, salir_si, hinloop);
DECLARE_RULE(sentencia, salir_si, hinloop);
DECLARE_RULE(id_o_array, hident, nombre);
DECLARE_RULE(asignacion_o_llamada, hident );
DECLARE_RULE(acceso_a_array);
DECLARE_RULE(parametros_llamadas , parametros);
DECLARE_RULE(expresion, nombre, tipo, gtrue, gfalse );
DECLARE_RULE(disyuncion);
DECLARE_RULE(disyuncion_prima);
DECLARE_RULE(conjuncion);
DECLARE_RULE(conjuncion_prima);
DECLARE_RULE(relacional);
DECLARE_RULE(relacional_prima);
DECLARE_RULE(aritmetica);
DECLARE_RULE(aritmetica_prima);
DECLARE_RULE(termino, nombre);
DECLARE_RULE(termino_prima, nombre, hnombre);
DECLARE_RULE(negacion);
DECLARE_RULE(factor);
DECLARE_RULE(factor_prima, tipo, nombre);
DECLARE_RULE(array_o_llamada, hident, nombre);
DECLARE_RULE(lista_de_expr, hident, nombre);
DECLARE_RULE(resto_lista_expr, hident, nombre);
DECLARE_RULE(opl1, op);
DECLARE_RULE(opl2, op);
DECLARE_RULE(oprel, op);
DECLARE_RULE(booleano);
DECLARE_RULE(M, ref);

FIRST_RULE_IS(programa);

#endif	/* CRULES_H */

