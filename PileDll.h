// PileDll.h — ce que la DLL rend visible à l'extérieur.
#ifndef PILE_DLL_H
#define PILE_DLL_H

#ifdef _WIN32
    #ifdef PILE_DLL_EXPORTS                      // défini seulement dans PileDll.cpp
        #define PILE_API extern "C" __declspec(dllexport)
    #else
        #define PILE_API extern "C" __declspec(dllimport)
    #endif
#else
    #define PILE_API extern "C"
#endif

// Choix de l'implémentation
#define PILE_TABLEAU 0
#define PILE_LISTE   1

// Retour : 1 = oui, 0 = non, -1 = erreur (expression nulle, exception...)
PILE_API int testerValidite(const char* expr, int typePile);
PILE_API int testerBonOrdre(const char* expr, int typePile);

#endif