#ifndef CRULES_INTERNAL_H
#define	CRULES_INTERNAL_H

#include <cstdlib>
#include <vector>
#include "CTranslator.h"
#include "CString.h"
#include "CListInString.h"

// Estructura de datos base de las reglas.
class __CRule__Base__
{
protected:
    __CRule__Base__ ( CTranslator* pTranslator )
    : m_pTranslator ( pTranslator )
    {}

    CTranslator*        GetTranslator   () { return m_pTranslator; }
    bool                panic_mode      () const
    {
        return m_pTranslator->IsInPanic ();
    }
    void                panic           ( const CTokenizer::SToken* pNextToken )
    {
        m_pTranslator->Panic ( pNextToken );
    }
    void                warning         ( const CString& msg )
    {
        m_pTranslator->Warning ( msg );
    }
    void                error           ( const CString& msg )
    {
        m_pTranslator->Error ( msg );
    }
    void                die             ( const CString& msg )
    {
        m_pTranslator->Die ( msg );
    }
    CTokenizer::SToken  match           ( CTokenizer::ETokenType eType,
                                          const CString& requiredValue,
                                          const CTokenizer::SToken* pNextToken,
                                          const char* szFile, unsigned int uiLine )
    {
        return m_pTranslator->Match ( eType, requiredValue, pNextToken, szFile, uiLine );
    }
    bool                is_first        ( CTokenizer::ETokenType eType,
                                          const CString& requiredValue = "" )
    {
        return m_pTranslator->Check ( eType, requiredValue );
    }
    bool                is_rule_first   ( const CTokenizer::SToken tokensFirst [],
                                          const CTokenizer::SToken tokensNext [] )
    {
        bool bFound = false;
        for ( unsigned int i = 0;
              bFound == false && tokensFirst[i].eType != CTokenizer::UNKNOWN;
              ++i )
        {
            if ( tokensFirst[i].eType == CTokenizer::EMPTY )
            {
                for ( unsigned int j = 0;
                      bFound == false && tokensNext[j].eType != CTokenizer::UNKNOWN;
                      ++j )
                {
                    bFound = is_first ( tokensNext[j].eType, tokensNext[j].value );
                }
            }
            else
                bFound = is_first ( tokensFirst[i].eType, tokensFirst[i].value );
        }
        return bFound;
    }
    void            push_instruction    ( const CString& strCode )
    {
        m_pTranslator->PushInstruction ( strCode );
    }
    CString         get_ref             ()
    {
        char szTemp [ 32 ];
        snprintf ( szTemp, NUMELEMS(szTemp), "%u", m_pTranslator->GetRef () );
        return CString ( szTemp );
    }
    CString         new_ident           ()
    {
        return m_pTranslator->NewIdent ();
    }
    CString         empty_list          ()
    {
        return CListInString::EmptyList ();
    }
    CString         init_list           ( const CString& strFirstElement )
    {
        return CListInString::InitList ( strFirstElement );
    }
    CString         join_lists          ( const CString& listLeft, const CString& listRight )
    {
        return CListInString::Join ( listLeft, listRight );
    }
    void            complete            ( const CString& list, const CString& ref )
    {
        std::vector<unsigned int> listNumbers;
        std::vector<CString> listStrings;
        CListInString::GetListElements(list, listStrings);

        for ( std::vector<CString>::iterator it = listStrings.begin ();
              it != listStrings.end();
              ++it )
        {
            listNumbers.push_back ( static_cast < unsigned int > ( atoi ( *it ) ) );
        }

        m_pTranslator->Complete ( listNumbers, static_cast < unsigned int > ( atoi(ref) ) );
    }
    void            symbol_table_push   ()
    {
        m_pTranslator->ST_Push();
    }
    void            symbol_table_pop    ()
    {
        m_pTranslator->ST_Pop();
    }
    bool            symbol_table_add    ( const CString& symbolName, const CString& typeInfo )
    {
        return m_pTranslator->ST_Add( symbolName, CTypeInfo ( typeInfo ) );
    }
    bool            symbol_table_get    ( const CString& strName, CTypeInfo* pInfo, unsigned int* puiRef )
    {
        return m_pTranslator->ST_Get( strName, pInfo, puiRef );
    }
    CTypeInfo       symbol_table_type   ( const CString& strName)
    {
    	CTypeInfo pInfo;
    	unsigned int puiRef;
        m_pTranslator->ST_Get( strName, &pInfo, &puiRef );
        return pInfo;
    }
    bool            symbol_table_match    ( const CString& symbolName, const CTypeInfo& info )
    {
    	return m_pTranslator->ST_Match( symbolName, info );
    }
    CString         type_of             ( const CString& typeInfo )
    {
        CTypeInfo info ( typeInfo );
        switch ( info.GetType() )
        {
            case CTypeInfo::INTEGER: return "int";
            case CTypeInfo::REAL: return "real";
            case CTypeInfo::BOOLEAN: return "bool";
            case CTypeInfo::ARRAY: return "array";
            case CTypeInfo::PROCEDURE: return "procedure";
            case CTypeInfo::FUNCTION: return "function";
            default: return "";
        }
    }
    CString     new_array_type          ( const CString& integerList,
                                          const CString& arrayContent )
    {
        std::vector < CString > vecIntegers;
        CListInString::GetListElements( integerList, vecIntegers );
        std::vector < unsigned int > vecDimensions;
        for ( std::vector<CString>::const_iterator iter = vecIntegers.begin ();
              iter != vecIntegers.end();
              ++iter )
        {
            vecDimensions.push_back ( atoi(*iter) );
        }
        return CTypeInfo ( vecDimensions, CTypeInfo ( arrayContent ) ).toString ();
    }
    CString     new_function_type       ( const CString& paramTypeList,
                                          const CString& paramClassesList,
                                          const CString& returns)
    {
        std::vector < CString > vecTypeList;
        std::vector < CString > vecClassesList;

        CTypeInfo typeReturns = CTypeInfo ( returns );

        CListInString::GetListElements( paramTypeList, vecTypeList );
        CListInString::GetListElements( paramClassesList, vecClassesList );

        std::vector < CTypeInfo > vecTypeList2;
        std::vector < CTypeInfo::EParamClass > vecClassesList2;

        std::vector<CString>::const_iterator cit = vecClassesList.begin ();
        for ( std::vector<CString>::const_iterator it = vecTypeList.begin ();
              it != vecTypeList.end ();
              ++it, ++cit )
        {
        	vecTypeList2.push_back( CTypeInfo ( *it ) );

            if ( *cit == "copyandrestore" )
            {
            	vecClassesList2.push_back( CTypeInfo::PARAM_COPY_AND_RESTORE );
            }
            else if ( *cit == "copy" )
            {
            	vecClassesList2.push_back( CTypeInfo::PARAM_COPY );
            }
            else if ( *cit == "ref" )
            {
            	vecClassesList2.push_back( CTypeInfo::PARAM_REF);
            }
            else assert ( false ); // No debería suceder nunca.

        }
        return CTypeInfo ( vecTypeList2, vecClassesList2, &typeReturns ).toString ();
    }

private:
    CTranslator*   m_pTranslator;
};

