#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

pid_t monitor_pid = 0;
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
    if (kill(monitor_pid, SIGTERM) !=0){
        perror("kill failed");
        return 1;
    }
    printf("terminated process with pid: %d", monitor_pid);

    int status;// variable to hold status info
    if (waitpid(monitor_pid, &status, 0) < 0) { //if it return -1 then it failed
        perror("waitpid failed");
    } else {
        printf("\nMonitor exited with status %d\n", WEXITSTATUS(status)); //else it went fine
    }
    monitor_pid = 0; //restted to zero after child is gone
    return 0;
}

int main() {
    char cmd[32];

    printf("\ntreasure_hub ready. \nCommands: \nstart_monitor, stop_monitor, exit\n");
    while (1) {
        printf("> ");
        if (!fgets(cmd, sizeof(cmd), stdin))
            break;            // EOF

        cmd[strcspn(cmd, "\n")] = '\0';
        if (strcmp(cmd, "start_monitor") == 0) {
            start_monitor();
        }
        else if (strcmp(cmd, "stop_monitor") == 0) {
            stop_monitor();
        }
        else if (strcmp(cmd, "exit") == 0) {
            break;
        }
        else {
            printf("Unknown command: %s\n", cmd);
        }
    }

    // ensure we clean up
    stop_monitor();
    printf("treasure_hub exiting.\n");
    return 0;
}