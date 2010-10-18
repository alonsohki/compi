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

public:
    bool            IsOk            () const;
    bool            Translate       ();

private:
    std::istream&       m_isOrig;
    std::ostream&       m_osDest;
};

#endif	/* CTRANSLATOR_H */
