#ifndef CBUFFEREDREADER_H
#define	CBUFFEREDREADER_H

#include <istream>

template < unsigned int BUFFER_SIZE = 1024 >
class CBufferedReader
{
    unsigned char   data [ BUFFER_SIZE ];
    unsigned char*  pos;
    unsigned char*  end;

public:
    CBufferedReader ( std::istream& isInput ) : m_isInput ( isInput ) { Initialize (); }

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
};

#endif	/* CBUFFEREDREADER_H */

