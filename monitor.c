#include <stdio.h>
#include <unistd.h>


int main(void) {
    printf("Monitor created (pid %d)\n", getpid());

    return 0;
}
