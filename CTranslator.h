#ifndef CTRANSLATOR_H
#define	CTRANSLATOR_H

#include <iostream>
#include <fstream>
#include <vector>
#include "CTokenizer.h"
#include "CString.h"

class CTranslator
{
public:
                            CTranslator     (std::istream& ifsOrig,
                                             std::ostream& ofsDest);
    virtual                 ~CTranslator    ();

    std::vector<CString>&   GetInstructions () { return m_vecInstructions; }

public:
    bool                    IsOk            () const;
    bool                    Translate       ();

private:
    std::istream&           m_isOrig;
    std::ostream&           m_osDest;
    CTokenizer              m_tokenizer;
    std::vector<CString>    m_vecInstructions;
};

#endif	/* CTRANSLATOR_H */
