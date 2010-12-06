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
    bool                is_rule_first   ( const CTokenizer::SToken tokens [] )
    {
        bool bFound = false;
        for ( unsigned int i = 0;
              bFound == false && tokens[i].eType != CTokenizer::UNKNOWN;
              ++i )
        {
            bFound = is_first ( tokens[i].eType, tokens[i].value );
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
#define MATCH(T, ...) MATCH_I(T, NUMARGS(__VA_ARGS__), __VA_ARGS__)
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
#define ST_PUSH symbol_table_push
#define ST_POP symbol_table_pop
#define IS_FIRST(x, ...) is_first ( CTokenizer:: x, ## __VA_ARGS__ )
#define IS_RULE_FIRST(x) is_rule_first ( BUILD_RULE_CLASS_NAME(x) :: ms_firstToken )

#endif	/* CRULES_INTERNAL_H */

