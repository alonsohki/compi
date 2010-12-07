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
    if ( EOFReached () == false )
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
        m_osDest << i << ": " << *(*it) << ";" << std::endl;
    }

    return true;
}

void CTranslator::NextLookahead ()
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

CTokenizer::SToken CTranslator::Match ( CTokenizer::ETokenType eType,
                                        const CString& requiredValue,
                                        const CTokenizer::SToken* pNextToken,
                                        const char* szFile, unsigned int uiLine )
{
    char debuggingPrefix [ 64 ];
    CTokenizer::SToken ret;

    snprintf ( debuggingPrefix, NUMELEMS(debuggingPrefix), "[%s:%d] ", szFile, uiLine );

    if ( EOFReached () == true )
        throw Exception ( 0, 0, Format( "%sUnexpected end of file", debuggingPrefix ) );

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

    if ( requiredValue != "" && strcasecmp ( requiredValue, m_lookahead.value ) != 0 )
    {
        throw Exception ( m_lookahead.uiLine + 1, m_lookahead.uiCol + 1,
                          Format("%sExpected token '%s', but got '%s'",
                                 debuggingPrefix,
                                 *requiredValue,
                                 m_lookahead.value
                                )
                        );
    }

    ret = m_lookahead;
    NextLookahead ();
    
    return ret;
}

bool CTranslator::Check ( CTokenizer::ETokenType eType,
                          const CString& requiredValue )
{
    if ( EOFReached () == true )
        return false;
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
