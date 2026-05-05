#ifndef STICKER_H
#define STICKER_H

#define MAX_STICKERS 980

typedef enum {
	MISSING = 0,
	HAVE = 1,
	DUPLICATE = 2
} Status;

typedef struct {
	Status status;
	int quantity;
	char code[6];
	char team_code[3];
	char *name;
} Sticker;

Sticker *sticker_find(Sticker stickers[], int count, char code[]);
void sticker_add(Sticker stickers[], int *count, int id);
void sticker_list(Sticker stickers[], int count);
void sticker_missing(Sticker stickers[], int count);
void sticker_duplicates(Sticker stickers[], int count);

extern const Sticker CATALOG[MAX_STICKERS];

#endif
