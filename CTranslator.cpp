#include <cstring>
#include <vector>
#include "CTranslator.h"
#include "CTokenizer.h"
#include "CRules.h"
#include "CTypeInfo.h"

CTranslator::CTranslator(std::istream& ifsOrig,
                         std::ostream& ofsDest)
: m_isOrig ( ifsOrig )
, m_osDest ( ofsDest )
, m_tokenizer ( ifsOrig )
, m_uiLastIdent ( 0 )
, m_bInPanic ( false )
, m_ePanicStrategy ( PANIC_STRATEGY_FAIL )
, m_bKeepCurrentLookahead ( false )
, m_numErrors ( 0 )
, m_numWarnings ( 0 )
{
}

CTranslator::~CTranslator()
{
}

bool CTranslator::IsOk () const
{
    bool bStreamsAreOpen = !m_isOrig.fail () && !m_osDest.fail ();
    return bStreamsAreOpen;
}

bool CTranslator::Translate ()
{
    try
    {
        // Inicializamos el lookahead.
        NextLookahead ();
        // Regla inicial.
        EXECUTE_FIRST_RULE();
    }
    catch ( Exception& e )
    {
        // Comprobamos si debemos mostrar fila y columna.
        bool bShowLineAndCol = e.GetLine () != 0 || e.GetColumn () != 0;
        Error ( e.GetReason(), e.GetLine(), e.GetColumn(), bShowLineAndCol );
    }

    // No debería quedar nada que leer (aparte de los blancos).
    if ( m_numErrors == 0 && EOFReached () == false )
    {
        Error ( Format ( "Unexpected token: %s", m_lookahead.value ) );
    }

    // Generamos el código si no ha habido errores.
    if ( m_numErrors == 0 )
    {
        unsigned int i = 0;
        for ( std::vector<CString>::const_iterator it = m_vecInstructions.begin ();
              it != m_vecInstructions.end();
              ++it )
        {
            m_osDest << i << ": " << *(*it) << ";" << std::endl;
        }
    }

    // Resultado de la compilación.
    fprintf ( stderr, "Compilation finished. %u errors, %u warnings\n",
              m_numErrors, m_numWarnings );
    return true;
}

void CTranslator::NextLookahead ()
{
    if ( EOFReached() == false && m_bKeepCurrentLookahead == false )
    {
        if ( m_tokenizer.NextToken ( &m_lookahead, true ) == false )
        {
            m_lookahead = CTokenizer::SToken ( CTokenizer::END_OF_FILE, "" );
        }
        else if ( m_lookahead.eType == CTokenizer::UNKNOWN )
        {
            const char* szError = m_tokenizer.GetErrorForToken ( m_lookahead );
            if ( szError == 0 )
                throw Exception ( m_lookahead.uiLine + 1, m_lookahead.uiCol + 1, Format("Unexpected token: %s", m_lookahead.value) );
            else
                throw Exception ( m_lookahead.uiLine + 1, m_lookahead.uiCol + 1, szError );
        }
    }
    m_bKeepCurrentLookahead = false;
}

void CTranslator::Error ( const CString& msg,
                          unsigned int uiLine, unsigned int uiCol,
                          bool bShowLineAndCol )
{
    char szPrefix [ 32 ] = "";
    if ( bShowLineAndCol == true )
    {
        if ( uiLine == 0 && uiCol == 0 )
        {
            snprintf ( szPrefix, NUMELEMS(szPrefix), "[%u:%u] ",
                       m_lookahead.uiLine + 1, m_lookahead.uiCol + 1 );
        }
        else
        {
            snprintf ( szPrefix, NUMELEMS(szPrefix), "[%u:%u] ", uiLine, uiCol );
        }
    }
    fprintf ( stderr, "%sError: %s\n", szPrefix, *msg );
    ++m_numErrors;
}

void CTranslator::Warning ( const CString& msg,
                            unsigned int uiLine, unsigned int uiCol,
                            bool bShowLineAndCol )
{
    char szPrefix [ 32 ] = "";
    if ( bShowLineAndCol == true )
    {
        if ( uiLine == 0 && uiCol == 0 )
        {
            snprintf ( szPrefix, NUMELEMS(szPrefix), "[%u:%u] ",
                       m_lookahead.uiLine + 1, m_lookahead.uiCol + 1 );
        }
        else
        {
            snprintf ( szPrefix, NUMELEMS(szPrefix), "[%u:%u] ", uiLine, uiCol );
        }
    }
    fprintf ( stderr, "%sWarning: %s\n", szPrefix, *msg );
    ++m_numWarnings;
}

