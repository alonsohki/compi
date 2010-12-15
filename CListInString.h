#ifndef CLISTINSTRING_H
#define	CLISTINSTRING_H

#include "CString.h"
#include <vector>

class CListInString
{
public:
    static CString EmptyList ()
    {
        return "()";
    }

    static bool IsEmpty ( const CString& ls )
    {
        return ( ls == "()" );
    }

    static CString InitList ( const CString& strFirstElement )
    {
        return CString("( ") || strFirstElement || " )";
    }

    static CString Join ( const CString& left, const CString& right )
    {
        if ( IsEmpty ( left ) )
            return right;
        else if ( IsEmpty ( right ) )
            return left;
        else
        {
            std::vector<CString> rightElements;
            GetListElements ( right, rightElements );

            CString ret ( left );
            ret.Resize ( ret.Length() - 2 );
            for ( std::vector<CString>::iterator it = rightElements.begin ();
                  it != rightElements.end();
                  ++it )
            {
                ret.Append ( ", " );
                ret.Append ( *it );
            }
            ret.Append ( " )" );

            return ret;
        }
    }

    static std::vector<CString>& GetListElements ( const CString& list, std::vector<CString>& to )
    {
        if ( list.Length() > 2 && list[0] == '(' && list[list.Length()-1] == ')' )
        {
            CString copy ( (*list) + 1 );
            copy.Resize ( copy.Length() - 1 );
            copy.CollapseWhiteSpaces ();
            copy.Split ( ',', to );
        }
        return to;
    }

    static unsigned int CountListElements ( const CString& list )
    {
        std::vector<CString> elements;
        GetListElements ( list, elements );
        return elements.size ();
    }

    static CString GetItem ( const CString& list, int idx )
    {
        std::vector<CString> elements;
        GetListElements ( list, elements );
        return elements [ idx ];
    }
};


#endif	/* CLISTINSTRING_H */

