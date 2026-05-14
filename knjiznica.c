#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "knjiznica.h"

/*
 * knjiznica.c – Implementacija svih funkcija sustava skladišta knjižnice
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "knjiznica.h"

 /* ─── Globalna varijabla (extern u headeru) ──────────────────── */
int sljedeci_id = 1;

/* ════════════════════════════════════════════════════════════════
 *  POMOĆNE FUNKCIJE
 * ════════════════════════════════════════════════════════════════ */

const char* vrsta_u_string(VrstaKnjige v) {
    static const char* nazivi[VRSTA_COUNT] = {
        "Roman", "Znanstvena fantastika", "Detektivski",
        "Biografija", "Tehnicka literatura", "Poezija", "Drama", "Ostalo"
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
        k->id,
        k->naziv,
        k->autor,
        vrsta_u_string(k->vrsta),
        k->godina_izdanja,
        k->dostupno ? "DOSTUPNO" : "POSUDENO");
}

void ispisi_zaglavlje(void) {
    printf("  %-4s | %-30s | %-20s | %-22s | %-4s | %s\n",
        "ID", "Naziv", "Autor", "Vrsta", "God.", "Status");
    printf("  %s\n",
        "------|-------------------------------|---------------------|----------------------|------|----------");
}

/* ════════════════════════════════════════════════════════════════
 *  I/O DATOTEKA
 * ════════════════════════════════════════════════════════════════ */

int ucitaj_sve(Knjiga** out) {
    PROVJERI_NULL_RET(out, "ucitaj_sve: out je NULL", -1);

    FILE* fp = fopen(DATOTEKA, "rb");
    if (fp == NULL) { *out = NULL; return 0; }

    if (fseek(fp, 0, SEEK_END) != 0) { perror("fseek"); fclose(fp); return -1; }
    long vel = ftell(fp);
    if (vel < 0) { perror("ftell"); fclose(fp); return -1; }
    rewind(fp);

    int n = (int)(vel / sizeof(Knjiga));
    if (n == 0) { *out = NULL; fclose(fp); return 0; }

    *out = malloc((size_t)n * sizeof(Knjiga));
    PROVJERI_NULL_RET(*out, "ucitaj_sve: malloc nije uspio", -1);

    int procitano = (int)fread(*out, sizeof(Knjiga), (size_t)n, fp);
    if (ferror(fp)) {
        perror("fread");
        free(*out); *out = NULL;
        fclose(fp);
        return -1;
    }
    fclose(fp);

    for (int i = 0; i < procitano; i++)
        if ((*out)[i].id >= sljedeci_id)
            sljedeci_id = (*out)[i].id + 1;

    return procitano;
}

int spremi_sve(const Knjiga* knjige, int n) {
    PROVJERI_NULL_RET(knjige, "spremi_sve: knjige je NULL", -1);

    remove(BACKUP_DAT);
    rename(DATOTEKA, BACKUP_DAT);

    FILE* fp = fopen(DATOTEKA, "wb");
    if (fp == NULL) {
        perror("fopen (spremi_sve)");
        rename(BACKUP_DAT, DATOTEKA);
        return -1;
    }

    if (n > 0) {
        size_t zapisano = fwrite(knjige, sizeof(Knjiga), (size_t)n, fp);
        if (zapisano != (size_t)n) {
            perror("fwrite");
            fclose(fp);
            rename(BACKUP_DAT, DATOTEKA);
            return -1;
        }
    }
    fclose(fp);
    return 0;
}

/* ════════════════════════════════════════════════════════════════
 *  COMPARATORI ZA qsort / bsearch
 * ════════════════════════════════════════════════════════════════ */

int cmp_naslov(const void* a, const void* b) {
    return strcmp(((const Knjiga*)a)->naziv, ((const Knjiga*)b)->naziv);
}
int cmp_autor(const void* a, const void* b) {
    return strcmp(((const Knjiga*)a)->autor, ((const Knjiga*)b)->autor);
}
int cmp_vrsta(const void* a, const void* b) {
    return (int)((const Knjiga*)a)->vrsta - (int)((const Knjiga*)b)->vrsta;
}

