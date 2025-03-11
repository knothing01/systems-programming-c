#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 1024

void promptUser(int *col, char *value) {
    printf("Enter column number: ");
    scanf("%d", col);
    printf("Enter search value: ");
    scanf("%s", value);
}

void searchCSV(const char *filename, int col, const char *value) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Unable to open file.\n");
        exit(1);
    }
    char line[MAX_LINE];
    fgets(line, MAX_LINE, fp); // read header
    while (fgets(line, MAX_LINE, fp)) {
        char copy[MAX_LINE];
        strcpy(copy, line);
        int current = 0;
        char *token = strtok(copy, ",");
        while (token != NULL) {
            if (current == col - 1) {
                if (strcmp(token, value) == 0) {
                    printf("%s", line);
                }
                break;
            }
            current++;
            token = strtok(NULL, ",");
        }
    }
    fclose(fp);
}

int main() {
    int col;
    char searchValue[100];
    promptUser(&col, searchValue);
    searchCSV("example.csv", col, searchValue);
    return 0;
}
