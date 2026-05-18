#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "knjiznica.h"

/* pretraga.c - Komparatori, pretraga i sortiranje */

/* ═══════════════════════════════════════════════════════════════
 *  KOMPARATORI za qsort / bsearch
 * ═══════════════════════════════════════════════════════════════ */

int cmp_naslov(const void* a, const void* b) {
    return strcmp(((const Knjiga*)a)->naziv,
        ((const Knjiga*)b)->naziv);
}

int cmp_autor(const void* a, const void* b) {
    return strcmp(((const Knjiga*)a)->autor,
        ((const Knjiga*)b)->autor);
}

int cmp_vrsta(const void* a, const void* b) {
    return (int)((const Knjiga*)a)->vrsta
        - (int)((const Knjiga*)b)->vrsta;
}

/* ═══════════════════════════════════════════════════════════════
 *  PRETRAGA
 * ═══════════════════════════════════════════════════════════════ */

void pretrazi_knjige(void) {
    printf("\nPretrazi po: 1=Naslov  2=Autor  3=Vrsta\nOdabir: ");
    int odabir;
    if (scanf("%d", &odabir) != 1) { ocisti_buffer(); return; }
    ocisti_buffer();

    Knjiga* knjige = NULL;
    int n = ucitaj_sve(&knjige);
    if (n <= 0) {
        printf("[INFO] Skladiste je prazno.\n");
        SLOBODNA_MEMORIJA(knjige);
        return;
    }

    static int ukupno_pretraga = 0;
    ukupno_pretraga++;

    int pronadjeno = 0;
    char upit[MAX_NAZIV] = { 0 };

    if (odabir == 1) {
        printf("Dio naziva (ili tocan naziv za brzu pretragu): ");
        citaj_redak(upit, MAX_NAZIV);

        qsort(knjige, (size_t)n, sizeof(Knjiga), cmp_naslov);
        Knjiga kljuc;
        memset(&kljuc, 0, sizeof(Knjiga));
        strncpy(kljuc.naziv, upit, MAX_NAZIV - 1);

        Knjiga* nadjena = bsearch(&kljuc, knjige, (size_t)n,
            sizeof(Knjiga), cmp_naslov);

        printf("\n─── REZULTATI (naslov) ───────────────────\n");
        ispisi_zaglavlje();

        if (nadjena != NULL) {
            ispisi_knjigu(nadjena);
            pronadjeno = 1;
        }
        else {
            for (int i = 0; i < n; i++) {
                if (strstr(knjige[i].naziv, upit)) {
                    ispisi_knjigu(&knjige[i]);
                    pronadjeno++;
                }
            }
        }

    }
    else if (odabir == 2) {
        printf("Dio autora: ");
        citaj_redak(upit, MAX_AUTOR);

        printf("\n─── REZULTATI (autor) ────────────────────\n");
        ispisi_zaglavlje();
        for (int i = 0; i < n; i++) {
            if (strstr(knjige[i].autor, upit)) {
                ispisi_knjigu(&knjige[i]);
                pronadjeno++;
            }
        }

    }
    else if (odabir == 3) {
        printf("Vrsta (0-%d): ", VRSTA_COUNT - 1);
        int v;
        if (scanf("%d", &v) != 1 || v < 0 || v >= VRSTA_COUNT) {
            ocisti_buffer();
            SLOBODNA_MEMORIJA(knjige);
            return;
        }
        ocisti_buffer();
        snprintf(upit, sizeof(upit), "Vrsta:%s",
            vrsta_u_string((VrstaKnjige)v));

        printf("\n─── REZULTATI (vrsta: %s) ─────────────\n",
            vrsta_u_string((VrstaKnjige)v));
        ispisi_zaglavlje();
        for (int i = 0; i < n; i++) {
            if ((int)knjige[i].vrsta == v) {
                ispisi_knjigu(&knjige[i]);
                pronadjeno++;
            }
        }

    }
    else {
        printf("[GRESKA] Neispravan odabir.\n");
        SLOBODNA_MEMORIJA(knjige);
        return;
    }

    if (pronadjeno == 0)
        printf("[INFO] Nema rezultata.\n");
    else
        printf("[INFO] Pronadjeno: %d  (Ukupno pretraga ovaj session: %d)\n",
            pronadjeno, ukupno_pretraga);

    dodaj_povijest(upit, pronadjeno);

    SLOBODNA_MEMORIJA(knjige);
}

/* ═══════════════════════════════════════════════════════════════
 *  SORTIRANJE
 * ═══════════════════════════════════════════════════════════════ */

void sortiraj_i_ispisi(void) {
    printf("\nSortiraj po: 1=Naslov  2=Autor  3=Vrsta\nOdabir: ");
    int odabir;
    if (scanf("%d", &odabir) != 1) { ocisti_buffer(); return; }
    ocisti_buffer();

    Knjiga* knjige = NULL;
    int n = ucitaj_sve(&knjige);
    if (n <= 0) {
        printf("[INFO] Skladiste je prazno.\n");
        SLOBODNA_MEMORIJA(knjige);
        return;
    }

    const char* kriterij = "";
    switch (odabir) {
    case 1:
        qsort(knjige, (size_t)n, sizeof(Knjiga), cmp_naslov);
        kriterij = "naslov";
        break;
    case 2:
        qsort(knjige, (size_t)n, sizeof(Knjiga), cmp_autor);
        kriterij = "autor";
        break;
    case 3:
        qsort(knjige, (size_t)n, sizeof(Knjiga), cmp_vrsta);
        kriterij = "vrsta";
        break;
    default:
        printf("[GRESKA] Neispravan odabir.\n");
        SLOBODNA_MEMORIJA(knjige);
        return;
    }

    printf("\n─── SORTIRANO PO: %s ─────────────────────\n", kriterij);
    ispisi_zaglavlje();
    for (int i = 0; i < n; i++)
        ispisi_knjigu(&knjige[i]);

    SLOBODNA_MEMORIJA(knjige);
}