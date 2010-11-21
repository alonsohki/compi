#include "CRules.h"

DEFINE_RULE(programa)
{
    MATCH ( RESERVED,  "programa" );
    TOKEN id = MATCH ( IDENTIFIER );
    ADD_INST ( "prog " || id.value );
}