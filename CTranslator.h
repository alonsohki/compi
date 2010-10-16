/* 
 * File:   CTranslator.h
 * Author: ryden
 *
 * Created on 16 de octubre de 2010, 17:31
 */

#ifndef CTRANSLATOR_H
#define	CTRANSLATOR_H

#include <string>
#include <iostream>
#include <fstream>

class CTranslator {
public:
                    CTranslator     (std::istream& ifsOrig,
                                     std::ostream& ofsDest);
    virtual         ~CTranslator    ();
private:
    // No permitimos copias.
                    CTranslator     ( const CTranslator& orig )
                    : m_isOrig ( orig.m_isOrig )
                    , m_osDest ( orig.m_osDest ) {}
    CTranslator&    operator=       ( const CTranslator& orig ) { return *this; }

public:
    bool            IsOk            () const;
    bool            Translate       ();

private:
    std::istream&       m_isOrig;
    std::ostream&       m_osDest;
};

#endif	/* CTRANSLATOR_H */
