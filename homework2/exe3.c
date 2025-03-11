#include <stdio.h>
#include <string.h>

void reverseString(char *str) {
    if (str == NULL || *str == '\0') {
        return;
    }
    char *start = str;
    char *end = str + strlen(str) - 1;
    while (start < end) {
        char temp = *start;
        *start++ = *end;
        *end-- = temp;
    }
}

int main() {
    char input[1000];
    printf("Enter a string: ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 1;
    }
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n') {
        input[len - 1] = '\0';
    }
    reverseString(input);
    printf("Reversed string: %s\n", input);
    return 0;
}
