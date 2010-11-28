#ifndef CTYPEINFO_H
#define	CTYPEINFO_H

#include <vector>
#include <string>
#include <cassert>
#include <cstdlib>
#include "CString.h"

class CTypeInfo
{
    // Configuración.
    enum
    {
        MAX_ARRAY_DIMENSIONS = 64,
        MAX_PARAMS = 32,
        WORD_SIZE = 1
    };

public:
    // Clases de tipos.
    enum EType
    {
        INTEGER,
        REAL,
        BOOLEAN,
        ARRAY,
        PROCEDURE,
        FUNCTION,

        UNKNOWN
    };

public:
    CTypeInfo ()
    {
        m_eType = UNKNOWN;
        memset ( &m_array, 0, sizeof (m_array) );
        memset ( &m_params, 0, sizeof (m_params) );
    }

    // Constructor copia
    CTypeInfo ( const CTypeInfo& Right )
    {
        memset ( &m_array, 0, sizeof (m_array) );
        memset ( &m_params, 0, sizeof (m_params) );
        operator= ( Right );
    }

    // Tipos básicos.
    CTypeInfo ( EType eType )
    : m_eType ( eType )
    {
        memset ( &m_array, 0, sizeof (m_array) );
        memset ( &m_params, 0, sizeof (m_params) );
    }

    // Arrays.
    CTypeInfo ( const std::vector<unsigned int>& dimensionsLength, const CTypeInfo& contentInfo )
    : m_eType ( ARRAY )
    {
        memset ( &m_params, 0, sizeof (m_params) );

        m_array.numDimensions = 0;
        for ( std::vector<unsigned int>::const_iterator it = dimensionsLength.begin ();
              it != dimensionsLength.end ();
              ++it )
        {
            m_array.dimensionsLength [ m_array.numDimensions ] = *it;
            ++m_array.numDimensions;
        }
        m_array.contentType = new CTypeInfo ( contentInfo );
    }

    // Procedimientos/Funciones
    CTypeInfo ( const std::vector<CTypeInfo>& paramTypes, const CTypeInfo* returns = 0 )
    {
        memset ( &m_array, 0, sizeof (m_array) );

        if ( returns != 0 )
        {
            m_eType = FUNCTION;
            m_params.returns = new CTypeInfo ( *returns );
        }
        else
        {
            m_eType = PROCEDURE;
            m_params.returns = 0;
        }

        m_params.numParams = 0;
        for ( std::vector<CTypeInfo>::const_iterator it = paramTypes.begin ();
              it != paramTypes.end ();
              ++it )
        {
            m_params.paramTypes [ m_params.numParams ] = new CTypeInfo ( *it );
            ++m_params.numParams;
        }
    }

    // Destructor
    ~CTypeInfo ()
    {
        Cleanup ();
    }

    // String <-> CTypeInfo
    // El formato es el siguiente (los espacios se han añadido por claridad):
    // nombreTipo $ datos11:datos12 $ datos21:datos22 $ ... $ datosN #
    //
    // Los $ separan las distintas secciones de datos, los : separan los
    // distintos datos de cada sección y la # marca el final de la definición
    // del tipo.
    //
    // Los tipos básicos (entero, real, booleano) no tienen datos:
    //  - integer#
    //  - real#
    //  - boolean#
    //
    // Los datos para el resto de tipos son:
    //  - array $ dimension1:dimension2:...:dimensionN $ tipoAlmacenado #
    //  - procedure $ tipoParam1:tipoParam2:...:tipoParamN #
    //  - function $ tipoRetorno $ tipoParam1:tipoParam2:...:tipoParamN #
    //
public:
    CTypeInfo ( const CString& str )
    {
        memset ( &m_array, 0, sizeof (m_array) );
        memset ( &m_params, 0, sizeof (m_params) );
        LoadFromString ( str.GetString() );
    }

    CTypeInfo& operator= ( const CString& str )
    {
        LoadFromString ( str.GetString() );
        return *this;
    }

private:
    CTypeInfo ( const std::string& str, size_t* sizeFromString )
    {
        memset ( &m_array, 0, sizeof (m_array) );
        memset ( &m_params, 0, sizeof (m_params) );
        if ( sizeFromString != 0 )
            *sizeFromString = LoadFromString ( str );
        else
            LoadFromString ( str );
    }

