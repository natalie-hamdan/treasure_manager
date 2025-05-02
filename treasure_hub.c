#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>



pid_t monitor_pid = 0;

void sigchld_handler(int sig) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (pid == monitor_pid) {
            monitor_pid = 0;
        }
    }
    
}

int start_monitor(){
    if (monitor_pid) {
        printf("Monitor already running (pid %d)\n", monitor_pid);
        return 1;
    }
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return 1;
    }
    if (pid == 0) {
        execl("./monitor", "monitor", (char*)NULL);
        perror("execl failed");
        _exit(1);
    }
    monitor_pid = pid;
    printf("Started monitor (pid %d)\n", monitor_pid);
    return 0;
    
}

int stop_monitor(){ 

    if (monitor_pid == 0){
        return 0;
    }
    if (kill(monitor_pid, SIGTERM) !=0){
        perror("kill failed");
        return 1;
    }
    printf("\nterminated process with pid: %d", monitor_pid);


    int status;// variable to hold status info
    printf("\nMonitor exited with status %d\n", WEXITSTATUS(status)); //else it went fine


    monitor_pid = 0; //restted to zero after child is gone
    return 0;
}

int main() {

    struct sigaction sa;
    memset(&sa, 0, sizeof sa);
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) < 0) {
        perror("sigaction");
        return 1;
    }
    char cmd[32];
    printf(
        "\ntreasure_hub ready.\n"
        "Commands:\n"
        "  start_monitor   – launch the monitor\n"
        "  stop_monitor    – terminate the monitor\n" //already being handeled by handler
        "  list            – list hunts via the monitor\n"
        "  exit            – quit\n"
    );

    while (1) {
        printf("> ");
        if (!fgets(cmd, sizeof cmd, stdin)) break;
        cmd[strcspn(cmd, "\n")] = '\0';

        if (strcmp(cmd, "start_monitor") == 0) {
            start_monitor();
        }
        else if (strcmp(cmd, "stop_monitor") == 0) {
            stop_monitor();
        }
        else if (strcmp(cmd, "list") == 0) {
            if (monitor_pid == 0) {
                printf("Monitor is not running; start it first.\n");
            } else {
                if (kill(monitor_pid, SIGUSR1) < 0)
                    perror("failed to signal monitor");
            }
        }
        else if (strcmp(cmd, "exit") == 0) {
            break;
        }
        else {
            printf("Unknown command: %s\n", cmd);
        }
    }

    printf("treasure_hub exiting.\n");
    return 0;

}