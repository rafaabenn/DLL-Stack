// Pile.h — Pile générique : interface abstraite + deux implémentations.
// Ce fichier est INTERNE à la DLL (le programme de test n'en a pas besoin).
#ifndef PILE_H
#define PILE_H

#include <stdexcept>

// ---------------------------------------------------------------
// Interface commune
// ---------------------------------------------------------------
template <class T>
class Pile {
public:
    virtual void empiler(const T& x) = 0;
    virtual T depiler() = 0;
    virtual const T& sommet() const = 0;
    virtual bool estVide() const = 0;
    virtual ~Pile() {}
};

// ---------------------------------------------------------------
// Pile par tableau dynamique (capacité doublée quand elle est pleine)
// ---------------------------------------------------------------
template <class T>
class PileTableau : public Pile<T> {
    T*  tab;
    int nb;
    int capacite;

    void agrandir() {
        T* nouveau = new T[capacite * 2];
        for (int i = 0; i < nb; i++) nouveau[i] = tab[i];
        delete[] tab;
        tab = nouveau;
        capacite *= 2;
    }

public:
    explicit PileTableau(int cap = 2) : tab(new T[cap]), nb(0), capacite(cap) {}

    // Règle des 3
    PileTableau(const PileTableau& p) : tab(new T[p.capacite]), nb(p.nb), capacite(p.capacite) {
        for (int i = 0; i < nb; i++) tab[i] = p.tab[i];
    }
    PileTableau& operator=(const PileTableau& p) {
        if (this != &p) {
            T* copie = new T[p.capacite];
            for (int i = 0; i < p.nb; i++) copie[i] = p.tab[i];
            delete[] tab;
            tab = copie;
            nb = p.nb;
            capacite = p.capacite;
        }
        return *this;
    }
    ~PileTableau() { delete[] tab; }

    void empiler(const T& x) override {
        if (nb == capacite) agrandir();
        tab[nb++] = x;
    }
    T depiler() override {
        if (estVide()) throw std::underflow_error("depiler sur une pile vide");
        return tab[--nb];
    }
    const T& sommet() const override {
        if (estVide()) throw std::underflow_error("sommet d'une pile vide");
        return tab[nb - 1];
    }
    bool estVide() const override { return nb == 0; }
};

// ---------------------------------------------------------------
// Pile par liste chaînée (le sommet est la tête de la liste)
// ---------------------------------------------------------------
template <class T>
class PileListe : public Pile<T> {
    struct Noeud {
        T      valeur;
        Noeud* suivant;
        Noeud(const T& v, Noeud* s) : valeur(v), suivant(s) {}
    };
    Noeud* tete;

    void vider() {
        while (tete) {
            Noeud* ancien = tete;
            tete = tete->suivant;
            delete ancien;
        }
    }
    void copier(const PileListe& p) {          // garde le même ordre
        Noeud** fin = &tete;
        for (Noeud* n = p.tete; n; n = n->suivant) {
            *fin = new Noeud(n->valeur, nullptr);
            fin = &(*fin)->suivant;
        }
    }

public:
    PileListe() : tete(nullptr) {}

    // Règle des 3
    PileListe(const PileListe& p) : tete(nullptr) { copier(p); }
    PileListe& operator=(const PileListe& p) {
        if (this != &p) { vider(); copier(p); }
        return *this;
    }
    ~PileListe() { vider(); }

    void empiler(const T& x) override { tete = new Noeud(x, tete); }
    T depiler() override {
        if (estVide()) throw std::underflow_error("depiler sur une pile vide");
        Noeud* ancien = tete;
        T v = ancien->valeur;
        tete = tete->suivant;
        delete ancien;
        return v;
    }
    const T& sommet() const override {
        if (estVide()) throw std::underflow_error("sommet d'une pile vide");
        return tete->valeur;
    }
    bool estVide() const override { return tete == nullptr; }
};

#endif