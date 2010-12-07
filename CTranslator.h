#ifndef CTRANSLATOR_H
#define	CTRANSLATOR_H

#include <iostream>
#include <fstream>
#include <vector>
#include "CTokenizer.h"
#include "CString.h"
#include "CSymbolTable.h"

class CTranslator
{
public:
    // Clases de apoyo.
    class Exception
    {
    public:
        Exception ( unsigned int uiLine, unsigned int uiColumn, const CString& strReason )
        : m_uiLine ( uiLine )
        , m_uiColumn ( uiColumn )
        , m_strReason ( strReason )
        {
        }

        unsigned int    GetLine     () const { return m_uiLine; }
        unsigned int    GetColumn   () const { return m_uiColumn; }
        const CString&  GetReason   () const { return m_strReason; }

    private:
        unsigned int    m_uiLine;
        unsigned int    m_uiColumn;
        CString         m_strReason;
    };
    
public:
                CTranslator     (std::istream& ifsOrig,
                                 std::ostream& ofsDest);
    virtual     ~CTranslator    ();

    // Funciones para uso de las reglas.
private:
    friend class __CRule__Base__;

    CTokenizer::SToken  Match           ( CTokenizer::ETokenType eType,
                                          const CString& requiredValue,
                                          const CTokenizer::SToken* pNextToken,
                                          const char* szFile, unsigned int uiLine );
    bool                Check           ( CTokenizer::ETokenType eType,
                                          const CString& requiredValue = "" );
    void            PushInstruction     ( const CString& strCode );
    unsigned int    GetRef              () const;
    CString         NewIdent            ();
    void            Complete            ( const std::vector<unsigned int>& refs, unsigned int ref );
    // Tabla de símbolos.
    unsigned int    ST_Push             ();
    unsigned int    ST_Pop              ();

public:
    bool        IsOk            () const;
    bool        Translate       ();

private:
    void        NextLookahead   ();
    bool        EOFReached      () const { return m_lookahead.eType == CTokenizer::END_OF_FILE; }

    // Modo pánico.
    enum EPanicStrategy
    {
        PANIC_STRATEGY_FAIL,        // La compilación se aborta.
        PANIC_STRATEGY_IGNORE,      // Se hace como si el match hubiera sido certero.
        PANIC_STRATEGY_TRYAGAIN,    // Igual que el anterior, pero manteniendo el lookahead.
        PANIC_STRATEGY_FINDIT,      // Busca tokens hasta encontrar el que se había solicitado.
        PANIC_STRATEGY_FINDNEXT     // Busca tokens hasta encontrar un siguiente de la regla.
    };
    bool        PanicMode       ( CTokenizer::ETokenType eType,
                                  const CString& requiredValue,
                                  const CTokenizer::SToken* pNextToken );
    void        SetInPanic      ( bool bInPanic ) { m_bInPanic = bInPanic; }
    bool        IsInPanic       ( ) const { return m_bInPanic; }
    void        SetPanicStrategy( EPanicStrategy eStrategy ) { m_ePanicStrategy = eStrategy; }

private:
    std::istream&           m_isOrig;
    std::ostream&           m_osDest;
    CTokenizer              m_tokenizer;
    CTokenizer::SToken      m_lookahead;
    std::vector<CString>    m_vecInstructions;
    CSymbolTable            m_symbolTable;
    unsigned int            m_uiLastIdent;
    bool                    m_bInPanic;
    EPanicStrategy          m_ePanicStrategy;
    bool                    m_bKeepCurrentLookahead;
};

#endif	/* CTRANSLATOR_H */