bool CTranslator::PanicMode ( CTokenizer::ETokenType eType,
                              const CString& requiredValue,
                              const CTokenizer::SToken* pNextToken )
{
    enum
    {
        DEADLY_PANIC = true,
        PANIC_RECOVER = false
    };

    switch ( m_ePanicStrategy )
    {
        default: case PANIC_STRATEGY_FAIL:
            return DEADLY_PANIC;

        case PANIC_STRATEGY_IGNORE:
            return PANIC_RECOVER;

        case PANIC_STRATEGY_TRYAGAIN:
            m_bKeepCurrentLookahead = true;
            return PANIC_RECOVER;

        case PANIC_STRATEGY_FINDIT:
        {
            bool bFound;
            do
            {
                // Obtenemos el siguiente token, ignorando cualquier error que pudiera
                // obtenerse por encontrar tokens inválidos.
                bool bKeepSearching;
                do
                {
                    bKeepSearching = false;
                    try
                    {
                        NextLookahead ();
                    }
                    catch ( Exception& e )
                    {
                        bKeepSearching = true;
                    }
                } while ( bKeepSearching == true );

                // Comprobamos si coinciden.
                if ( m_lookahead.eType == eType )
                {
                    if ( requiredValue == "" || requiredValue == m_lookahead.value )
                    {
                        bFound = true;
                    }
                }
            } while ( bFound == false && EOFReached() == false );

            if ( bFound == true )
                return PANIC_RECOVER;
            else
                return DEADLY_PANIC;
        }
            
        case PANIC_STRATEGY_FINDNEXT:
        {
            // En modo de pánico de búsqueda del siguiente.
            bool bNextFound;
            do
            {
                // Obtenemos el siguiente token, ignorando cualquier error que pudiera
                // obtenerse por encontrar tokens inválidos.
                bool bKeepSearching;
                do
                {
                    bKeepSearching = false;
                    try
                    {
                        NextLookahead ();
                    }
                    catch ( Exception& e )
                    {
                        bKeepSearching = true;
                    }
                } while ( bKeepSearching == true );

                // Comprobamos si el token leído coincide con alguno de los siguientes.
                bNextFound = false;
                for ( unsigned int i = 0; pNextToken[i].eType != CTokenizer::UNKNOWN; ++i )
                {
                    if ( pNextToken[i].eType == m_lookahead.eType )
                    {
                        if ( pNextToken[i].value[0] != '\0' &&
                             strcmp(pNextToken[i].value, m_lookahead.value) == 0 )
                        {
                            bNextFound = true;
                        }
                    }
                }

                // Si llegamos al final del fichero y este no era ninguno de los
                // siguientes, no podemos recuperarnos del error.
                if ( bNextFound == false && EOFReached() == true )
                    break;
            } while ( bNextFound == false );

            SetInPanic ( bNextFound );

            if ( bNextFound == true )
                return PANIC_RECOVER;
            else
                return DEADLY_PANIC;
        }
    }
}

void CTranslator::Panic ( const CTokenizer::SToken* pNextToken )
{
    SetPanicStrategy ( PANIC_STRATEGY_FINDNEXT );
    Error ( Format ( "Unexpected token '%s'", m_lookahead.value ) );
    bool bDeadlyPanic = PanicMode ( CTokenizer::UNKNOWN, "", pNextToken );
    if ( bDeadlyPanic == true )
    {
        throw Exception ( 0, 0, "Unable to recover from error. Compilation aborted." );
    }
}


