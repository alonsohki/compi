#ifndef CTOKENIZER_H
#define	CTOKENIZER_H

#include "CClassifier.h"
#include <istream>

class CTokenizer
{
public:
    enum ETokenType
    {
        INTEGER = 0,
        REAL,
        IDENTIFIER,
        OPERATOR,
        COMMENT,
        SPACE,
        NEWLINE,
        SEPARATOR,
        UNKNOWN,
        MAX_TOKEN_TYPE
    };

    enum
    {
        NUMSTATES = 19,
        BUFFER_SIZE = 1024
    };

    struct SToken
    {
        ETokenType      eType;
        char            value [ 1024 ];
        unsigned int    uiValueLength;
        unsigned int    uiLine;
        unsigned int    uiCol;
        unsigned int    uiState;
    };

private:
    typedef void (CTokenizer::*fn_stateTrigger) ( unsigned char );
    static const int                ms_iTransitions [ CTokenizer::NUMSTATES ] [ CClassifier::GROUP_MAX ];
    static const ETokenType         ms_eFinalStates [ NUMSTATES ];
    static fn_stateTrigger          ms_fnTriggers [ NUMSTATES ];
    static const char*              ms_szErrors [ NUMSTATES ];

public:
                    CTokenizer      ( std::istream& isInput );
    virtual         ~CTokenizer     ();

    bool            NextToken       ( SToken* pToken = 0, bool bIgnoreWhiteSpaces = true );
private:
    bool            ReadToken       ( SToken* pToken = 0 );
public:
    const char*     NameThisToken   ( ETokenType eType ) const;
    const char*     GetErrorForToken( const SToken& token ) const { return ms_szErrors [ token.uiState ] != 0 ? ms_szErrors [ token.uiState ] : ""; }

private:
    void            CheckMultilineComment   ( unsigned char c );

private:
    struct BufferedReader
    {
        unsigned char   data [ BUFFER_SIZE ];
        unsigned char*  pos;
        unsigned char*  end;

        BufferedReader ( std::istream& isInput ) : m_isInput ( isInput ) { Initialize (); }

        bool ReadFromStream ()
        {
            // Inicializamos el buffer.
            m_isInput.read ( reinterpret_cast < char* > ( &data[0] ), BUFFER_SIZE );
            unsigned int uiCount = m_isInput.gcount ();
            if ( uiCount != 0 )
            {
                pos = data;
                end = data + uiCount;
            }
            else
            {
                // Marcamos que no se ha podido leer nada.
                end = data;
                pos = end + 1;
            }

            return ( uiCount != 0 );
        }

        bool Initialize () { return ReadFromStream (); }
        
        bool Get ( unsigned char* c = 0 )
        {
            // Rellenamos el buffer si es necesario.
            if ( pos >= end )
                if ( ReadFromStream () == false )
                    return false;

            if ( c != 0 )
                *c = pos [ 0 ];
            ++pos;
            return true;
        }

        void Rollback ()
        {
            --pos;
        }

        private:
            std::istream& m_isInput;
    } m_buffer;

    unsigned int    m_uiState;
    unsigned int    m_uiLine;
    unsigned int    m_uiCol;
    std::istream&   m_isInput;
};

#endif	/* CTOKENIZER_H */

