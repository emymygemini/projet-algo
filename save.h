//
// Created by frede on 19/05/2026.

#ifndef SAVE_H
#define SAVE_H

#include <stdio.h>

typedef struct {
    int   vies;
    int   score;
    int   niveau;
    float x, y;
} SaveData;

int  sauvegarder_partie(const SaveData *data);
int  charger_partie(SaveData *data);
void supprimer_sauvegarde(void);

#endif // SAVE_H