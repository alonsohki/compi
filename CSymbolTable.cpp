#include "CSymbolTable.h"

CSymbolTable::CSymbolTable ()
: m_uiCurrentDepth ( 0 )
{
}
CSymbolTable::~CSymbolTable ()
{
}

unsigned int CSymbolTable::Push ()
{
    if ( m_uiCurrentDepth < MAX_DEPTH-1 )
    {
        ++m_uiCurrentDepth;
    }
    return m_uiCurrentDepth;
}

unsigned int CSymbolTable::Pop ()
{
    if ( m_uiCurrentDepth > 0 )
    {
        m_symbols [ m_uiCurrentDepth ].clear ();
        --m_uiCurrentDepth;
    }
    return m_uiCurrentDepth;
}

bool CSymbolTable::MakeSymbol ( const CString& strName,
                                const CTypeInfo& type,
                                unsigned int uiRef,
                                unsigned int* puiOldRef
                              )
{
    bool bOk = true;
    mapType& curMap = m_symbols [ m_uiCurrentDepth ];
    mapType::iterator it = curMap.find ( strName );
    if ( it != curMap.end () )
    {
        if ( puiOldRef != 0 )
            *puiOldRef = it->second.uiRef;
        if ( (EStrategy)STRATEGY == STRATEGY_REPLACE )
        {
            it->second.type = type;
            it->second.uiRef = uiRef;
        }
        bOk = false;
    }
    else
    {
        curMap.insert( mapType::value_type(strName, SSymbolData(type, uiRef)) );
    }

    return bOk;
}

bool CSymbolTable::GetSymbol ( const CString& strName, CTypeInfo* pInfo, unsigned int* puiRef ) const
{
    bool bFound = false;
    mapType::const_iterator it;

    // Búsqueda en profundidad.
    for ( unsigned int i = 0; i <= m_uiCurrentDepth; ++i )
    {
        unsigned int uiDepth = m_uiCurrentDepth - i;
        it = m_symbols [ uiDepth ].find ( strName );
        if ( it != m_symbols [ uiDepth ].end () )
        {
            bFound = true;
            if ( pInfo != 0 )
                *pInfo = it->second.type;
            if ( puiRef != 0 )
                *puiRef = it->second.uiRef;
            break;
        }
    }

    return bFound;
}

bool CSymbolTable::MatchSymbolType ( const CString& strName,
                                     const CTypeInfo& type ) const
{
    CTypeInfo info;
    bool bFound = GetSymbol ( strName, &info, 0 );
    if ( bFound )
        return info == type;
    return false;
}