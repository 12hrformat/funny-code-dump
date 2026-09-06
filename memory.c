#include <stdio.h>

int main(void) {
    char *p = "hello";

    while (*p)
        putchar(*p++);

    int x = 1;

    *(volatile int *)&x = 69420;

    printf("\n%d\n", x);

    return *(int *)((char *)0);
}
