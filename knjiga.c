#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "knjiznica.h"

/*
 * knjiga.c — CRUID operacije nad strukturom Knjiga
 *
 * Koncepti u ovoj datoteci:
 *  - Create                    : dodaj_knjigu() — dodaje zapis u memoriju i sprema datoteku
 *  - Read                      : ispisi_sve_knjige() — cita i ispisuje sve zapise
 *  - Read                      : provjeri_dostupnost() — trazi zapis po ID-u
 *  - Update                    : azuriraj_knjigu() — mijenja polja postojeceg zapisa
 *  - Delete                    : obrisi_knjigu() — uklanja zapis, pise datoteku iznova
 *  - Pokazivaci na strukture   : Knjiga* knjige, Knjiga* k = &knjige[indeks]
 *  - realloc (preko dodaj_u_niz): prosiruje niz knjiga pri dodavanju nove knjige
 *  - calloc / SLOBODNA_MEMORIJA: alokacija i sigurno oslobadanje niza knjiga
 *  - errno / strerror          : ispis greske pri fopen
 *  - PROVJERI_NULL makro       : zastita pokazivaca
 *  - typedef enum cast         : (VrstaKnjige)vrsta_unos — provjera raspona
 *  - tolower()                 : normalizacija unosa potvrde brisanja (d/n)
 */

void dodaj_knjigu(void) {
    Knjiga nova;
    memset(&nova, 0, sizeof(Knjiga));
    nova.id = sljedeci_id++;

    printf("\n--- DODAJ NOVU KNJIGU -------------------\n");

    printf("Naziv: ");
    citaj_redak(nova.naziv, MAX_NAZIV);
    if (strlen(nova.naziv) == 0) {
        printf("[GRESKA] Naziv ne smije biti prazan.\n");
        sljedeci_id--;
        return;
    }

    printf("Autor: ");
    citaj_redak(nova.autor, MAX_AUTOR);
    if (strlen(nova.autor) == 0) {
        printf("[GRESKA] Autor ne smije biti prazan.\n");
        sljedeci_id--;
        return;
    }

    printf("Vrsta (");
    for (int i = 0; i < VRSTA_COUNT; i++)
        printf("%d=%s ", i, vrsta_u_string((VrstaKnjige)i));
    printf("): ");
    int vrsta_unos;
    if (scanf("%d", &vrsta_unos) != 1 ||
        vrsta_unos < 0 || vrsta_unos >= VRSTA_COUNT) {
        printf("[GRESKA] Neispravna vrsta.\n");
        ocisti_buffer();
        sljedeci_id--;
        return;
    }
    ocisti_buffer();
    nova.vrsta = (VrstaKnjige)vrsta_unos;

    printf("Godina izdanja: ");
    if (scanf("%d", &nova.godina_izdanja) != 1) {
        printf("[GRESKA] Neispravna godina.\n");
        ocisti_buffer();
        sljedeci_id--;
        return;
    }
    ocisti_buffer();
    nova.dostupno = 1;

    /* Ucitaj postojeci niz, prosiri ga pomocu realloc (dodaj_u_niz), spremi */
    Knjiga* knjige = NULL;
    int n = ucitaj_sve(&knjige);
    if (n < 0) {
        printf("[GRESKA] Citanje datoteke nije uspjelo.\n");
        return;
    }

    Knjiga* prosireni = dodaj_u_niz(knjige, n, &nova);
    if (prosireni == NULL) {
        SLOBODNA_MEMORIJA(knjige);
        return;
    }

    if (spremi_sve(prosireni, n + 1) == 0)
        printf("[OK] Knjiga '%s' dodana s ID=%d.\n", nova.naziv, nova.id);

    SLOBODNA_MEMORIJA(prosireni);
}

void ispisi_sve_knjige(void) {
    Knjiga* knjige = NULL;
    int n = ucitaj_sve(&knjige);
    if (n <= 0) {
        printf("\n[INFO] Skladiste je prazno.\n");
        SLOBODNA_MEMORIJA(knjige);
        return;
    }

    printf("\n─── SVE KNJIGE (%d) ──────────────────────\n", n);
    ispisi_zaglavlje();
    for (int i = 0; i < n; i++)
        ispisi_knjigu(&knjige[i]);

    SLOBODNA_MEMORIJA(knjige);
}

