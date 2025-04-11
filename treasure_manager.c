#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>


typedef struct {
    char TreasID;
    char username;
    float gpsCoo;
    char clue;
    int value;
}treasure;

int main (int argc, char *argv[]){
    if (argc < 3){
        printf("error: missing arguments\n");
        return 1;
    }
    if (strcmp(argv[1], "--add") ==0 ){
        char *hunt_id = argv[2];
        char treasures_path[50];
        char logged_hunt_path[50];
        int treasures_fd,logged_fd;

        if (mkdir(hunt_id, 0755) == -1){
            if (errno == EEXIST){
                printf("directory '%s' already exists", hunt_id);
            }
            else {
                perror("mkdir failed");
                return 1;
            }
        }
        else {
            printf("Directory '%s' created successfully\n", hunt_id);
        }

        sprintf(treasures_path, "%s/treasures.bin", hunt_id);
        sprintf(logged_hunt_path, "%s/logged_hunt.txt", hunt_id);

        treasures_fd = open(treasures_path,O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if (treasures_fd == -1){
            printf("failed to create treasures file\n");
            return 1;
        }

        logged_fd = open (logged_hunt_path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
        if (logged_fd == -1){
            printf("failed to create logged_hunt file\n");
            return 1;
        }
    
        close(treasures_fd);
        close(logged_fd);
    }
    

    return 0;

}
