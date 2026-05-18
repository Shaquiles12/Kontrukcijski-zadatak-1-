#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "knjiznica.h"

/* io.c - Datotecni I/O: ucitaj_sve, spremi_sve */

int sljedeci_id = 1;

int ucitaj_sve(Knjiga** out) {
    PROVJERI_NULL_RET(out, "ucitaj_sve: out je NULL", -1);

    FILE* fp = fopen(DATOTEKA, "rb");
    if (fp == NULL) {
        if (errno == ENOENT) { *out = NULL; return 0; }
        fprintf(stderr, "[GRESKA] fopen: %s\n", strerror(errno));
        return -1;
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        perror("fseek");
        fclose(fp);
        return -1;
    }
    long vel = ftell(fp);
    if (vel < 0) { perror("ftell"); fclose(fp); return -1; }
    rewind(fp);

    int n = (int)(vel / (long)sizeof(Knjiga));
    if (n == 0) { *out = NULL; fclose(fp); return 0; }

    *out = calloc((size_t)n, sizeof(Knjiga));
    PROVJERI_NULL_RET(*out, "ucitaj_sve: calloc nije uspio", -1);

    int procitano = (int)fread(*out, sizeof(Knjiga), (size_t)n, fp);

    if (ferror(fp)) {
        fprintf(stderr, "[GRESKA] fread: %s\n", strerror(errno));
        SLOBODNA_MEMORIJA(*out);
        fclose(fp);
        return -1;
    }
    if (procitano < n && !feof(fp))
        fprintf(stderr, "[UPOZORENJE] Procitano manje zapisa nego ocekivano.\n");

    fclose(fp);

    for (int i = 0; i < procitano; i++)
        sljedeci_id = max_int(sljedeci_id, (*out)[i].id + 1);

    return procitano;
}

int spremi_sve(const Knjiga* knjige, int n) {
    if (n > 0) {
        PROVJERI_NULL_RET(knjige, "spremi_sve: knjige je NULL", -1);
    }

    remove(BACKUP_DAT);
    if (rename(DATOTEKA, BACKUP_DAT) != 0 && errno != ENOENT)
        fprintf(stderr, "[UPOZORENJE] rename: %s\n", strerror(errno));

    FILE* fp = fopen(DATOTEKA, "wb");
    if (fp == NULL) {
        fprintf(stderr, "[GRESKA] fopen (spremi_sve): %s\n", strerror(errno));
        rename(BACKUP_DAT, DATOTEKA);
        return -1;
    }

    if (n > 0) {
        size_t zapisano = fwrite(knjige, sizeof(Knjiga), (size_t)n, fp);
        if ((int)zapisano != n) {
            fprintf(stderr, "[GRESKA] fwrite: %s\n", strerror(errno));
            fclose(fp);
            rename(BACKUP_DAT, DATOTEKA);
            return -1;
        }
    }
    fclose(fp);
    return 0;
}