/* 
 * File:   CTranslator.cpp
 * Author: ryden
 * 
 * Created on 16 de octubre de 2010, 17:31
 */

#include "CTranslator.h"

CTranslator::CTranslator(std::istream& ifsOrig,
                         std::ostream& ofsDest)
: m_isOrig ( ifsOrig )
, m_osDest ( ofsDest )
{
}

CTranslator::~CTranslator()
{
}

bool CTranslator::IsOk () const
{
    bool bStreamsAreOpen = !m_isOrig.fail () && !m_osDest.fail ();
    return bStreamsAreOpen;
}

bool CTranslator::Translate ()
{
    return true;
}