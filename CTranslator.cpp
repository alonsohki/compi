#include "CTranslator.h"
#include "CTokenizer.h"

CTranslator::CTranslator(std::istream& ifsOrig,
                         std::ostream& ofsDest)
: m_isOrig ( ifsOrig )
, m_osDest ( ofsDest )
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
    CTokenizer tokenizer ( m_isOrig );
    CTokenizer::SToken token;

    while ( true == tokenizer.NextToken( &token, true ) )
    {
        if ( token.eType != CTokenizer::UNKNOWN )
        {
            printf("(Tipo: %s, %u:%u) %s\n", tokenizer.NameThisToken(token.eType),
                                             token.uiLine + 1,
                                             token.uiCol + 1,
                                             token.value );
        }
        else
        {
            printf("(Tipo: %s, %u:%u) %s [Error: %s]\n", tokenizer.NameThisToken(token.eType),
                                             token.uiLine + 1,
                                             token.uiCol + 1,
                                             token.value,
                                             tokenizer.GetErrorForToken(token)
                                            );
        }
    }

    return true;
}