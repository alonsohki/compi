#ifndef CTRANSLATOR_H
#define	CTRANSLATOR_H

#include <iostream>
#include <fstream>
#include <vector>
#include "CTokenizer.h"
#include "CString.h"

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
                                          const CString& requiredValue = "" );
    void            PushInstruction     ( const CString& strCode );
    unsigned int    GetRef              () const;
    void            Complete            ( const std::vector<unsigned int>& refs, unsigned int ref );

public:
    bool        IsOk            () const;
    bool        Translate       ();

private:
    void        NextLookahead   ();

private:
    std::istream&           m_isOrig;
    std::ostream&           m_osDest;
    CTokenizer              m_tokenizer;
    CTokenizer::SToken      m_lookahead;
    std::vector<CString>    m_vecInstructions;
    bool                    m_bEOFReached;
};

#endif	/* CTRANSLATOR_H */
