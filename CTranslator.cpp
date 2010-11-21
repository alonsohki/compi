#include <cstring>
#include "CTranslator.h"
#include "CTokenizer.h"
#include "CRules.h"

CTranslator::CTranslator(std::istream& ifsOrig,
                         std::ostream& ofsDest)
: m_isOrig ( ifsOrig )
, m_osDest ( ofsDest )
, m_tokenizer ( ifsOrig )
, m_bEOFReached ( false )
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
        fprintf ( stderr, "Error [%u:%u]: %s.\n",
                  e.GetLine(), e.GetColumn(), *(e.GetReason()) );
        return false;
    }

    // No debería quedar nada que leer (aparte de los blancos).
    if ( m_bEOFReached == false )
    {
        fprintf ( stderr, "Error [%u:%u]: Unexpected token: %s.\n",
                  m_lookahead.uiLine + 1, m_lookahead.uiCol + 1,
                  m_lookahead.value );
        return false;
    }

    // Escribimos el resultado en la salida.
    unsigned int i = 0;
    for ( std::vector<CString>::const_iterator it = m_vecInstructions.begin ();
          it != m_vecInstructions.end();
          ++it )
    {
        m_osDest << i << ": " << *(*it) << std::endl;
    }

    return true;
}

void CTranslator::NextLookahead ()
{
    m_bEOFReached = ( m_tokenizer.NextToken ( &m_lookahead, true ) == false );
    if ( m_bEOFReached == false && m_lookahead.eType == CTokenizer::UNKNOWN )
    {
        const char* szError = m_tokenizer.GetErrorForToken ( m_lookahead );
        if ( szError == 0 )
            throw Exception ( m_lookahead.uiLine + 1, m_lookahead.uiCol + 1, Format("Unexpected token: %s", m_lookahead.value) );
        else
            throw Exception ( m_lookahead.uiLine + 1, m_lookahead.uiCol + 1, szError );
    }
        
}

CTokenizer::SToken CTranslator::Match ( CTokenizer::ETokenType eType,
                                        const CString& requiredValue )
{
    CTokenizer::SToken ret;

    if ( m_bEOFReached == true )
        throw Exception ( 0, 0, "Unexpected end of file" );

    if ( m_lookahead.eType != eType )
    {
        throw Exception ( m_lookahead.uiLine + 1, m_lookahead.uiCol + 1,
                          Format("Expected token of type '%s', but got a token of type '%s'",
                                 m_tokenizer.NameThisToken(eType),
                                 m_tokenizer.NameThisToken(m_lookahead.eType)
                                )
                        );
    }

    if ( requiredValue != "" && strcasecmp ( requiredValue, m_lookahead.value ) != 0 )
    {
        throw Exception ( m_lookahead.uiLine + 1, m_lookahead.uiCol + 1,
                          Format("Expected token '%s', but got '%s'",
                                 *requiredValue,
                                 m_lookahead.value
                                )
                        );
    }

    ret = m_lookahead;
    NextLookahead ();
    
    return ret;
}

void CTranslator::PushInstruction ( const CString& strCode )
{
    m_vecInstructions.push_back ( strCode );
}

unsigned int CTranslator::GetRef () const
{
    return m_vecInstructions.size ();
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
