#ifndef CSYMBOLTABLE_H
#define	CSYMBOLTABLE_H

#include <map>
#include "CString.h"
#include "CTypeInfo.h"

class CSymbolTable
{
    enum
    {
        MAX_DEPTH = 512
    };

public:
                        CSymbolTable    ();
                        ~CSymbolTable   ();

    unsigned int        Push            ();
    unsigned int        Pop             ();

    bool                MakeSymbol      ( const CString& strName,
                                          const CTypeInfo& type,
                                          bool bAllowRedeclare = false );
    bool                GetSymbol       ( const CString& strName,
                                          CTypeInfo* pInfo = 0 ) const;

    bool                MatchSymbolType ( const CString& strName,
                                          const CTypeInfo& type) const;

private:
    typedef std::map < CString, CTypeInfo > mapType;

    mapType         m_symbols [ MAX_DEPTH ];
    unsigned int    m_uiCurrentDepth;
};

#endif	/* CSYMBOLTABLE_H */

