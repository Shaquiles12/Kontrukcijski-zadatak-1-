#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include "knjiznica.h"

/*
 * main.c — Ulazna tocka programa "Skladiste knjiznice"
 *
 * Koncepti u ovoj datoteci:
 *  - switch na typedef enum    : switch((OpcijaMenua)odabir) — glavni izbornik
 *  - Inicijalno ucitavanje     : postavlja sljedeci_id na max(id) + 1 pri pokretanju
 *  - SLOBODNA_MEMORIJA makro   : sigurno oslobadanje privremenog pokazivaca pri init
 *  - oslobodi_povijest()       : ciscenje cijele povezane liste pri izlasku programa
 *  - do-while petlja           : glavni loop programa sve dok korisnik ne odabere izlaz
 */

int main(void) {
    /* Inicijalno ucitavanje – postavlja sljedeci_id na max(id)+1 */
    {
        Knjiga* tmp = NULL;
        int n = ucitaj_sve(&tmp);
        (void)n;
        SLOBODNA_MEMORIJA(tmp);
    }

    int odabir;
    do {
        ispisi_meni();

        if (scanf("%d", &odabir) != 1) {
            ocisti_buffer();
            odabir = -1;
        }
        else {
            ocisti_buffer();
        }

        switch ((OpcijaMenua)odabir) {
        case DODAJ:            dodaj_knjigu();        break;
        case ISPISI_SVE:       ispisi_sve_knjige();   break;
        case PROVJERI_DOSTUP:  provjeri_dostupnost(); break;
        case PRETRAZI:         pretrazi_knjige();     break;
        case SORTIRAJ:         sortiraj_i_ispisi();   break;
        case AZURIRAJ:         azuriraj_knjigu();     break;
        case OBRISI:           obrisi_knjigu();       break;
        case POVIJEST:         ispisi_povijest();     break;
        case IZLAZ:            printf("\nDovidenja!\n"); break;
        default:               printf("[GRESKA] Neispravan odabir.\n"); break;
        }
    } while (odabir != IZLAZ);

    /* Ciscenje svih dinamicki alociranih resursa */
    oslobodi_povijest();

    return EXIT_SUCCESS;
}