#ifndef CSTRING_H
#define	CSTRING_H

#include <cstdio>
#include <cstdarg>
#include <string>
#include "util.h"

class CString
{
private:
    std::string     m_str;
public:
    CString () {};
    CString ( const char* str ) : m_str ( str ) {}
    CString ( const std::string& str ) : m_str ( str ) {}
    CString ( const CString& Right ) : m_str ( Right.m_str ) {}

    CString& operator= ( const CString& Right )
    {
        m_str = Right.m_str;
        return *this;
    }
    CString& operator= ( const char* str )
    {
        m_str.assign ( str );
        return *this;
    }
    CString& operator= ( const std::string& str )
    {
        m_str = str;
        return *this;
    }

    const char* operator* () const
    {
        return m_str.c_str ();
    }
    
    operator const char*() const
    {
        return m_str.c_str ();
    }

    CString operator|| ( const CString& Right ) const
    {
        CString ret ( *this );
        ret.m_str.append ( Right.m_str );
        return ret;
    }

    CString operator|| ( const char* Right ) const
    {
        CString ret ( *this );
        ret.m_str.append ( Right );
        return ret;
    }
};

class Format : public CString
{
public:
    explicit Format ( const char* format, ... )
    {
        va_list vl;
        char temp [ 512 ];
        va_start ( vl, format );
        vsnprintf ( temp, NUMELEMS(temp), format, vl );
        va_end ( vl );

        *static_cast<CString *> ( this ) = temp;
    }
};

#endif	/* CSTRING_H */

