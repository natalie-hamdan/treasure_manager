#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

//i use the comments to explain to myself what i'm supposed to be doing so i can better understand how everything is working, ignore them pls.

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
        scanf("%s", new_treasure.TreasID); //no "&" cuz its an array 
        printf("Add your username:\n");
        scanf("%s", new_treasure.username);
        printf("Add GPS cooridnates (in this format 00.00):\n");
        printf("Latitude: "); 
        scanf("%f", &new_treasure.lat);
        printf("Longitude: "); 
        scanf("%f", &new_treasure.lon);
        printf("Add your clue:\n "); // scanf stops when it encounters a whitespace, cant use it with clue cuz i will be writing a sentence, use fgets instead 
        getchar(); //used this to remove the leftover newline charecter that is generated by the previous scanf
        fgets(new_treasure.clue, sizeof(new_treasure.clue), stdin);
        // store text in, gets size of the text, take it from standard input
        
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

        //writing the text to the logged file 
        char text_buffer[256];//a char array to act as a buffer to store the info in before writing it to the file 
        int size;//and int variable to store the length of info that sprintf will return, so we can use it in write() which needs the size of bytes as the last parameter

        //treasure ID
        size = sprintf(text_buffer,"ID: %s", new_treasure.TreasID); // stores in the buffer the text,from standard input. also returns the size of the data and stores it in size. 
        write(logged_fd, text_buffer, size);
        //the size variable is overwritten everytime we use sprintf
        
        //username
        size = sprintf(text_buffer,"\nusername: %s", new_treasure.username);
        write(logged_fd, text_buffer, size);

        //GPS coor
        size= sprintf(text_buffer, "\nLat: %f", new_treasure.lat);
        write(logged_fd, text_buffer, size);

        size = sprintf(text_buffer,"\nLon: %f", new_treasure.lon);
        write(logged_fd,text_buffer, size);

        //clue
        size = sprintf(text_buffer, "\nClue: %s", new_treasure.clue);
        write(logged_fd, text_buffer, size);

        //value
        size = sprintf(text_buffer, "\nValue: %d", new_treasure.value);
        write(logged_fd, text_buffer, size);
    
        close(treasures_fd);
        close(logged_fd);

        //symbolic links 
        char name[50]; //buffer for name of the symlink
        char target[50]; //buffer for name of file we will point to 
        sprintf(name, "logged_hunt-%s", hunt_id);//storing formatted text in buffer "name"
        sprintf(target,"%s/logged_hunt.txt", hunt_id); 
        symlink(target,name);
        //target: the real file, name: the link to the real file 

        
    }
    

    return 0;

}
