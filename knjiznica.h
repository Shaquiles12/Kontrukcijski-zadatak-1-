#pragma once
#ifndef KNJIZNICA_H
#define KNJIZNICA_H

#include <stdio.h>
#include <stdlib.h>


#define MAX_NAZIV    128
#define MAX_AUTOR    64
#define DATOTEKA     "knjiznica.bin"
#define BACKUP_DAT   "knjiznica_backup.bin"


#define PROVJERI_NULL(ptr, poruka)              \
    do {                                        \
        if ((ptr) == NULL) {                    \
            fprintf(stderr,                     \
                    "[GREŠKA] %s\n", poruka);   \
            return;                             \
        }                                       \
    } while(0)

#define PROVJERI_NULL_RET(ptr, poruka, ret)     \
    do {                                        \
        if ((ptr) == NULL) {                    \
            fprintf(stderr,                     \
                    "[GREŠKA] %s\n", poruka);   \
            return (ret);                       \
        }                                       \
    } while(0)


static inline int max_int(int a, int b) { return (a > b) ? a : b; }
static inline void ocisti_ekran(void) { system("cls || clear"); }


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


typedef struct {
    int          id;
    char         naziv[MAX_NAZIV];
    char         autor[MAX_AUTOR];
    VrstaKnjige  vrsta;
    int          godina_izdanja;
    int          dostupno;   /* 1 = dostupno, 0 = posudeno */
} Knjiga;


typedef enum {
    IZLAZ = 0,
    DODAJ = 1,
    ISPISI_SVE = 2,
    PROVJERI_DOSTUP = 3,
    PRETRAZI = 4,
    SORTIRAJ = 5,
    AZURIRAJ = 6,
    OBRISI = 7
} OpcijaMenua;


extern int sljedeci_id;

const char* vrsta_u_string(VrstaKnjige v);
void        citaj_redak(char* buf, int vel);
void        ocisti_buffer(void);
void        ispisi_knjigu(const Knjiga* k);
void        ispisi_zaglavlje(void);

int  ucitaj_sve(Knjiga** out);
int  spremi_sve(const Knjiga* knjige, int n);


int cmp_naslov(const void* a, const void* b);
int cmp_autor(const void* a, const void* b);
int cmp_vrsta(const void* a, const void* b);


void dodaj_knjigu(void);
void ispisi_sve_knjige(void);
void provjeri_dostupnost(void);
void pretrazi_knjige(void);
void azuriraj_knjigu(void);
void obrisi_knjigu(void);
void sortiraj_i_ispisi(void);


void ispisi_meni(void);

#endif 