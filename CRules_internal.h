#ifndef CRULES_INTERNAL_H
#define	CRULES_INTERNAL_H

#include <map>
#include <vector>
#include "CString.h"

// Estructura de datos base de las reglas.
class __CRule__Base__
{
    typedef std::map<CString, CString> mapType;
public:
    __CRule__Base__ ( std::vector<CString>& vecInstructions )
    : m_vecInstructions ( vecInstructions )
    {}
    
    CString& operator[] ( const char* key )
    {
        return operator[](CString(key));
    }

    CString& operator[] ( const CString& key )
    {
        mapType::iterator it = m_mapAttributes.find(key);
        if ( it == m_mapAttributes.end () )
        {
            // No se encontró un atributo con esta clave. Lo generamos.
            std::pair<mapType::iterator, bool> inserted =
                m_mapAttributes.insert(std::make_pair(key, CString()));
            return inserted.first->second;
        }
        return it->second;
    }
private:
    mapType                 m_mapAttributes;
    std::vector<CString>&   m_vecInstructions;
};

// Macros de uso interno.
#define BUILD_RULE_CLASS_NAME(x) __CRuleDef__ ## x ## __
#define DECLARE_RULE(x) class BUILD_RULE_CLASS_NAME(x) : public __CRule__Base__ \
{ \
public: BUILD_RULE_CLASS_NAME(x) (std::vector<CString>& v) : __CRule__Base__(v) {} \
        void operator() (); \
}
#define DEFINE_RULE(x) void BUILD_RULE_CLASS_NAME(x) :: operator() ()

#endif	/* CRULES_INTERNAL_H */