/* ════════════════════════════════════════════════════════════════
 *  CREATE
 * ════════════════════════════════════════════════════════════════ */

void dodaj_knjigu(void) {
    Knjiga nova;
    memset(&nova, 0, sizeof(Knjiga));
    nova.id = sljedeci_id++;

    printf("\n─── DODAJ NOVU KNJIGU ───────────────────\n");

    printf("Naziv: ");
    citaj_redak(nova.naziv, MAX_NAZIV);
    if (strlen(nova.naziv) == 0) { printf("[GREŠKA] Naziv ne smije biti prazan.\n"); return; }

    printf("Autor: ");
    citaj_redak(nova.autor, MAX_AUTOR);
    if (strlen(nova.autor) == 0) { printf("[GREŠKA] Autor ne smije biti prazan.\n"); return; }

    printf("Vrsta (");
    for (int i = 0; i < VRSTA_COUNT; i++)
        printf("%d=%s ", i, vrsta_u_string((VrstaKnjige)i));
    printf("): ");
    int vrsta_unos;
    if (scanf("%d", &vrsta_unos) != 1 || vrsta_unos < 0 || vrsta_unos >= VRSTA_COUNT) {
        printf("[GREŠKA] Neispravna vrsta.\n"); ocisti_buffer(); return;
    }
    ocisti_buffer();
    nova.vrsta = (VrstaKnjige)vrsta_unos;

    printf("Godina izdanja: ");
    if (scanf("%d", &nova.godina_izdanja) != 1) {
        printf("[GREŠKA] Neispravna godina.\n"); ocisti_buffer(); return;
    }
    ocisti_buffer();
    nova.dostupno = 1;

    FILE* fp = fopen(DATOTEKA, "ab");
    if (fp == NULL) { perror("fopen (dodaj)"); return; }
    fwrite(&nova, sizeof(Knjiga), 1, fp);
    fclose(fp);

    printf("[OK] Knjiga '%s' dodana s ID=%d.\n", nova.naziv, nova.id);
}

/* ════════════════════════════════════════════════════════════════
 *  READ
 * ════════════════════════════════════════════════════════════════ */

void ispisi_sve_knjige(void) {
    Knjiga* knjige = NULL;
    int n = ucitaj_sve(&knjige);
    if (n <= 0) { printf("\n[INFO] Skladište je prazno.\n"); free(knjige); return; }

    printf("\n─── SVE KNJIGE (%d) ──────────────────────\n", n);
    ispisi_zaglavlje();
    for (int i = 0; i < n; i++)
        ispisi_knjigu(&knjige[i]);

    free(knjige);
}

void provjeri_dostupnost(void) {
    printf("\nID knjige: ");
    int id;
    if (scanf("%d", &id) != 1) { ocisti_buffer(); return; }
    ocisti_buffer();

    Knjiga* knjige = NULL;
    int n = ucitaj_sve(&knjige);
    for (int i = 0; i < n; i++) {
        if (knjige[i].id == id) {
            printf("\n─── DOSTUPNOST ───────────────────────────\n");
            ispisi_zaglavlje();
            ispisi_knjigu(&knjige[i]);
            free(knjige);
            return;
        }
    }
    printf("[INFO] Knjiga s ID=%d nije pronađena.\n", id);
    free(knjige);
}