// Macros de uso interno.
// AVISO: A continuación vienen un montón de HACKS feos y apestosos, que podrían
// herir la sensibilidad del lector.
#define EXPAND(x) x
#define CAT(a,b) EXPAND(a ## b)
#define NUMARGS(...) NUMARGS_I(0, ## __VA_ARGS__, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define NUMARGS_I(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, N, ...) N
#define FOR_EACH_PARAM(fn, ...) FOR_EACH_PARAM_I(fn, NUMARGS(__VA_ARGS__), __VA_ARGS__)
#define FOR_EACH_PARAM_I(fn, n, ...) CAT(FOR_EACH_PARAM_I_, n)(fn, ## __VA_ARGS__)
#define FOR_EACH_PARAM_I_0(fn, elem, ...)
#define FOR_EACH_PARAM_I_1(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_0(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_2(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_1(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_3(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_2(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_4(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_3(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_5(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_4(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_6(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_5(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_7(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_6(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_8(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_7(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_9(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_8(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_10(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_9(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_11(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_10(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_12(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_11(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_13(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_12(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_14(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_13(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_15(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_14(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_16(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_15(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_17(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_16(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_18(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_17(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_19(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_18(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_20(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_19(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_21(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_20(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_22(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_21(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_23(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_22(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_24(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_23(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_25(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_24(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_26(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_25(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_27(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_26(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_28(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_27(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_29(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_28(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_30(fn, elem, ...) fn(elem) FOR_EACH_PARAM_I_29(fn, __VA_ARGS__)



