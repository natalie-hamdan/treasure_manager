#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>


typedef struct {
    char TreasID[30];
    char username[30];
    float lat, lon;
    char clue[100];
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
        //add hunt operation
        if (mkdir(hunt_id, 0755) == -1){
            if (errno == EEXIST){
                printf("directory '%s' already exists", hunt_id);
                return 1;
            }
            else {
                perror("mkdir failed");
                return 1;
            }
        }
        else {
            printf("Directory '%s' created successfully\n", hunt_id);
        }
        //adding 2 empty files inside hunts 
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
        //operations for adding treasure info from STDIN to the files
        //making a new treasure struct
        treasure new_treasure;
        //read data from stdin
        printf("Add treasure ID:\n");
        scanf("%s", &new_treasure.TreasID[30]);
        printf("Add your username:\n");
        scanf("%s", &new_treasure.username[30]);
        printf("Add GPS cooridnates (in this format 00.00):\n");
        printf("Latitude: "); 
        scanf("%f", &new_treasure.lat);
        printf("Longitude: "); 
        scanf("%f", &new_treasure.lon);
        printf("Add your clue:\n ");
        scanf("%s", &new_treasure.clue[100]); // problem with clue: scanf encounters empty line and stops 
        printf("Add value(number): \n");
        scanf("%d",&new_treasure.value);
        
        //use seek_end to go to end of the file 
        off_t go_to_end = lseek(treasures_fd,0, SEEK_END); //file descriptor of file u want to open, at what byte to position file pointer 0 for end of file, seek end with 0 positions it at end of file 
        //the actual writing in the file and storng the size 
        ssize_t bytes_written = write(treasures_fd, &new_treasure, sizeof(treasure) );
        //a check if write operation worked by comparing sizes
        if (bytes_written != sizeof(treasure)){
            printf("Error writing in file\n");
            return 1;
        }

    
        close(treasures_fd);
        close(logged_fd);

        
    }
    

    return 0;

}
