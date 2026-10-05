// test.cpp — programme SÉPARÉ qui charge PileDll.dll et mesure les temps de réponse.
// Il connaît seulement le nom de la DLL et de ses fonctions, pas le code de la pile.
#include <windows.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
using namespace std;

typedef int (*FonctionTest)(const char*, int);

const int PILE_TABLEAU = 0;
const int PILE_LISTE   = 1;

// Pile peu profonde : "{[(3+5)]}" répété -> jamais plus de 3 éléments dans la pile
string exprPlate(int n) {
    string e;
    while ((int)e.size() < n) e += "{[(3+5)]}";
    return e;
}

// Pile très profonde : "((((...7...))))" -> jusqu'à n/2 éléments dans la pile
string exprProfonde(int n) {
    int k = n / 2;
    return string(k, '(') + "7" + string(k, ')');
}

// Temps moyen d'un appel, en microsecondes
double mesurer(FonctionTest f, const string& expr, int typePile, int repetitions) {
    auto debut = chrono::steady_clock::now();
    for (int i = 0; i < repetitions; i++) f(expr.c_str(), typePile);
    auto fin = chrono::steady_clock::now();
    return chrono::duration<double, micro>(fin - debut).count() / repetitions;
}

void comparer(const string& nomAlgo, FonctionTest f, const string& nomExpr, const string& expr) {
    int n = (int)expr.size();
    int repetitions = n >= 1000000 ? 10 : 10000000 / n;   // assez de répétitions pour les petites tailles

    // Les deux piles doivent donner le même résultat
    if (f(expr.c_str(), PILE_TABLEAU) != f(expr.c_str(), PILE_LISTE))
        cout << "  !! resultats differents pour " << nomExpr << endl;

    double tTab   = mesurer(f, expr, PILE_TABLEAU, repetitions);
    double tListe = mesurer(f, expr, PILE_LISTE,   repetitions);

    cout << left  << setw(10) << nomAlgo << setw(10) << nomExpr
         << right << setw(10) << n
         << setw(15) << fixed << setprecision(2) << tTab
         << setw(15) << tListe
         << setw(10) << setprecision(2) << tListe / tTab << "x" << endl;
}

// Texte du résultat pour une expression et une pile
string resultat(FonctionTest validite, FonctionTest bonOrdre, const char* expr, int typePile) {
    if (validite(expr, typePile) != 1) return "invalide";
    return bonOrdre(expr, typePile) == 1 ? "valide, bon ordre" : "valide, mauvais ordre";
}

// Partie 1 : vérification des expressions de test avec les deux piles
void verifierExpressions(FonctionTest validite, FonctionTest bonOrdre) {
    const char* expressions[] = {
        // valides et dans le bon ordre
        "{[(3+5)*2]-4}", "[(12+8)*(7-3)]", "((2+3)*4)", "15+27",
        // valides mais dans le mauvais ordre
        "([4+6]*2)", "[{9+1}]",
        // invalides
        "(5+3]", "{[(2])}", "((7+2)", "8+4)"
    };

    cout << "=== Verification des expressions ===" << endl;
    cout << left << setw(18) << "Expression" << setw(26) << "Pile liste" << "Pile tableau" << endl;
    cout << string(70, '-') << endl;
    for (const char* e : expressions) {
        cout << left << setw(18) << e
             << setw(26) << resultat(validite, bonOrdre, e, PILE_LISTE)
             << resultat(validite, bonOrdre, e, PILE_TABLEAU) << endl;
    }
    cout << endl;
}

int main() {
    HINSTANCE dll = LoadLibrary(TEXT("PileDll.dll"));
    if (!dll) {
        cout << "ERREUR : impossible de charger PileDll.dll" << endl;
        return 1;
    }

    FonctionTest validite = (FonctionTest)GetProcAddress(dll, "testerValidite");
    FonctionTest bonOrdre = (FonctionTest)GetProcAddress(dll, "testerBonOrdre");
    if (!validite || !bonOrdre) {
        cout << "ERREUR : fonctions introuvables dans la DLL" << endl;
        FreeLibrary(dll);
        return 1;
    }

    verifierExpressions(validite, bonOrdre);

    cout << "=== Mesure des temps de reponse ===" << endl;
    cout << left  << setw(10) << "Algo" << setw(10) << "Expr"
         << right << setw(10) << "Taille"
         << setw(15) << "Tableau (us)" << setw(15) << "Liste (us)"
         << setw(11) << "Liste/Tab" << endl;
    cout << string(71, '-') << endl;

    int tailles[] = {1000, 10000, 100000, 1000000};
    for (int n : tailles) {
        string plate = exprPlate(n), profonde = exprProfonde(n);
        comparer("validite", validite, "plate",    plate);
        comparer("validite", validite, "profonde", profonde);
        comparer("ordre",    bonOrdre, "plate",    plate);
        comparer("ordre",    bonOrdre, "profonde", profonde);
    }

    FreeLibrary(dll);
    return 0;
}