#define BUILD_RULE_CLASS_NAME(x) __CRuleDef__ ## x ## __
#define MAP_ATTRIBUTE_TYPE(attr) CString (attr);
#define DECLARE_RULE(x, ...) class BUILD_RULE_CLASS_NAME(x) : public __CRule__Base__ \
{ \
public: \
    static const CTokenizer::SToken ms_firstToken []; \
    static const CTokenizer::SToken ms_nextToken []; \
\
public: \
    FOR_EACH_PARAM(MAP_ATTRIBUTE_TYPE, __VA_ARGS__); \
    BUILD_RULE_CLASS_NAME(x) ( CTranslator* pTranslator ) : __CRule__Base__(pTranslator) {} \
    void operator() (); \
}

#define MAP_STOKEN2(type, ...) ( CTokenizer:: type , ## __VA_ARGS__ )
#define MAP_STOKEN(elem) CTokenizer::SToken MAP_STOKEN2 elem ,
#define FIRST(...) { FOR_EACH_PARAM(MAP_STOKEN, __VA_ARGS__) CTokenizer::SToken ( CTokenizer::UNKNOWN, "" ) }
#define NEXT(...) { FOR_EACH_PARAM(MAP_STOKEN, __VA_ARGS__) CTokenizer::SToken ( CTokenizer::UNKNOWN, "" ) }
#define DEFINE_RULE(x, first, next) \
const CTokenizer::SToken BUILD_RULE_CLASS_NAME(x) :: ms_firstToken [] = first; \
const CTokenizer::SToken BUILD_RULE_CLASS_NAME(x) :: ms_nextToken [] = next; \
void BUILD_RULE_CLASS_NAME(x) :: operator() ()

#define FIRST_RULE_IS(x) class __CRuleFirst__ : public BUILD_RULE_CLASS_NAME(x) { \
public: \
    __CRuleFirst__ ( CTranslator* pTranslator ) : BUILD_RULE_CLASS_NAME(x) (pTranslator) {} \
    void operator() () { BUILD_RULE_CLASS_NAME(x) :: operator() (); } \
}

#define RULE(T, ...) RULE_I(T, NUMARGS(__VA_ARGS__), __VA_ARGS__)
#define RULE_I(T, n, ...) CAT(RULE_I_, n)(T, ## __VA_ARGS__)
#define RULE_I_0(T, varName) BUILD_RULE_CLASS_NAME(T) ( this->GetTranslator() )
#define RULE_I_1(T, varName) BUILD_RULE_CLASS_NAME(T) (varName) ( this->GetTranslator() ); (varName)

#define EXECUTE_FIRST_RULE(T) __CRuleFirst__ ( this ) ()

// Abstracciones funcionales de la ETDS, y macros de apoyo.
#define THIS (*this)
#define TT(x) CTokenizer:: x
#define TOKEN CTokenizer::SToken
#define MATCH(T, ...) MATCH_I(T, NUMARGS(__VA_ARGS__), __VA_ARGS__); if ( panic_mode() == true ) return
#define MATCH_I(T, n, ...) CAT(MATCH_I_, n)(T, __VA_ARGS__)
#define MATCH_I_0(T, req) match(TT(T), "", ms_nextToken, __FILE__, __LINE__ )
#define MATCH_I_1(T, req) match(TT(T), (req), ms_nextToken, __FILE__, __LINE__ )
#define ADD_INST(x) push_instruction(CString() || x )
#define GET_REF get_ref
#define NEW_IDENT new_ident
#define EMPTY_LIST empty_list
#define INIT_LIST init_list
#define JOIN join_lists
#define COMPLETE complete
#define IS_FIRST(x, ...) is_first ( CTokenizer:: x, ## __VA_ARGS__ )
#define IS_RULE_FIRST(x) is_rule_first ( BUILD_RULE_CLASS_NAME(x) :: ms_firstToken , BUILD_RULE_CLASS_NAME(x) :: ms_nextToken )
#define PANIC() panic(ms_nextToken)
#define WARNING(msg) warning(CString() || msg )
#define ERROR(msg) error(CString() || msg )
#define DIE(msg) die(CString() || msg )

