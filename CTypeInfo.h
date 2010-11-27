#ifndef CTYPEINFO_H
#define	CTYPEINFO_H

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
        FUNCTION
    };

public:
    // Constructor copia
    CTypeInfo ( const CTypeInfo& Right )
    {
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

