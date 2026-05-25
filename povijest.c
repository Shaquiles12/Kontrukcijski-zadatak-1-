#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "knjiznica.h"

/*
 * povijest.c — Jednostruko povezana lista: povijest pretrage
 *
 * Koncepti u ovoj datoteci:
 *  - Jednostruko povezana lista : CvorPovijesti — svaki cvor cuva upit i broj rezultata
 *  - Dodavanje na pocetak liste : dodaj_povijest() — novi cvor postaje nova glava
 *  - malloc()                   : dinamicka alokacija novog cvora
 *  - SLOBODNA_MEMORIJA makro    : sigurno free() + NULL pri oslobadanju cvora
 *  - Rekurzija                  : ispisi_cvor_rekurzivno() — rekurzivni ispis liste
 *  - static funkcija            : ispisi_cvor_rekurzivno() — vidljiva samo unutar ovog fajla
 *  - extern CvorPovijesti*      : definicija globalne varijable povijest_pretrage (dekl. u .h)
 *  - Oslobadanje cijele liste   : oslobodi_povijest() — poziva main.c pri izlasku programa
 */

CvorPovijesti* povijest_pretrage = NULL;

void dodaj_povijest(const char* upit, int rezultata) {
    PROVJERI_NULL(upit, "dodaj_povijest: upit je NULL");

    CvorPovijesti* novi = malloc(sizeof(CvorPovijesti));
    PROVJERI_NULL(novi, "dodaj_povijest: malloc nije uspio");

    strncpy(novi->upit, upit, MAX_NAZIV - 1);
    novi->upit[MAX_NAZIV - 1] = '\0';
    novi->rezultata = rezultata;
    novi->sljedeci = povijest_pretrage;
    povijest_pretrage = novi;
}

static void ispisi_cvor_rekurzivno(const CvorPovijesti* c, int redni) {
    if (c == NULL) return;
    printf("  %2d. Upit: \"%-30s\"  Rezultata: %d\n",
        redni, c->upit, c->rezultata);
    ispisi_cvor_rekurzivno(c->sljedeci, redni + 1);
}

void ispisi_povijest(void) {
    if (povijest_pretrage == NULL) {
        printf("\n[INFO] Povijest pretrage je prazna.\n");
        return;
    }
    printf("\n─── POVIJEST PRETRAGE ───────────────────\n");
    ispisi_cvor_rekurzivno(povijest_pretrage, 1);
}

void oslobodi_povijest(void) {
    CvorPovijesti* trenutni = povijest_pretrage;
    while (trenutni != NULL) {
        CvorPovijesti* sljedeci = trenutni->sljedeci;
        SLOBODNA_MEMORIJA(trenutni);
        trenutni = sljedeci;
    }
    povijest_pretrage = NULL;
}