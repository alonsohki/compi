/* 
 * File:   main.cpp
 * Author: ryden
 *
 * Created on 16 de octubre de 2010, 17:15
 */

#include <cstdio>
#include <cstdlib>
#include "util.h"
#include "CTranslator.h"

/*
 * 
 */
int main(int argc, const char** argv)
{
    // Obtenemos los descriptores de entrada y salida de acuerdo a los parámetros
    // especificados.
    std::ifstream ifsOrig;
    std::ofstream ofsDest;
    std::istream* pInput;
    std::ostream* pOutput;

    // Si no nos dan un fichero de entrada, usamos la entrada estándar.
    if ( argc < 2 )
    {
        pInput = &std::cin;
        pOutput = &std::cout;
    }
    else if ( argc < 3 )
    {
        // Si no nos dan un fichero de salida, usamos la salida estándar.
        ifsOrig.open ( argv[1], std::ios::in );
        pInput = &ifsOrig;
        pOutput = &std::cout;
    }
    else
    {
        // Nos dan tanto un fichero de entrada como un fichero de salida.
        ifsOrig.open ( argv[1], std::ios::in );
        ofsDest.open ( argv[2], std::ios::out );
        pInput = &ifsOrig;
        pOutput = &ofsDest;
    }


    // Creamos el traductor y traducimos.
    CTranslator translator ( *pInput, *pOutput );
    if ( translator.Translate() == true )
        return EXIT_SUCCESS;

    // Ha sucedido algún error al traducir.
    // Aquí debería haber código para mostrar el error...
    return EXIT_FAILURE;
}

