#include "CTranslator.h"
#include "CTokenizer.h"
#include "CRules.h"

CTranslator::CTranslator(std::istream& ifsOrig,
                         std::ostream& ofsDest)
: m_isOrig ( ifsOrig )
, m_osDest ( ofsDest )
, m_tokenizer ( ifsOrig )
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
#if 0
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
#endif

    // Regla inicial.
    RULE(programa)();

    return true;
}