    size_t LoadFromString ( const std::string& str )
    {
        Cleanup ();

        if ( str.compare( 0, 8, "unknown#" ) == 0 )
        {
            m_eType = UNKNOWN;
        }
        else if ( str.compare( 0, 8, "integer#" ) == 0 )
        {
            m_eType = INTEGER;
            return 8;
        }
        else if ( str.compare ( 0, 5, "real#" ) == 0 )
        {
            m_eType = REAL;
            return 5;
        }
        else if ( str.compare ( 0, 8, "boolean#" ) == 0 )
        {
            m_eType = BOOLEAN;
            return 8;
        }
        else if ( str.compare ( 0, 6, "array$" ) == 0 )
        {
            size_t pos = str.find ( '$', 6 );
            assert ( pos != std::string::npos );
            std::vector<CString> dims;
            CString ( str.substr(6, pos-6) ).Split ( ':', dims );

            m_array.numDimensions = 0;
            for ( std::vector<CString>::const_iterator it = dims.begin ();
                  it != dims.end ();
                  ++it )
            {
                m_array.dimensionsLength [ m_array.numDimensions ] = atoi ( *it );
                ++m_array.numDimensions;
            }

            size_t subLen;
            m_array.contentType = new CTypeInfo ( str.substr ( pos + 1 ), &subLen );
            m_eType = ARRAY;

            assert ( str [ pos + 1 + subLen ] == '#' );
            return subLen + pos + 2; // Nos saltamos también el #
        }
        else
        {
            size_t start;
            if ( str.compare ( 0, 9, "function$" ) == 0 )
            {
                size_t retLen;
                m_params.returns = new CTypeInfo ( str.substr ( 9 ), &retLen );

                m_eType = FUNCTION;
                start = 9 + retLen + 1;// +1 por el segundo $ de function$retorno$
            }
            else
            {
                m_eType = PROCEDURE;
                start = 10; // strlen("procedure$");
            }
            assert ( str [ start - 1 ] == '$' );

            // Extraemos los parámetros.
            m_params.numParams = 0;
            if ( str [ start ] != '#' )
            {
                do
                {
                    size_t paramLen;
                    m_params.paramTypes [ m_params.numParams ]
                        = new CTypeInfo ( str.substr ( start ), &paramLen );
                    ++m_params.numParams;
                    start += paramLen + 1;
                } while ( str [ start - 1 ] == ':' );
                --start;
            }

            return start;
        }
    }

public:
    CString toString () const
    {
        CString ret;
        switch ( m_eType )
        {
            case UNKNOWN:
                ret = "unknown#";
                break;
            case INTEGER:
                ret = "integer#";
                break;
            case REAL:
                ret = "real#";
                break;
            case BOOLEAN:
                ret = "boolean#";
                break;
            case ARRAY:
                ret = "array$";
                for ( unsigned int i = 0; i < m_array.numDimensions; ++i )
                {
                    char szTemp [ 32 ];
                    snprintf ( szTemp, NUMELEMS(szTemp), "%u", m_array.dimensionsLength [ i ] );
                    ret.Append ( szTemp );
                    ret.Append ( ":" );
                }
                ret.Resize ( ret.Length() - 1 );
                ret.Append ( "$" );
                ret.Append ( m_array.contentType->toString () );
                ret.Append ( "#" );
                break;
            case FUNCTION:
            case PROCEDURE:
                if ( m_eType == FUNCTION )
                {
                    ret = "function$";
                    ret.Append ( m_params.returns->toString () );
                    ret.Append ( "$" );
                }
                else
                {
                    ret = "procedure$";
                }

                for ( unsigned int i = 0; i < m_params.numParams; ++i )
                {
                    ret.Append ( m_params.paramTypes[i]->toString () );
                    ret.Append ( ":" );
                }
                ret.Resize ( ret.Length () - 1 );
                ret.Append ( "#" );
                break;
        }

        return ret;
    }

private:
    void Cleanup ()
    {
        if ( m_array.contentType != 0 )
            delete m_array.contentType;
        if ( m_params.returns != 0 )
            delete m_params.returns;
        for ( unsigned int i = 0; i < MAX_PARAMS; ++i )
        {
            if ( m_params.paramTypes [ i ] != 0 )
                delete m_params.paramTypes [ i ];
        }
        memset ( &m_array, 0, sizeof (m_array) );
        memset ( &m_params, 0, sizeof (m_params) );
    }

public:
    CTypeInfo& operator= ( const CTypeInfo& Right )
    {
        Cleanup ();
        
        m_eType = Right.m_eType;
        m_array = Right.m_array;
        m_params = Right.m_params;

        if ( m_array.contentType != 0 )
            m_array.contentType = new CTypeInfo ( *(m_array.contentType) );
        if ( m_params.returns != 0 )
            m_params.returns = new CTypeInfo ( *(m_params.returns) );
        for ( unsigned int i = 0; i < MAX_PARAMS; ++i )
        {
            if ( m_params.paramTypes[i] != 0 )
                m_params.paramTypes[i] = new CTypeInfo ( *(m_params.paramTypes[i]) );
        }

        return *this;
    }

