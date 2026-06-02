#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sc_convert.h"

#define MAX_LINE_LENGTH 512

static void trim(char *str) {
    char *end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
}

static void process_line(char *line, int duplicate_flag) {
    trim(line);
    if (strlen(line) == 0 || strchr(line, ':') == NULL) {
        return; 
    }

    char team[4] = {0};
    int i = 0;
    while (i < 3 && line[i] != '\0' && isalpha((unsigned char)line[i])) {
        team[i] = line[i];
        i++;
    }
    team[i] = '\0';

    if (strlen(team) != 3) return;

    int max_stickers = (strcmp(team, "FWC") == 0) ? 19 : 20;
    int missing[21] = {0}; 

    char *numbers_part = strchr(line, ':');
    if (numbers_part == NULL) return;
    numbers_part++; 

    char *token = strtok(numbers_part, ",");
    while (token != NULL) {
        trim(token);
        if (strchr(token, '-') != NULL) {
            int start, end;
            if (sscanf(token, "%d-%d", &start, &end) == 2) {
                for (int s = start; s <= end; s++) {
                    if (s <= max_stickers) missing[s] = 1;
                }
            }
        } else {
            int num = atoi(token);
            if (num <= max_stickers) missing[num] = 1;
        }
        token = strtok(NULL, ",");
    }

    for (int s = 1; s <= max_stickers; s++) {
        if (!duplicate_flag) {
            if (!missing[s]) {
                printf("./stracker -d example.dat add %s%02d\n", team, s);
            }
        } else {
            if (missing[s]) {
                printf("./stracker -d example.dat add %s%02d\n", team, s);
            }
        }
    }
}

int convert_stickers(const char *filename, int duplicate_flag) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL) {
        process_line(line, duplicate_flag);
    }

    fclose(file);
    return 0;
}
