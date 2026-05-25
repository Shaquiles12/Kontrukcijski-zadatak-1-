#pragma once
#ifndef KNJIZNICA_H
#define KNJIZNICA_H

/*
 * knjiznica.h — Zajednicki header za cijeli projekt
 *
 * Koncepti u ovoj datoteci:
 *  - #define konstante         : MAX_NAZIV, MAX_AUTOR, DATOTEKA, BACKUP_DAT
 *  - Makro funkcije            : PROVJERI_NULL, PROVJERI_NULL_RET, SLOBODNA_MEMORIJA
 *  - static inline funkcije    : max_int(), ocisti_ekran()
 *  - typedef enum              : VrstaKnjige (ROMAN, ZNANSTVENA_FANTASTIKA, ...)
 *  - typedef enum              : OpcijaMenua (IZLAZ, DODAJ, ISPISI_SVE, ...)
 *  - typedef struct            : Knjiga (id, naziv, autor, vrsta, godina, dostupno)
 *  - typedef struct            : CvorPovijesti (cvor jednostruko povezane liste)
 *  - extern deklaracije        : sljedeci_id, povijest_pretrage
 *  - Imenovanje identifikatora : snake_case funkcije, PascalCase typedef, UPPER_CASE makroi
 *  - Deklaracije svih funkcija : vidljive svim .c datotekama
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

 /* ═══════════════════════════════════════════════════════════════
  *  KONSTANTE
  * ═══════════════════════════════════════════════════════════════ */
#define MAX_NAZIV    128
#define MAX_AUTOR    64
#define DATOTEKA     "knjiznica.bin"
#define BACKUP_DAT   "knjiznica_backup.bin"

  /* ═══════════════════════════════════════════════════════════════
   *  MAKRO FUNKCIJE – zastita pokazivaca
   * ═══════════════════════════════════════════════════════════════ */
#define PROVJERI_NULL(ptr, poruka)                  \
    do {                                            \
        if ((ptr) == NULL) {                        \
            fprintf(stderr, "[GRESKA] %s\n",        \
                    poruka);                        \
            return;                                 \
        }                                           \
    } while(0)

#define PROVJERI_NULL_RET(ptr, poruka, ret)         \
    do {                                            \
        if ((ptr) == NULL) {                        \
            fprintf(stderr, "[GRESKA] %s\n",        \
                    poruka);                        \
            return (ret);                           \
        }                                           \
    } while(0)

   /* Sigurno oslobadanje memorije – NULL-ira pokazivac nakon free */
#define SLOBODNA_MEMORIJA(ptr)                      \
    do {                                            \
        free(ptr);                                  \
        (ptr) = NULL;                               \
    } while(0)

/* ═══════════════════════════════════════════════════════════════
 *  INLINE POMOCNE FUNKCIJE
 * ═══════════════════════════════════════════════════════════════ */
static inline int  max_int(int a, int b) { return (a > b) ? a : b; }
static inline void ocisti_ekran(void) { system("cls || clear"); }

/* ═══════════════════════════════════════════════════════════════
 *  ENUM – vrsta knjige
 * ═══════════════════════════════════════════════════════════════ */
typedef enum {
    ROMAN = 0,
    ZNANSTVENA_FANTASTIKA,
    DETEKTIVSKI,
    BIOGRAFIJA,
    TEHNICKA_LITERATURA,
    POEZIJA,
    DRAMA,
    OSTALO,
    VRSTA_COUNT
} VrstaKnjige;

/* ═══════════════════════════════════════════════════════════════
 *  STRUKTURA – jedna knjiga (za binarnu datoteku)
 * ═══════════════════════════════════════════════════════════════ */
typedef struct {
    int         id;
    char        naziv[MAX_NAZIV];
    char        autor[MAX_AUTOR];
    VrstaKnjige vrsta;
    int         godina_izdanja;
    int         dostupno;   /* 1 = dostupno, 0 = posudeno */
} Knjiga;

/* ═══════════════════════════════════════════════════════════════
 *  STRUKTURA – cvor jednostruko povezane liste (povijest pretrage)
 * ═══════════════════════════════════════════════════════════════ */
typedef struct CvorPovijesti {
    char                  upit[MAX_NAZIV];
    int                   rezultata;
    struct CvorPovijesti* sljedeci;
} CvorPovijesti;

/* ═══════════════════════════════════════════════════════════════
 *  ENUM – opcije glavnog izbornika
 * ═══════════════════════════════════════════════════════════════ */
typedef enum {
    IZLAZ = 0,
    DODAJ = 1,
    ISPISI_SVE = 2,
    PROVJERI_DOSTUP = 3,
    PRETRAZI = 4,
    SORTIRAJ = 5,
    AZURIRAJ = 6,
    OBRISI = 7,
    POVIJEST = 8
} OpcijaMenua;

/* ═══════════════════════════════════════════════════════════════
 *  EXTERN GLOBALNE VARIJABLE
 * ═══════════════════════════════════════════════════════════════ */
extern int            sljedeci_id;
extern CvorPovijesti* povijest_pretrage;  /* glava liste */

/* ═══════════════════════════════════════════════════════════════
 *  DEKLARACIJE FUNKCIJA
 * ═══════════════════════════════════════════════════════════════ */

 /* Pomocne */
const char* vrsta_u_string(VrstaKnjige v);
void        citaj_redak(char* buf, int vel);
void        ocisti_buffer(void);
void        ispisi_knjigu(const Knjiga* k);
void        ispisi_zaglavlje(void);

/* Datotecni I/O */
int     ucitaj_sve(Knjiga** out);
int     spremi_sve(const Knjiga* knjige, int n);
Knjiga* dodaj_u_niz(Knjiga* niz, int trenutni_n, const Knjiga* nova); /* koristi realloc */

/* Komparatori za qsort / bsearch */
int cmp_naslov(const void* a, const void* b);
int cmp_autor(const void* a, const void* b);
int cmp_vrsta(const void* a, const void* b);

/* CRUID operacije */
void dodaj_knjigu(void);
void ispisi_sve_knjige(void);
void provjeri_dostupnost(void);
void pretrazi_knjige(void);
void azuriraj_knjigu(void);
void obrisi_knjigu(void);
void sortiraj_i_ispisi(void);

/* Povezana lista – povijest pretrage */
void dodaj_povijest(const char* upit, int rezultata);
void ispisi_povijest(void);
void oslobodi_povijest(void);

/* Izbornik */
void ispisi_meni(void);

#endif /* KNJIZNICA_H */