void provjeri_dostupnost(void) {
    printf("\nID knjige: ");
    int id;
    if (scanf("%d", &id) != 1) { ocisti_buffer(); return; }
    ocisti_buffer();

    Knjiga* knjige = NULL;
    int n = ucitaj_sve(&knjige);
    if (n <= 0) {
        printf("[INFO] Skladiste je prazno.\n");
        SLOBODNA_MEMORIJA(knjige);
        return;
    }

    int pronadjeno = 0;
    for (int i = 0; i < n; i++) {
        if (knjige[i].id == id) {
            printf("\n─── DOSTUPNOST ───────────────────────────\n");
            ispisi_zaglavlje();
            ispisi_knjigu(&knjige[i]);
            pronadjeno = 1;
            break;
        }
    }
    if (!pronadjeno)
        printf("[INFO] Knjiga s ID=%d nije pronadena.\n", id);

    SLOBODNA_MEMORIJA(knjige);
}

void azuriraj_knjigu(void) {
    printf("\nID knjige za azuriranje: ");
    int id;
    if (scanf("%d", &id) != 1) { ocisti_buffer(); return; }
    ocisti_buffer();

    Knjiga* knjige = NULL;
    int n = ucitaj_sve(&knjige);
    if (n <= 0) {
        printf("[INFO] Skladiste je prazno.\n");
        SLOBODNA_MEMORIJA(knjige);
        return;
    }

    int indeks = -1;
    for (int i = 0; i < n; i++)
        if (knjige[i].id == id) { indeks = i; break; }

    if (indeks == -1) {
        printf("[INFO] Knjiga s ID=%d nije pronadena.\n", id);
        SLOBODNA_MEMORIJA(knjige);
        return;
    }

    Knjiga* k = &knjige[indeks];
    printf("\n─── AZURIRANJE (ID=%d, '%s') ─────────────\n", k->id, k->naziv);
    printf("1=Naziv  2=Autor  3=Vrsta  4=Godina  5=Dostupnost\nOdabir: ");

    int opcija;
    if (scanf("%d", &opcija) != 1) {
        ocisti_buffer();
        SLOBODNA_MEMORIJA(knjige);
        return;
    }
    ocisti_buffer();

    switch (opcija) {
    case 1:
        printf("Novi naziv: ");
        citaj_redak(k->naziv, MAX_NAZIV);
        break;
    case 2:
        printf("Novi autor: ");
        citaj_redak(k->autor, MAX_AUTOR);
        break;
    case 3: {
        printf("Nova vrsta (0-%d): ", VRSTA_COUNT - 1);
        int v;
        if (scanf("%d", &v) == 1 && v >= 0 && v < VRSTA_COUNT)
            k->vrsta = (VrstaKnjige)v;
        else
            printf("[GRESKA] Neispravna vrsta.\n");
        ocisti_buffer();
        break;
    }
    case 4:
        printf("Nova godina: ");
        if (scanf("%d", &k->godina_izdanja) != 1)
            printf("[GRESKA] Neispravna godina.\n");
        ocisti_buffer();
        break;
    case 5:
        k->dostupno = !k->dostupno;
        printf("[OK] Status promijenjen: %s\n",
            k->dostupno ? "DOSTUPNO" : "POSUDENO");
        break;
    default:
        printf("[GRESKA] Neispravan odabir.\n");
        SLOBODNA_MEMORIJA(knjige);
        return;
    }

    if (spremi_sve(knjige, n) == 0)
        printf("[OK] Knjiga azurirana.\n");

    SLOBODNA_MEMORIJA(knjige);
}

void obrisi_knjigu(void) {
    printf("\nID knjige za brisanje: ");
    int id;
    if (scanf("%d", &id) != 1) { ocisti_buffer(); return; }
    ocisti_buffer();

    Knjiga* knjige = NULL;
    int n = ucitaj_sve(&knjige);
    if (n <= 0) {
        printf("[INFO] Skladiste je prazno.\n");
        SLOBODNA_MEMORIJA(knjige);
        return;
    }

    int indeks = -1;
    for (int i = 0; i < n; i++)
        if (knjige[i].id == id) { indeks = i; break; }

    if (indeks == -1) {
        printf("[INFO] Knjiga s ID=%d nije pronadena.\n", id);
        SLOBODNA_MEMORIJA(knjige);
        return;
    }

    printf("Jeste li sigurni da zelite obrisati '%s'? (d/n): ",
        knjige[indeks].naziv);
    char potvrda[4];
    citaj_redak(potvrda, sizeof(potvrda));
    if (tolower((unsigned char)potvrda[0]) != 'd') {
        printf("[INFO] Brisanje otkazano.\n");
        SLOBODNA_MEMORIJA(knjige);
        return;
    }

    for (int i = indeks; i < n - 1; i++)
        knjige[i] = knjige[i + 1];

    if (spremi_sve(knjige, n - 1) == 0)
        printf("[OK] Knjiga s ID=%d obrisana.\n", id);

    SLOBODNA_MEMORIJA(knjige);
}