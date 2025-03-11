#include <stdio.h>

void compressString(char* s) {
    int i = 0, w = 0;
    while (s[i]) {
        char c = s[i];
        int count = 0;
        while (s[i] == c) {
            count++;
            i++;
        }
        s[w++] = c;
        int len = sprintf(s + w, "%d", count);
        w += len;
    }
    s[w] = '\0';
}

int main() {
    char str[] = "aaabbc";
    compressString(str);
    printf("%s\n", str);
    return 0;
}