void pretrazi_knjige(void) {
    printf("\nPretraži po: 1=Naslov  2=Autor  3=Vrsta\nOdabir: ");
    int odabir;
    if (scanf("%d", &odabir) != 1) { ocisti_buffer(); return; }
    ocisti_buffer();

    Knjiga* knjige = NULL;
    int n = ucitaj_sve(&knjige);
    if (n <= 0) { printf("[INFO] Skladište je prazno.\n"); free(knjige); return; }

    int pronadjeno = 0;

    if (odabir == 1) {
        qsort(knjige, (size_t)n, sizeof(Knjiga), cmp_naslov);
        printf("Dio naziva: ");
        char upit[MAX_NAZIV];
        citaj_redak(upit, MAX_NAZIV);
        printf("\n─── REZULTATI (naslov) ───────────────────\n");
        ispisi_zaglavlje();
        for (int i = 0; i < n; i++)
            if (strstr(knjige[i].naziv, upit)) { ispisi_knjigu(&knjige[i]); pronadjeno++; }

    }
    else if (odabir == 2) {
        printf("Dio autora: ");
        char upit[MAX_AUTOR];
        citaj_redak(upit, MAX_AUTOR);
        printf("\n─── REZULTATI (autor) ────────────────────\n");
        ispisi_zaglavlje();
        for (int i = 0; i < n; i++)
            if (strstr(knjige[i].autor, upit)) { ispisi_knjigu(&knjige[i]); pronadjeno++; }

    }
    else if (odabir == 3) {
        printf("Vrsta (0-%d): ", VRSTA_COUNT - 1);
        int v;
        if (scanf("%d", &v) != 1 || v < 0 || v >= VRSTA_COUNT) {
            ocisti_buffer(); free(knjige); return;
        }
        ocisti_buffer();
        printf("\n─── REZULTATI (vrsta: %s) ────────────\n", vrsta_u_string((VrstaKnjige)v));
        ispisi_zaglavlje();
        for (int i = 0; i < n; i++)
            if ((int)knjige[i].vrsta == v) { ispisi_knjigu(&knjige[i]); pronadjeno++; }

    }
    else {
        printf("[GRESKA] Neispravan odabir.\n");
    }

    if (pronadjeno == 0) printf("[INFO] Nema rezultata.\n");
    else printf("[INFO] Pronađeno: %d\n", pronadjeno);

    free(knjige);
}

/* ════════════════════════════════════════════════════════════════
 *  UPDATE
 * ════════════════════════════════════════════════════════════════ */

void azuriraj_knjigu(void) {
    printf("\nID knjige za azuriranje: ");
    int id;
    if (scanf("%d", &id) != 1) { ocisti_buffer(); return; }
    ocisti_buffer();

    Knjiga* knjige = NULL;
    int n = ucitaj_sve(&knjige);

    int indeks = -1;
    for (int i = 0; i < n; i++)
        if (knjige[i].id == id) { indeks = i; break; }

    if (indeks == -1) {
        printf("[INFO] Knjiga s ID=%d nije pronadena.\n", id);
        free(knjige); return;
    }

    Knjiga* k = &knjige[indeks];
    printf("\n─── AZURIRANJE (ID=%d, '%s') ─────────────\n", k->id, k->naziv);
    printf("1=Naziv  2=Autor  3=Vrsta  4=Godina  5=Dostupnost\nOdabir: ");

    int opcija;
    if (scanf("%d", &opcija) != 1) { ocisti_buffer(); free(knjige); return; }
    ocisti_buffer();

    switch (opcija) {
    case 1: printf("Novi naziv: "); citaj_redak(k->naziv, MAX_NAZIV); break;
    case 2: printf("Novi autor: "); citaj_redak(k->autor, MAX_AUTOR); break;
    case 3:
        printf("Nova vrsta (0-%d): ", VRSTA_COUNT - 1);
        { int v; if (scanf("%d", &v) == 1 && v >= 0 && v < VRSTA_COUNT) k->vrsta = (VrstaKnjige)v; }
        ocisti_buffer(); break;
    case 4:
        printf("Nova godina: ");
        scanf("%d", &k->godina_izdanja);
        ocisti_buffer(); break;
    case 5:
        k->dostupno = !k->dostupno;
        printf("[OK] Status: %s\n", k->dostupno ? "DOSTUPNO" : "POSUDENO");
        break;
    default:
        printf("[GRESKA] Neispravan odabir.\n");
        free(knjige); return;
    }

    if (spremi_sve(knjige, n) == 0)
        printf("[OK] Knjiga azurirana.\n");

    free(knjige);
}

