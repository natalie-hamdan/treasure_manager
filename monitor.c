#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

//will later creat a file with shared functions in order to not copy everything here 

void list_hunts_handler(int sig)
{
    DIR *d = opendir(".");
    if (!d) {
        perror("opendir failed");
    }

    struct dirent *ent;
    int count = 0;

    printf("Hunts:\n");

    ent = readdir(d);
    while (ent != NULL) {
        // 1) is it a directory?
        if (ent->d_type == DT_DIR) {
            // 2) does its name start with "hunt"?
            if (strncmp(ent->d_name, "hunt", 4) == 0) {
                printf("  %s\n", ent->d_name);
                count++;
            }
        }
        ent = readdir(d);
        
    }

    closedir(d);

    printf("Total hunts: %d\n", count);

}
int main(void) {
    
    struct sigaction sa;                   
    memset(&sa, 0, sizeof(sa));            
    sa.sa_handler = list_hunts_handler;    
    sigemptyset(&sa.sa_mask);              
    sa.sa_flags = SA_RESTART;               

    if (sigaction(SIGUSR1, &sa, NULL) < 0) {
        perror("sigaction");
        return 1;
    }

    printf("Monitor running (pid %d)\n", getpid());

    while (1) {
        pause();
    }

    return 0;
}
