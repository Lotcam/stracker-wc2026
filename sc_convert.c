#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sc_convert.h"

#define MAX_LINE_LENGTH 512

#define NUM_TEAMS 49

static const char *ALL_TEAMS[] = {
    "FWC", "MEX", "RSA", "KOR", "CZE", "CAN", "BIH", "QAT", "SUI",
    "BRA", "MAR", "HAI", "SCO", "USA", "PAR", "AUS", "TUR", "GER",
    "CUW", "CIV", "ECU", "NED", "JPN", "SWE", "TUN", "BEL", "EGY",
    "IRN", "NZL", "ESP", "CPV", "KSA", "URU", "FRA", "SEN", "IRQ",
    "NOR", "ARG", "ALG", "AUT", "JOR", "POR", "COD", "UZB", "COL",
    "ENG", "CRO", "GHA", "PAN"
};

static void trim(char *str) {
    char *end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
}

// Helper to find the index of a team in our master list
static int get_team_index(const char *team) {
    for (int i = 0; i < NUM_TEAMS; i++) {
        if (strcmp(ALL_TEAMS[i], team) == 0) {
            return i;
        }
    }
    return -1; // Not found
}

int convert_stickers(const char *filename, int duplicate_flag) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // 2D Array: tracks missing stickers for [team_index][sticker_number]
    // 0 = owned/not listed, 1 = missing
    int missing_matrix[NUM_TEAMS][21] = {0};

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        trim(line);
        if (strlen(line) == 0 || strchr(line, ':') == NULL) {
            continue; 
        }

        char team[4] = {0};
        int i = 0;
        while (i < 3 && line[i] != '\0' && isalpha((unsigned char)line[i])) {
            team[i] = line[i];
            i++;
        }
        team[i] = '\0';

        if (strlen(team) != 3) continue;

        int team_idx = get_team_index(team);
        if (team_idx == -1) continue; // Ignore unrecognized teams

        int max_stickers = (strcmp(team, "FWC") == 0) ? 19 : 20;

        char *numbers_part = strchr(line, ':');
        if (numbers_part == NULL) continue;
        numbers_part++; 

        // Work on a copy of numbers_part because strtok modifies the string
        char numbers_copy[MAX_LINE_LENGTH];
        strncpy(numbers_copy, numbers_part, sizeof(numbers_copy) - 1);
        numbers_copy[sizeof(numbers_copy) - 1] = '\0';

        char *token = strtok(numbers_copy, ",");
        while (token != NULL) {
            trim(token);
            if (strchr(token, '-') != NULL) {
                int start, end;
                if (sscanf(token, "%d-%d", &start, &end) == 2) {
                    for (int s = start; s <= end; s++) {
                        if (s <= max_stickers) missing_matrix[team_idx][s] = 1;
                    }
                }
            } else {
                int num = atoi(token);
                if (num <= max_stickers) missing_matrix[team_idx][num] = 1;
            }
            token = strtok(NULL, ",");
        }
    }
    fclose(file);

    // --- Output Generation ---
    // Now we loop through ALL teams in the album, ensuring complete squads are processed
    for (int t = 0; t < NUM_TEAMS; t++) {
        const char *team = ALL_TEAMS[t];
        int max_stickers = (strcmp(team, "FWC") == 0) ? 19 : 20;

        for (int s = 1; s <= max_stickers; s++) {
            int is_missing = missing_matrix[t][s];

            if (!duplicate_flag) {
                // If it's NOT missing, we want to add it to stracker
                if (!is_missing) {
                    printf("./stracker -d example.dat add %s%02d\n", team, s);
                }
            } else {
                // If we want duplicates (which your code maps to missing=1)
                if (is_missing) {
                    printf("./stracker -d example.dat add %s%02d\n", team, s);
                }
            }
        }
    }

    return 0;
}
