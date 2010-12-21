/////////////////
//
// Práctica de Compilación I (Curso 2010-2011)
//
//  FICHERO:        CBufferedReader.h
//  OBJETIVO:       Lectura del stream de entrada con buffering.
//  LICENCIA:       Mira el fichero LICENSE en el directorio raíz.
//  AUTORES:        El equipo del JAG.
//

#ifndef CBUFFEREDREADER_H
#define	CBUFFEREDREADER_H

#include <istream>

template < unsigned int BUFFER_SIZE = 1024 >
class CBufferedReader
{
    unsigned char   data [ BUFFER_SIZE ];
    unsigned char*  pos;
    unsigned char*  end;
    std::istream&   isInput;
    bool            bLastCharWasCR;
    unsigned int    uiLine;
    unsigned int    uiCol;

public:
    CBufferedReader ( std::istream& p_isInput )
    : isInput ( p_isInput )
    , bLastCharWasCR ( false )
    , uiLine ( 0 )
    , uiCol ( 0 )
    {
        Initialize ();
    }

private:
    bool ReadFromStream ()
    {
        // Inicializamos el buffer.
        isInput.read ( reinterpret_cast < char* > ( &data[0] ), BUFFER_SIZE );
        unsigned int uiCount = isInput.gcount ();
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

public:
    bool Get ( unsigned char* c = 0 )
    {
        // Rellenamos el buffer si es necesario.
        if ( pos >= end )
            if ( ReadFromStream () == false )
                return false;

        // Comprobamos saltos de linea.
        switch ( pos [ 0 ] )
        {
            case '\r':
                bLastCharWasCR = true;
                ++uiLine;
                uiCol = 0;
                break;
            case '\n':
                if ( bLastCharWasCR == true )
                {
                    bLastCharWasCR = false;
                }
                else
                {
                    ++uiLine;
                }
                uiCol = 0;
                break;
            default:
                ++uiCol;
                bLastCharWasCR = false;
        }

        if ( c != 0 )
            *c = pos [ 0 ];
        ++pos;
        return true;
    }

    void Rollback ()
    {
        --pos;
        if ( uiCol > 0 )
            --uiCol;
        else
            --uiLine;
    }

    unsigned int GetLine () const { return uiLine; }
    unsigned int GetCol () const { return uiCol; }
};

#endif	/* CBUFFEREDREADER_H */

