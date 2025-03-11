#include <stdio.h>
#include <stdlib.h>


int main() {

    FILE* fptr = fopen("file.txt", "r");
    
    char ch = ' ';

    int positionI = 1; // row position
    int positionJ = 1; // col position
 

    if (fptr == NULL) {
        printf("error\n");
        return 0;
    }

    while ((ch = fgetc(fp)) != EOF) {
        positionJ++;
        if (ch == '\n') {
            positionI++;
            col = 0;
        } else if (ch == '{' || ch == '}') {
            printf("'%c' at line %d, col %d\n", ch, positionI, positionJ);
        }
    }

    fclose(fptr);

    return 0;
}
