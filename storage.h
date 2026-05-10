#ifndef STORAGE_H
#define STORAGE_H

#include "sticker.h"

void save_db(const char *filename, Sticker *db, int count);

Sticker *load_db(const char *filename, int *count);

#endif