/* ════════════════════════════════════════════════════════════════
 *  DELETE
 * ════════════════════════════════════════════════════════════════ */

void obrisi_knjigu(void) {
    printf("\nID knjige za brisanje: ");
    int id;
    if (scanf("%d", &id) != 1) { ocisti_buffer(); return; }
    ocisti_buffer();

    Knjiga* knjige = NULL;
    int n = ucitaj_sve(&knjige);

    int indeks = -1;
    for (int i = 0; i < n; i++)
        if (knjige[i].id == id) { indeks = i; break; }

    if (indeks == -1) {
        printf("[INFO] Knjiga s ID=%d nije pronađena.\n", id);
        free(knjige); return;
    }

    printf("Jeste li sigurni da zelite obrisati '%s'? (d/n): ", knjige[indeks].naziv);
    char potvrda[4];
    citaj_redak(potvrda, sizeof(potvrda));
    if (tolower((unsigned char)potvrda[0]) != 'd') {
        printf("[INFO] Brisanje otkazano.\n");
        free(knjige); return;
    }

    for (int i = indeks; i < n - 1; i++)
        knjige[i] = knjige[i + 1];

    if (spremi_sve(knjige, n - 1) == 0)
        printf("[OK] Knjiga s ID=%d obrisana.\n", id);

    free(knjige);
}

/* ════════════════════════════════════════════════════════════════
 *  SORTIRANJE
 * ════════════════════════════════════════════════════════════════ */

void sortiraj_i_ispisi(void) {
    printf("\nSortiraj po: 1=Naslov  2=Autor  3=Vrsta\nOdabir: ");
    int odabir;
    if (scanf("%d", &odabir) != 1) { ocisti_buffer(); return; }
    ocisti_buffer();

    Knjiga* knjige = NULL;
    int n = ucitaj_sve(&knjige);
    if (n <= 0) { printf("[INFO] Skladiste je prazno.\n"); free(knjige); return; }

    const char* kriterij = "";
    switch (odabir) {
    case 1: qsort(knjige, (size_t)n, sizeof(Knjiga), cmp_naslov); kriterij = "naslov"; break;
    case 2: qsort(knjige, (size_t)n, sizeof(Knjiga), cmp_autor);  kriterij = "autor";  break;
    case 3: qsort(knjige, (size_t)n, sizeof(Knjiga), cmp_vrsta);  kriterij = "vrsta";  break;
    default: printf("[GRESKA] Neispravan odabir.\n"); free(knjige); return;
    }

    printf("\n─── SORTIRANO PO: %s ─────────────────────\n", kriterij);
    ispisi_zaglavlje();
    for (int i = 0; i < n; i++)
        ispisi_knjigu(&knjige[i]);

    free(knjige);
}

/* ─── Izbornik ───────────────────────────────────────────────── */

void ispisi_meni(void) {
    printf("\n╔══════════════════════════════════════╗\n");
    printf("║     SKLADISTE KNJIZNICE              ║\n");
    printf("╠══════════════════════════════════════╣\n");
    printf("║  1. Dodaj novu knjigu                ║\n");
    printf("║  2. Ispisi sve knjige                ║\n");
    printf("║  3. Provjeri dostupnost (po ID)      ║\n");
    printf("║  4. Pretrazi (naslov/autor/vrsta)    ║\n");
    printf("║  5. Sortiraj i ispisi                ║\n");
    printf("║  6. Azuriraj knjigu                  ║\n");
    printf("║  7. Obriši knjigu                    ║\n");
    printf("║  0. Izlaz                            ║\n");
    printf("╚══════════════════════════════════════╝\n");
    printf("Odabir: ");
}