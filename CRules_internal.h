#ifndef CRULES_INTERNAL_H
#define	CRULES_INTERNAL_H

#include <vector>
#include "CString.h"

// Estructura de datos base de las reglas.
class __CRule__Base__
{
public:
    __CRule__Base__ ( std::vector<CString>& vecInstructions )
    : m_vecInstructions ( vecInstructions )
    {}

    std::vector<CString>&   GetInstructionsVector () { return m_vecInstructions; }
    
private:
    std::vector<CString>&   m_vecInstructions;
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
#define FOR_EACH_PARAM_I_1(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_0(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_2(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_1(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_3(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_2(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_4(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_3(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_5(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_4(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_6(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_5(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_7(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_6(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_8(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_7(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_9(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_8(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_10(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_9(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_11(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_10(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_12(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_11(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_13(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_12(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_14(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_13(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_15(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_14(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_16(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_15(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_17(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_16(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_18(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_17(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_19(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_18(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_20(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_19(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_21(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_20(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_22(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_21(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_23(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_22(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_24(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_23(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_25(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_24(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_26(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_25(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_27(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_26(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_28(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_27(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_29(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_28(fn, __VA_ARGS__)
#define FOR_EACH_PARAM_I_30(fn, elem, ...) fn(elem); FOR_EACH_PARAM_I_29(fn, __VA_ARGS__)




#define BUILD_RULE_CLASS_NAME(x) __CRuleDef__ ## x ## __
#define MAP_ATTRIBUTE_TYPE(attr) CString (attr)
#define DECLARE_RULE(x, ...) class BUILD_RULE_CLASS_NAME(x) : public __CRule__Base__ \
{ \
    FOR_EACH_PARAM(MAP_ATTRIBUTE_TYPE, __VA_ARGS__); \
public: BUILD_RULE_CLASS_NAME(x) (std::vector<CString>& v) : __CRule__Base__(v) {} \
        void operator() (); \
}
#define DEFINE_RULE(x) void BUILD_RULE_CLASS_NAME(x) :: operator() ()

#define THIS (*this)

#define RULE(T, ...) RULE_I(T, NUMARGS(__VA_ARGS__), __VA_ARGS__)
#define RULE_I(T, n, ...) CAT(RULE_I_, n)(T, ## __VA_ARGS__)
#define RULE_I_0(T, varName) BUILD_RULE_CLASS_NAME(T) ( this->GetInstructions() )
#define RULE_I_1(T, varName) BUILD_RULE_CLASS_NAME(T) (varName) ( this->GetInstructions() ); (varName)

#endif	/* CRULES_INTERNAL_H */

