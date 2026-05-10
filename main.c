#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sticker.h"
#include "storage.h"


int main(int argc, char *argv[]) {
    int count = 980;
    Sticker *sticker = load_db("test.dat", &count);
    sticker_add(sticker, &count, "PNI01");
    printf("quantity: %d\n", sticker[0].quantity);
    save_db("test.dat", sticker, count);


    return 0;
}
