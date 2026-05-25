#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "knjiznica.h"

/*
 * pomocne.c — Pomocne funkcije: ispis, unos i izbornik
 *
 * Koncepti u ovoj datoteci:
 *  - static niz (lokalni u funk.) : static const char* nazivi[] u vrsta_u_string()
 *  - PROVJERI_NULL makro          : zastita pokazivaca u citaj_redak() i ispisi_knjigu()
 *  - fgets / strcspn              : sigurno citanje retka bez prekoracenja buffera
 *  - printf formatiranje          : ispis tablice knjiga s poravnanjem (%-30s, %-4d, ...)
 *  - Parametri funkcija           : const Knjiga* k — zastita od nenamjerne izmjene
 */

const char* vrsta_u_string(VrstaKnjige v) {
    static const char* nazivi[VRSTA_COUNT] = {
        "Roman", "Znanstvena fantastika", "Detektivski",
        "Biografija", "Tehnicka literatura", "Poezija",
        "Drama",  "Ostalo"
    };
    if (v < VRSTA_COUNT) return nazivi[v];
    return "Nepoznato";
}

void citaj_redak(char* buf, int vel) {
    PROVJERI_NULL(buf, "citaj_redak: buf je NULL");
    if (fgets(buf, vel, stdin) != NULL)
        buf[strcspn(buf, "\n")] = '\0';
}

void ocisti_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void ispisi_knjigu(const Knjiga* k) {
    PROVJERI_NULL(k, "ispisi_knjigu: k je NULL");
    printf("  %-4d | %-30s | %-20s | %-22s | %4d | %s\n",
        k->id, k->naziv, k->autor,
        vrsta_u_string(k->vrsta),
        k->godina_izdanja,
        k->dostupno ? "DOSTUPNO" : "POSUDENO");
}

void ispisi_zaglavlje(void) {
    printf("  %-4s | %-30s | %-20s | %-22s | %-4s | %s\n",
        "ID", "Naziv", "Autor", "Vrsta", "God.", "Status");
    printf("  %s\n",
        "------|-------------------------------|---------------------"
        "|----------------------|------|----------");
}

void ispisi_meni(void) {
    printf("\n+--------------------------------------+\n");
    printf("|     SKLADISTE KNJIZNICE              |\n");
    printf("+--------------------------------------+\n");
    printf("|  1. Dodaj novu knjigu                |\n");
    printf("|  2. Ispisi sve knjige                |\n");
    printf("|  3. Provjeri dostupnost (po ID)      |\n");
    printf("|  4. Pretrazi (naslov/autor/vrsta)    |\n");
    printf("|  5. Sortiraj i ispisi                |\n");
    printf("|  6. Azuriraj knjigu                  |\n");
    printf("|  7. Obrisi knjigu                    |\n");
    printf("|  8. Povijest pretrage                |\n");
    printf("|  0. Izlaz                            |\n");
    printf("+--------------------------------------+\n");
    printf("Odabir: ");
}