#include <cstring>
#include "CTokenizer.h"
#include "CClassifier.h"
#include "util.h"

const int CTokenizer::ms_iTransitions [ CTokenizer::NUMSTATES ] [ CClassifier::GROUP_MAX ] = {
/*          ALPHA  DIGIT  eE     DOT    UNDER  EQUAL  LT     GT     PLUS   MINUS  STAR   SLASH  CR     LF     SPACE  SEP    UNKN */
/* q0  */ {  4,    13,     4,    -1,    -1,     9,     9,     9,     8,     8,     8,     7,     2,     3,     1,     6,    -1   },
/* q1  */ { -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     1,    -1,    -1   },
/* q2  */ { -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,    -1,    -1,    -1   },
/* q3  */ { -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1   },
/* q4  */ {  4,     4,     4,    -1,     5,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1   },
/*          ALPHA  DIGIT  eE     DOT    UNDER  EQUAL  LT     GT     PLUS   MINUS  STAR   SLASH  CR     LF     SPACE  SEP    UNKN */
/* q5  */ {  4,     4,     4,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1   },
/* q6  */ { -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1   },
/* q7  */ { -1,    -1,    -1,    -1,    -1,     8,    -1,    -1,    -1,    -1,    10,    -1,    -1,    -1,    -1,    -1,    -1   },
/* q8  */ { -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1   },
/* q9  */ { -1,    -1,    -1,    -1,    -1,     8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1   },
/*          ALPHA  DIGIT  eE     DOT    UNDER  EQUAL  LT     GT     PLUS   MINUS  STAR   SLASH  CR     LF     SPACE  SEP    UNKN */
/* q10 */ { 10,    10,    10,    10,    10,    10,    10,    10,    10,    10,    11,    10,    10,    10,    10,    10,    10   },
/* q11 */ { 10,    10,    10,    10,    10,    10,    10,    10,    10,    10,    11,    12,    10,    10,    10,    10,    10   },
/* q12 */ { -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1   },
/* q13 */ { -1,    13,    -1,    14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1   },
/* q14 */ { -1,    15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1   },
/*          ALPHA  DIGIT  eE     DOT    UNDER  EQUAL  LT     GT     PLUS   MINUS  STAR   SLASH  CR     LF     SPACE  SEP    UNKN */
/* q15 */ { -1,    15,    16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1   },
/* q16 */ { -1,    18,    -1,    -1,    -1,    -1,    -1,    -1,    17,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1   },
/* q17 */ { -1,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1   },
/* q18 */ { -1,    18,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1   }
};

const CTokenizer::ETokenType CTokenizer::ms_eFinalStates [ CTokenizer::NUMSTATES ] = {
/* q0  */ UNKNOWN,
/* q1  */ SPACE,
/* q2  */ NEWLINE,
/* q3  */ NEWLINE,
/* q4  */ IDENTIFIER,
/* q5  */ UNKNOWN,
/* q6  */ SEPARATOR,
/* q7  */ OPERATOR,
/* q8  */ OPERATOR,
/* q9  */ OPERATOR,
/* q10 */ UNKNOWN,
/* q11 */ UNKNOWN,
/* q12 */ COMMENT,
/* q13 */ INTEGER,
/* q14 */ UNKNOWN,
/* q15 */ REAL,
/* q16 */ UNKNOWN,
/* q17 */ UNKNOWN,
/* q18 */ REAL
};

CTokenizer::fn_stateTrigger CTokenizer::ms_fnTriggers [ CTokenizer::NUMSTATES ] = {
/* q0  */ 0,
/* q1  */ 0,
/* q2  */ 0,
/* q3  */ 0,
/* q4  */ 0,
/* q5  */ 0,
/* q6  */ 0,
/* q7  */ 0,
/* q8  */ 0,
/* q9  */ 0,
/* q10 */ 0,
/* q11 */ 0,
/* q12 */ 0,
/* q13 */ 0,
/* q14 */ 0,
/* q15 */ 0,
/* q16 */ 0,
/* q17 */ 0,
/* q18 */ 0
};

const char* CTokenizer::ms_szErrors [ CTokenizer::NUMSTATES ] = {
/* q0  */ "Unknown token",
/* q1  */ 0,
/* q2  */ 0,
/* q3  */ 0,
/* q4  */ 0,
/* q5  */ "Invalid identifier: Must start with an alphabetic character and followed by alphanumeric characters or single inserted underscores.",
/* q6  */ 0,
/* q7  */ 0,
/* q8  */ 0,
/* q9  */ 0,
/* q10 */ "Non closed comment",
/* q11 */ "Non closed comment",
/* q12 */ 0,
/* q13 */ 0,
/* q14 */ "Invalid real number format. It must be in scientific notation.",
/* q15 */ 0,
/* q16 */ "Invalid real number format. It must be in scientific notation.",
/* q17 */ "Invalid real number format. It must be in scientific notation.",
/* q18 */ 0
};

