#ifndef CSYMBOLTABLE_H
#define	CSYMBOLTABLE_H

#include <map>
#include "CString.h"
#include "CTypeInfo.h"

class CSymbolTable
{
public:
    // Estrategias a seguir cuando se intenta agregar un símbolo ya existente.
    enum EStrategy
    {
        STRATEGY_FAIL,
        STRATEGY_REPLACE,
        STRATEGY_IGNORE
    };

    enum
    {
        MAX_DEPTH = 512,
        STRATEGY = STRATEGY_REPLACE
    };

public:
                        CSymbolTable    ();
                        ~CSymbolTable   ();

    unsigned int        Push            ();
    unsigned int        Pop             ();

    bool                MakeSymbol      ( const CString& strName,
                                          const CTypeInfo& type,
                                          unsigned int uiRef,
                                          unsigned int* puiOldRef = 0 );
    bool                GetSymbol       ( const CString& strName,
                                          CTypeInfo* pInfo = 0,
                                          unsigned int* puiRef = 0 ) const;

    bool                MatchSymbolType ( const CString& strName,
                                          const CTypeInfo& type ) const;

private:
    struct SSymbolData
    {
        SSymbolData ( const CTypeInfo& t, unsigned int r )
        : type ( t ), uiRef ( r ) {}
        CTypeInfo type;
        unsigned int uiRef;
    };
    typedef std::map < CString, SSymbolData > mapType;

    mapType         m_symbols [ MAX_DEPTH ];
    unsigned int    m_uiCurrentDepth;
};

#endif	/* CSYMBOLTABLE_H */

