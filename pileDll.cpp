// PileDll.cpp — corps de la DLL. PAS de main() ici : une DLL n'est pas un programme.
#define PILE_DLL_EXPORTS
#include "PileDll.h"
#include "Pile.h"

#ifdef _WIN32
#include <windows.h>
#endif

// ---------------------------------------------------------------
// Fonctions internes (non exportées)
// ---------------------------------------------------------------
namespace {

bool estOuvrante(char c) { return c == '(' || c == '[' || c == '{'; }
bool estFermante(char c) { return c == ')' || c == ']' || c == '}'; }

bool correspondent(char o, char f) {
    return (o == '(' && f == ')') || (o == '[' && f == ']') || (o == '{' && f == '}');
}

int rang(char c) { return c == '(' ? 1 : (c == '[' ? 2 : 3); }

bool estValide(const char* expr, Pile<char>& p) {
    for (const char* c = expr; *c; ++c) {
        if (estOuvrante(*c)) {
            p.empiler(*c);
        } else if (estFermante(*c)) {
            if (p.estVide()) return false;
            if (!correspondent(p.depiler(), *c)) return false;
        }
    }
    return p.estVide();
}

bool estBonOrdre(const char* expr, Pile<char>& p) {
    for (const char* c = expr; *c; ++c) {
        if (estOuvrante(*c)) {
            if (!p.estVide() && rang(*c) > rang(p.sommet())) return false;
            p.empiler(*c);
        } else if (estFermante(*c)) {
            p.depiler();                         // peut lever une exception si expr invalide
        }
    }
    return true;
}

Pile<char>* creerPile(int typePile) {
    if (typePile == PILE_LISTE) return new PileListe<char>();
    return new PileTableau<char>();
}

// Exécute un algorithme sans laisser passer d'exception hors de la DLL
int executer(bool (*algo)(const char*, Pile<char>&), const char* expr, int typePile) {
    if (!expr) return -1;
    Pile<char>* p = creerPile(typePile);
    int resultat;
    try {
        resultat = algo(expr, *p) ? 1 : 0;
    } catch (...) {
        resultat = -1;
    }
    delete p;
    return resultat;
}

} // namespace

// ---------------------------------------------------------------
// Fonctions exportées
// ---------------------------------------------------------------
PILE_API int testerValidite(const char* expr, int typePile) {
    return executer(estValide, expr, typePile);
}

PILE_API int testerBonOrdre(const char* expr, int typePile) {
    return executer(estBonOrdre, expr, typePile);
}

// Point d'entrée de la DLL (remplace le main, appelé par Windows au chargement)
#ifdef _WIN32
BOOL APIENTRY DllMain(HMODULE, DWORD, LPVOID) {
    return TRUE;
}
#endif