const char* CTokenizer::ms_szReserved [ ] = {
    "programa",
    "procedimiento",
    "comienzo",
    "fin",
    "variables",
    "entero",
    "real",
    "entrada",
    "salida",
    "si",
    "entonces",
    "hacer",
    "mientras",
    "salir",
    "get",
    "put_line",
    "true",
    "false",
    "retorna",
    "retornar",
    "funcion",
    "array",
    "de"
};

/////////////////////////////////////////////////////////

CTokenizer::CTokenizer ( std::istream& isInput )
: m_buffer ( isInput )
, m_uiState ( 0 )
, m_isInput ( isInput )
{
}

CTokenizer::~CTokenizer() {
}

bool CTokenizer::ReadToken ( SToken* pToken )
{
    bool bFailed = false;
    unsigned char c;
    CClassifier classifier;
    CClassifier::ECharacterGroup eGroup;

    // Inicializamos el buffer del token para ir añadiendo lo que leamos.
    if ( pToken != 0 )
    {
        memset ( pToken->value, 0, sizeof ( pToken->value ) );
        pToken->uiValueLength = 0;
        pToken->uiCol = m_buffer.GetCol ();
        pToken->uiLine = m_buffer.GetLine ();
    }

    // Inicializamos el autómata.
    m_uiState = 0;

    do
    {
        // Leemos el siguiente caracter del buffer.
        if ( m_buffer.Get ( &c ) == false )
        {
            bFailed = ( m_uiState == 0 );
            break;
        }

        // Clasificamos el caracter y comprobamos que exista una transición
        // desde el estado actual.
        eGroup = classifier [ c ];
        if ( ms_iTransitions [ m_uiState ] [ (unsigned int)eGroup ] == -1 )
        {
            if ( m_uiState != 0 )
                m_buffer.Rollback ();
            else
            {
                // Añadimos el caracter recientemente leído al token.
                if ( pToken != 0 )
                {
                    pToken->value [ pToken->uiValueLength ] = c;
                    pToken->uiValueLength++;
                }
            }
            break;
        }

        // Cambiamos de estado.
        m_uiState = ms_iTransitions [ m_uiState ] [ (unsigned int)eGroup ];

        // Añadimos el caracter recientemente leído al token.
        if ( pToken != 0 )
        {
            pToken->value [ pToken->uiValueLength ] = c;
            pToken->uiValueLength++;
        }

        if ( ms_fnTriggers [ m_uiState ] != 0 )
            ((*this).*(ms_fnTriggers [ m_uiState ])) ( c );
    } while ( true );

    // Clasificamos el tipo token.
    if ( pToken != 0 )
    {
        pToken->eType = ms_eFinalStates [ m_uiState ];
        pToken->uiState = m_uiState;
    }

    return !bFailed;
}

bool CTokenizer::NextToken ( SToken* pToken, bool bIgnoreWhiteSpaces )
{
    SToken token;
    bool bContinueSearching = true;
    bool bRetval = true;

    do
    {
        bRetval = ReadToken ( &token );
        if ( bRetval == false )
            break;

        switch ( token.eType )
        {
            case NEWLINE:
            case COMMENT:
            case SPACE:
                if ( bIgnoreWhiteSpaces == false )
                    bContinueSearching = false;
                break;
            case IDENTIFIER:
                if ( IsReserved ( token.value ) == true )
                    token.eType = RESERVED;
                bContinueSearching = false;
                break;
            default:
                bContinueSearching = false;
        }
    } while ( bContinueSearching == true );

    if ( pToken != 0 )
        *pToken = token;

    return bRetval;
}

const char* CTokenizer::NameThisToken ( ETokenType eType ) const
{
#define TOKTYPE(x) case x : return #x ; break
    switch ( eType )
    {
        TOKTYPE(INTEGER);
        TOKTYPE(REAL);
        TOKTYPE(IDENTIFIER);
        TOKTYPE(RESERVED);
        TOKTYPE(OPERATOR);
        TOKTYPE(COMMENT);
        TOKTYPE(SPACE);
        TOKTYPE(NEWLINE);
        TOKTYPE(SEPARATOR);
        TOKTYPE(UNKNOWN);
        default:
            return "";
    }
#undef TOKTYPE
}

bool CTokenizer::IsReserved ( const char* szTokenValue ) const
{
    for ( unsigned int i = 0; i < NUMELEMS(ms_szReserved); ++i )
    {
        if ( strcasecmp ( szTokenValue, ms_szReserved [ i ] ) == 0 )
            return true;
    }
    return false;
}