CTokenizer::SToken CTranslator::Match ( CTokenizer::ETokenType eType,
                                        const CString& requiredValue,
                                        const CTokenizer::SToken* pNextToken,
                                        const char* szFile, unsigned int uiLine )
{
    char debuggingPrefix [ 64 ];
    CTokenizer::SToken ret;
    bool bDeadlyPanic = false;

    // Configuramos el modo de pánico.
    SetInPanic ( false );
    switch ( eType )
    {
        case CTokenizer::RESERVED:
            SetPanicStrategy ( PANIC_STRATEGY_IGNORE );
            break;
        case CTokenizer::SEPARATOR:
            SetPanicStrategy ( PANIC_STRATEGY_TRYAGAIN );
            break;
        default:
            SetPanicStrategy ( PANIC_STRATEGY_FINDNEXT );
            break;
    }

    // Generamos un string para información de debug, el cual contiene el
    // nombre del fichero y la linea de este en la que se ha ejecutado este match.
    snprintf ( debuggingPrefix, NUMELEMS(debuggingPrefix), "[%s:%d] ", szFile, uiLine );

    // Si habíamos alcanzado el fin de fichero, no deberíamos recibir más match.
    if ( EOFReached () == true )
        throw Exception ( 0, 0, Format( "%sUnexpected end of file.", debuggingPrefix ) );

    try
    {
        // Comprobamos que el valor del token sea el requerido.
        if ( requiredValue != "" && strcasecmp ( requiredValue, m_lookahead.value ) != 0 )
        {
            throw Exception ( m_lookahead.uiLine + 1, m_lookahead.uiCol + 1,
                              Format("%sExpected '%s', but got '%s'",
                                     debuggingPrefix,
                                     *requiredValue,
                                     m_lookahead.value
                                    )
                            );
        }

        // Comprobamos que el tipo de token sea el requerido.
        if ( m_lookahead.eType != eType )
        {
            throw Exception ( m_lookahead.uiLine + 1, m_lookahead.uiCol + 1,
                              Format("%sExpected token of type '%s', but got a token of type '%s': %s",
                                     debuggingPrefix,
                                     m_tokenizer.NameThisToken(eType),
                                     m_tokenizer.NameThisToken(m_lookahead.eType),
                                     m_lookahead.value
                                    )
                            );
        }
    }
    catch ( Exception& e )
    {
        // Si llegamos aquí, significa que el match ha fallado. Ejecutamos el
        // modo de pánico.
        bool bShowLineAndCol = e.GetLine() != 0 || e.GetColumn() != 0;
        Error ( e.GetReason(), e.GetLine(), e.GetColumn(), bShowLineAndCol );
        bDeadlyPanic = PanicMode ( eType, requiredValue, pNextToken );
    }

    // Comprobamos si se ha entrado en modo de pánico y el error no ha sido
    // recuperable.
    if ( bDeadlyPanic == true )
    {
        if ( m_ePanicStrategy == PANIC_STRATEGY_FAIL )
            throw Exception ( 0, 0, "Compilation aborted." );
        else
            throw Exception ( 0, 0, "Unable to recover from error. Compilation aborted." );
    }

    // Obtenemos el siguiente token.
    ret = m_lookahead;
    NextLookahead ();

    // Retornamos el token emparejado.
    return ret;
}

bool CTranslator::Check ( CTokenizer::ETokenType eType,
                          const CString& requiredValue )
{
    if ( EOFReached () == true )
        return ( eType == CTokenizer::END_OF_FILE );
    if ( m_lookahead.eType != eType )
        return false;
    if ( requiredValue != "" && requiredValue != m_lookahead.value )
        return false;
    return true;
}

void CTranslator::PushInstruction ( const CString& strCode )
{
    m_vecInstructions.push_back ( strCode );
}

unsigned int CTranslator::GetRef () const
{
    return m_vecInstructions.size ();
}

CString CTranslator::NewIdent ()
{
    return m_uiLastIdent++;
}

void CTranslator::Complete ( const std::vector<unsigned int>& refs, unsigned int ref )
{
    char strRef [ 32 ];
    snprintf ( strRef, NUMELEMS(strRef), "%u", ref );
    
    for ( std::vector<unsigned int>::const_iterator it = refs.begin ();
          it != refs.end();
          ++it )
    {
        const unsigned int& curRef = *it;
        CString& instruction = m_vecInstructions [ curRef ];
        instruction.Append ( strRef );
    }
}

unsigned int CTranslator::ST_Push ()
{
    return m_symbolTable.Push ();
}

unsigned int CTranslator::ST_Pop ()
{
    return m_symbolTable.Pop ();
}