// Foreach
struct __ETDS__Foreach_Iterator
{
    __ETDS__Foreach_Iterator ( const CString& ls )
    {
        CListInString::GetListElements( ls, m_vec );
        iter = m_vec.begin ();
    }
    bool end () const { return iter == m_vec.end(); }
    __ETDS__Foreach_Iterator& operator++() { ++iter; return *this; }
    operator const char*() const { return *(*iter); }

    std::vector<CString> m_vec;
    std::vector<CString>::iterator iter;
};
#define FOREACH_GET_FIRST(a,b) a
#define FOREACH_GET_SECOND(a,b) b
#define FOREACH(ls_iter) FOREACH_I(FOREACH_GET_FIRST(ls_iter), FOREACH_GET_SECOND(ls_iter))
#define FOREACH_I(ls, iter) for ( __ETDS__Foreach_Iterator iter (ls); (iter).end() == false; ++iter )
#define AS ,

// Tipos
#define IS_INTEGER(x)   ( CTypeInfo(x).GetType() == CTypeInfo::INTEGER || IS_ARRAY(x) && ARRAY_CONTENT(x) == CTypeInfo::INTEGER)
#define IS_REAL(x)      ( CTypeInfo(x).GetType() == CTypeInfo::REAL || IS_ARRAY(x) && ARRAY_CONTENT(x) == CTypeInfo::REAL)
#define IS_BOOLEAN(x)   ( CTypeInfo(x).GetType() == CTypeInfo::BOOLEAN || IS_ARRAY(x) && ARRAY_CONTENT(x) == CTypeInfo::BOOLEAN)
#define IS_ARRAY(x)     ( CTypeInfo(x).GetType() == CTypeInfo::ARRAY )
#define IS_VARIABLE(x)  ( IS_INTEGER(x) || IS_REAL(x) || IS_BOOLEAN(x) || IS_ARRAY(x) )
#define IS_PROCEDURE(x) ( CTypeInfo(x).GetType() == CTypeInfo::PROCEDURE )
#define IS_FUNCTION(x)  ( CTypeInfo(x).GetType() == CTypeInfo::FUNCTION )
#define IS_LITERAL(x)   ( x.literal == true )
#define IS_NUMERIC(x)   ( IS_REAL(x) || IS_INTEGER(x) )
#define TYPE_OF(x)      type_of(x)

// Arrays
#define NEW_BASIC_TYPE(x)       ( CTypeInfo(CTypeInfo:: x ).toString() )
#define NEW_ARRAY_TYPE(d,t)     new_array_type(d,t)

#define ARRAY_CONTENT(x)        ( CTypeInfo(x).GetArrayContent()->toString() )
#define ARRAY_SIZE(x)           CString( CTypeInfo(x).GetArraySize() )
#define ARRAY_DEPTH(x)          CTypeInfo(x).GetArrayDepth()
#define ARRAY_DIMENSION(x,y)    CTypeInfo(x).GetArrayDimensions()[y]

// Funciones y procedimientos
#define NEW_FUNCTION_TYPE(x,y,z)    new_array_type(x,y,z)

#define SUBPROG_NUM_PARAMS(x)   ( CTypeInfo(x).GetNumparams )
#define SUBPROG_PARAM(x,y)      ( CTypeInfo(x).GetProcedureParams()[y]->toString() )
#define FUNCTION_RETURN(x)      ( CTypeInfo(x).GetFunctionRetType()->toString() )

// Tabla de símbolos
#define ST_PUSH                 symbol_table_push
#define ST_POP                  symbol_table_pop
#define ST_ADD(x,t)             symbol_table_add((CString)x,(CString)t)
#define ST_GET_TYPE(var)        symbol_table_type((CString)var).toString()

#endif	/* CRULES_INTERNAL_H */

