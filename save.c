#include "save.h"
#include <stdio.h>

int sauvegarder_partie(const SaveData *data) {
    FILE *f = fopen("save.bin", "wb");
    if (!f) return 0;
    fwrite(data, sizeof(SaveData), 1, f);
    fclose(f);
    return 1;
}

int charger_partie(SaveData *data) {
    FILE *f = fopen("save.bin", "rb");
    if (!f) return 0;
    fread(data, sizeof(SaveData), 1, f);
    fclose(f);
    return 1;
}

void supprimer_sauvegarde(void) {
    remove("save.bin");
}