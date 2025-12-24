#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t count = 0;

void signal_SIGINT(int signum) {
    (void)signum;
    write(1, "\a", 1);
    fflush(NULL);
    count++;
}

void signal_SIGQUIT(int signum) { //выход
    (void) signum;
    printf("\nСчётчик SIGINT = %d\n", count);
    exit(0);
}


int main() {
    struct sigaction sint, squit;

    sint.sa_handler = signal_SIGINT;
    sigemptyset(&sint.sa_mask);
    sint.sa_flags = 0;

    squit.sa_handler = signal_SIGQUIT;
    sigemptyset(&squit.sa_mask);
    squit.sa_flags = 0;

    sigaction(SIGINT, &sint, NULL);
    sigaction(SIGQUIT, &squit, NULL);

    while(1);
}