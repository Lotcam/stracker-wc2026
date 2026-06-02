#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sticker.h"
#include "storage.h"
#include "sc_convert.h"

#define TEAM_COUNT_ORDER 49

static const char *TEAM_ORDER[] = {
    "FWC", "MEX", "RSA", "KOR", "CZE", "CAN", "BIH", "QAT", "SUI",
    "BRA", "MAR", "HAI", "SCO", "USA", "PAR", "AUS", "TUR", "GER",
    "CUW", "CIV", "ECU", "NED", "JPN", "SWE", "TUN", "BEL", "EGY",
    "IRN", "NZL", "ESP", "CPV", "KSA", "URU", "FRA", "SEN", "IRQ",
    "NOR", "ARG", "ALG", "AUT", "JOR", "POR", "COD", "UZB", "COL",
    "ENG", "CRO", "GHA", "PAN"
};

static int find_team_index(const char *team_code) {
    for (int i = 0; i < TEAM_COUNT_ORDER; i++)
        if (strcmp(TEAM_ORDER[i], team_code) == 0) return i;
    return 0;
}
void album_interactive(Sticker stickers[], int count, char team_code[], char message[], const char *data_file) {

    char input[32];
    int idx = find_team_index(team_code);

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
        printf("n      -> next page\n");
        printf("p      -> previous page\n");
        printf("q      -> quit\n\n");

        printf("> ");

        fgets(input, sizeof(input), stdin);

        // remove newline
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "q") == 0) {
            break;
        }
        else if (strcmp(input, "n") == 0) {
            idx = (idx + 1) % TEAM_COUNT_ORDER;
            strcpy(team_code, TEAM_ORDER[idx]);
        }
        else if (strcmp(input, "p") == 0) {
            idx = (idx - 1 + TEAM_COUNT_ORDER) % TEAM_COUNT_ORDER;
            strcpy(team_code, TEAM_ORDER[idx]);
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

        save_db(data_file, stickers, count);
    }
}

