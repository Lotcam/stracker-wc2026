#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sticker.h"
#include "storage.h"

void album_interactive(Sticker stickers[], int count, char team_code[], char message[]) {

    char input[32];

    while (1) {

        // clear screen
        // system("clear");
        printf("\033[2J\033[H");

        album_page(stickers, team_code, message);

        printf("\n%s\n", message);
        strcpy(message, "");
        printf("\nCommands:\n");
        printf("CODE   -> add sticker\n");
        printf("-CODE  -> remove sticker\n");
        printf("q      -> quit\n\n");

        printf("> ");

        fgets(input, sizeof(input), stdin);

        // remove newline
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "q") == 0) {
            break;
        }
        else if (input[0] == '-') {

            char number[8];
            char full_code[16];

            sscanf(input + 1, "%s", number);

            sprintf(full_code, "%s%02d", team_code, atoi(number));

            sticker_remove(stickers, full_code, message);
        }
        else {
            char number[8];
            char full_code[16];

            sscanf(input, "%s", number);

            sprintf(full_code, "%s%02d", team_code, atoi(number));

            sticker_add(stickers, full_code, message);
        }

        save_db("storage.dat", stickers, count);
    }
}

int main(int argc, char *argv[]) {
    int count = MAX_STICKERS;
    Sticker *stickers = load_db("storage.dat", &count);
    char message[256];
    if (!stickers) {
        save_db("storage.dat", CATALOG, count);
        stickers = load_db("storage.dat", &count);
    }
    if (argc < 2 || strcmp(argv[1], "help") == 0) {
        printf("\nStracker WC2026 - Sticker Album CLI\n");
        printf("Usage: ./stracker <command> [options]\n\n");
        printf("Commands:\n");
        printf("  help                Show this help message\n");
        printf("  list [status] [...] List stickers by status (missing/m, have/h, duplicate/d) and optional teams\n");
        printf("  add <CODE>          Add a sticker by code (e.g., MEX01)\n");
        printf("  remove <CODE>       Remove a sticker by code\n");
        printf("  album <TEAM_CODE>   Interactive album view for a team\n");
        printf("\nExamples:\n");
        printf("  ./stracker list missing mex arg\n");
        printf("  ./stracker add MEX01\n");
        printf("  ./stracker album MEX\n");
        return 0;
    }
    else {
        if (strcmp(argv[1], "list") == 0) {
            if (argc < 3) {
                printf("Please specify a status (missing/m, have/h, duplicate/d).\n");
                printf("Example: ./stracker list h\n");
                return 0;
            }
            sticker_list(stickers, argc, argv, message);
        }
        else if (strcmp(argv[1], "add") == 0) {
            if (argc < 3) return 0;
            sticker_add(stickers, argv[2], message);
            save_db("storage.dat", stickers, count);
        }
        else if (strcmp(argv[1], "remove") == 0) {
            if (argc < 3) return 0;
            sticker_remove(stickers, argv[2], message);
            save_db("storage.dat", stickers, count);
        }
        else if (strcmp(argv[1], "album") == 0) {
            if (argc < 3) {
                printf("Please specify a team code.\n");
                printf("Example: ./stracker album MEX\n");
                return 0;
            }
            album_interactive(stickers, count, argv[2], message);
        }
        else {
            printf("Unknown command: %s\n\n", argv[1]);
            printf("Use './stracker help' to see available commands.\n");
        }
    }

    return 0;
}
