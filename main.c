#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sticker.h"
#include "storage.h"


int main(int argc, char *argv[]) {
    int count = MAX_STICKERS;
    Sticker *stickers = load_db("storage.dat", &count);
    if (!stickers) {
        save_db("storage.dat", CATALOG, count);
        stickers = load_db("storage.dat", &count);
    }
    if (argc < 2) {
        return 0;
    }
    else {
        if (strcmp(argv[1], "list") == 0) {
            sticker_list(stickers, &count, argc, argv);
        }
        else if (strcmp(argv[1], "add") == 0) {
            if (argc < 3) return 0;
            sticker_add(stickers, &count, argv[2]);
            save_db("storage.dat", stickers, count);
        }
        else if (strcmp(argv[1], "remove") == 0) {
            if (argc < 3) return 0;
            sticker_remove(stickers, &count, argv[2]);
            save_db("storage.dat", stickers, count);
        }
    }

    return 0;
}