int main(int argc, char *argv[]) {
    const char *data_file = "storage.dat";

    // parse -d/--data before the command
    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--data") == 0) {
            data_file = argv[i + 1];
            // remove the two args from argv
            for (int j = i; j < argc - 2; j++) argv[j] = argv[j + 2];
            argc -= 2;
            break;
        }
    }

    int count = MAX_STICKERS;
    Sticker *stickers = load_db(data_file, &count);
    char message[256];
    if (!stickers) {
        save_db(data_file, CATALOG, count);
        stickers = load_db(data_file, &count);
    }
    if (argc < 2 || strcmp(argv[1], "help") == 0) {
        printf("\nStracker WC2026 - Sticker Album CLI\n");
        printf("Usage: stracker <command> [args]\n\n");
        printf("Commands:\n");
        printf("  list <missing|have|duplicate>  [TEAM...] [--oneline]\n");
        printf("  add <CODE>\n");
        printf("  remove <CODE>\n");
        printf("  album <TEAM>\n");
        printf("  compare <your_db> <their_db>\n");
        printf("  help\n\n");
        printf("Examples:\n");
        printf("  ./stracker list missing\n");
        printf("  ./stracker list d mex arg --oneline\n");
        printf("  ./stracker add MEX01\n");
        printf("  ./stracker album MEX\n");
        printf("  ./stracker compare my.dat friend.dat\n");
        return 0;
    }
    else {
        if (strcmp(argv[1], "list") == 0) {
            if (argc < 3) {
                fprintf(stderr, "error: missing status argument\n");
                fprintf(stderr, "usage: stracker list <missing|have|duplicate> [TEAM...] [--oneline]\n");
                printf("Example: ./stracker list h\n");
                return 1;
            }
            sticker_list(stickers, argc, argv, message);
        }
        else if (strcmp(argv[1], "add") == 0) {
            if (argc < 3) {
                fprintf(stderr, "error: missing sticker code\n");
                fprintf(stderr, "usage: stracker add <CODE>\n");
                return 1;
            }
            sticker_add(stickers, argv[2], message);
            save_db(data_file, stickers, count);
        }
        else if (strcmp(argv[1], "remove") == 0) {
            if (argc < 3) {
                fprintf(stderr, "error: missing sticker code\n");
                fprintf(stderr, "usage: stracker remove <CODE>\n");
                return 1;
            }
            int a = sticker_remove(stickers, argv[2], message);
            if (a != 0) {
                fprintf(stderr, "error: missing sticker %s, can't remove\n", argv[2]);
                return 1;
            }
            save_db(data_file, stickers, count);
        }
        else if (strcmp(argv[1], "album") == 0) {
            if (argc < 3) {
                fprintf(stderr, "error: missing team code\n");
                fprintf(stderr, "usage: stracker album <TEAM>\n");
                fprintf(stderr, "Example: ./stracker album MEX\n");
                return 1;
            }
            album_interactive(stickers, count, argv[2], message, data_file);
        }
        else if (strcmp(argv[1], "compare") == 0) {
            if (argc < 4) {
                fprintf(stderr, "error: missing file arguments\n");
                fprintf(stderr, "usage: stracker compare <your_storage> <their_storage>\n");
                return 1;
            }
            int count_a = MAX_STICKERS, count_b = MAX_STICKERS;
            Sticker *a = load_db(argv[2], &count_a);
            Sticker *b = load_db(argv[3], &count_b);
            if (!a || !b) {
                fprintf(stderr, "error: could not load one or both storage files\n");
                return 1;
            }

            // extract names without extension
            char name_a[64], name_b[64];
            strncpy(name_a, argv[2], sizeof(name_a) - 1); name_a[sizeof(name_a)-1] = 0;
            strncpy(name_b, argv[3], sizeof(name_b) - 1); name_b[sizeof(name_b)-1] = 0;
            char *dot_a = strrchr(name_a, '.'); if (dot_a) *dot_a = 0;
            char *dot_b = strrchr(name_b, '.'); if (dot_b) *dot_b = 0;
            for (int i = 0; name_a[i]; i++) name_a[i] = toupper((unsigned char)name_a[i]);
            for (int i = 0; name_b[i]; i++) name_b[i] = toupper((unsigned char)name_b[i]);

            printf("Stickers \"%s\" has that \"%s\" needs (\"%s\" can give \"%s\"):\n", name_b, name_a, name_b, name_a);
            int found = 0, found_b = 0;
            for (int i = 0; i < MAX_STICKERS; i++) {
                if (a[i].status == MISSING && b[i].status == DUPLICATE) {
                    printf("  %s - %s\n", a[i].code, a[i].name);
                    found++;
                }
            }
            if (!found) printf("  (none)\n");

            printf("\nStickers \"%s\" has extra that \"%s\" needs (\"%s\" can give \"%s\"):\n", name_a, name_b, name_a, name_b);
            for (int i = 0; i < MAX_STICKERS; i++) {
                if (a[i].status == DUPLICATE && b[i].status == MISSING) {
                    printf("  %s - %s\n", a[i].code, a[i].name);
                    found_b++;
                }
            }
            if (!found_b) printf("  (none)\n");

            printf("\nTotal tradeable: %d (\"%s\" gives \"%s\": %d, \"%s\" gives \"%s\": %d)\n",
                found + found_b, name_b, name_a, found, name_a, name_b, found_b);

            int a_missing = 0, b_missing = 0, a_have = 0, b_have = 0;
            for (int i = 0; i < MAX_STICKERS; i++) {
                if (a[i].status == MISSING) a_missing++; else a_have++;
                if (b[i].status == MISSING) b_missing++; else b_have++;
            }
            printf("\nAfter trading:\n");
            printf("  \"%s\": %d missing (%.1f%% complete)\n",
                name_a, a_missing - found, (a_have + found) * 100.0 / MAX_STICKERS);
            printf("  \"%s\": %d missing (%.1f%% complete)\n",
                name_b, b_missing - found_b, (b_have + found_b) * 100.0 / MAX_STICKERS);
            free(b);
        }
        else if (strcmp(argv[1], "trade") == 0) {
            if (argc < 4) {
                fprintf(stderr, "error: missing file arguments\n");
                fprintf(stderr, "usage: stracker trade <sticker_give> <sticker_receive>\n");
                return 1;
            }
            int a = sticker_remove(stickers, argv[2], message);
            if (a != 0) {
                fprintf(stderr, "error: missing sticker %s\n", argv[2]);
                return 1;
            }
            sticker_add(stickers, argv[3], message);
            printf("\nSuccessful trade\n%s ------> %s\n", argv[2], argv[3]);
            save_db(data_file, stickers, count);
        }
        else if (strcmp(argv[1], "convert") == 0) {
            if (argc < 4) {
                return 1;
                //TODO: add error
            }

            int dup_flag = 1;
            if (strcmp(argv[2], "m") == 0 || strcmp(argv[2], "missing") == 0) {dup_flag = 0;}

            printf("Starting sticker list conversion for file %s\n", argv[3]);

            int result = convert_stickers(argv[3], dup_flag);

            if (result != 0) {
                fprintf(stderr, "error: failed to process file %s.\n", argv[3]);
                return 1;
            }
        }
        else {
            fprintf(stderr, "error: unknown command '%s'\n", argv[1]);
            fprintf(stderr, "run 'stracker help' for usage\n");
            return 1;
        }
    }

    return 0;
}