    bool operator== ( const CTypeInfo& Right ) const
    {
        bool bEquals = ( m_eType == Right.m_eType );

        if ( bEquals == true )
        {
            switch ( m_eType )
            {
                case ARRAY:
                    bEquals = ( m_array.numDimensions == Right.m_array.numDimensions ) &&
                              ( *(m_array.contentType) == *(Right.m_array.contentType) );
                    if ( bEquals )
                    {
                        for ( unsigned int i = 0; bEquals && i < m_array.numDimensions; ++i )
                            bEquals = m_array.dimensionsLength[i] == Right.m_array.dimensionsLength[i];
                    }
                    break;
                case FUNCTION:
                    bEquals = *(m_params.returns) == *(Right.m_params.returns);
                case PROCEDURE:
                    bEquals = bEquals && (m_params.numParams == Right.m_params.numParams);
                    if ( bEquals )
                    {
                        for ( unsigned int i = 0; bEquals && i < m_params.numParams; ++i )
                            bEquals = *(m_params.paramTypes[i]) == *(Right.m_params.paramTypes[i]);
                    }
                    break;
                default:
                    break;
            }
        }

        return bEquals;
    }

    bool operator!= ( const CTypeInfo& Right ) const
    {
        return ( operator==(Right) == false );
    }
    

    // "Getters"
    EType               GetType             () const { return m_eType; }

    const CTypeInfo*    GetArrayContent     () const { return m_array.contentType; }
    const unsigned int* GetArrayDimensions  () const { return m_array.dimensionsLength; }
    unsigned int        GetArrayDepth       () const { return m_array.numDimensions; }

    const CTypeInfo*    GetFunctionRetType  () const { return m_params.returns; }
    const CTypeInfo* const*
                        GetProcedureParams  () const { return m_params.paramTypes; }
    unsigned int        GetNumparams        () const { return m_params.numParams; }

    unsigned int        GetSize             () const
    {
        unsigned int uiSize;

        switch ( m_eType )
        {
            case INTEGER:
            case REAL:
            case BOOLEAN:
                uiSize = WORD_SIZE;
                break;
            case PROCEDURE:
            case FUNCTION:
                uiSize = 0;
                break;
            case ARRAY:
                uiSize = m_array.contentType->GetSize ();
                for ( unsigned int i = 0; i < m_array.numDimensions; ++i )
                {
                    uiSize *= m_array.dimensionsLength [ i ];
                }
                break;
            case UNKNOWN:
                uiSize = 0;
                break;
        }

        return uiSize;
    }
    
private:
    EType           m_eType;
    struct
    {
        CTypeInfo*      contentType;
        unsigned int    dimensionsLength [ MAX_ARRAY_DIMENSIONS ];
        unsigned int    numDimensions;
    } m_array;
    
    struct
    {
        CTypeInfo*      returns;
        CTypeInfo*      paramTypes [ MAX_PARAMS ];
        unsigned int    numParams;
    } m_params;
};

#endif	/* CTYPEINFO_H */

