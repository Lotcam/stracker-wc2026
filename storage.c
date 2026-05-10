#include <stdio.h>
#include <stdlib.h>
#include "storage.h"
#include "sticker.h"

void save_db(const char *filename, Sticker *db, int count) {
    FILE *f = fopen(filename, "wb");
    if (!f) return;

    fwrite(&count, sizeof(int), 1, f);
    fwrite(db, sizeof(Sticker), count, f);

    fclose(f);
}

Sticker *load_db(const char *filename, int *count) {
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;

    fread(count, sizeof(int), 1, f);

    Sticker *db = malloc(sizeof(Sticker) * (*count));

    fread(db, sizeof(Sticker), *count, f);

    fclose(f);
    return db;
}

