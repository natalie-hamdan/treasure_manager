#include <sys/types.h> 
#include <dirent.h>    
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>



//later try to make a shared file of the functions so u can just have the main here 

//i use the comments to explain to myself what i'm supposed to be doing so i can better understand how everything is working, ignore them pls.

typedef struct {
    char TreasID[30];
    char username[30];
    float lat, lon;
    char clue[100];
    int value;
}treasure;
int create_hunt(char *hunt_id){ //this function creates the 2 files and closes them, main later opens them to write data in then 
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
        close(treasures_fd);
        close(logged_fd);
        return 0; 
}

int add_treasure(int treasures_fd, int logged_fd){
    //operations for adding treasure info from STDIN to the files
        //making a new treasure struct
        treasure new_treasure;
        char text_buffer[256];//a char array to act as a buffer to store the info in before writing it to the file 
        int size;//and int variable to store the length of info that sprintf will return, so we can use it in write() which needs the size of bytes as the last parameter

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
        return 0;

}

int create_symlink(char *hunt_id){

        //symbolic links 
        char name[50]; //buffer for name of the symlink
        char target[50]; //buffer for name of file we will point to 
        sprintf(name, "logged_hunt-%s", hunt_id);//storing formatted text in buffer "name"
        sprintf(target,"%s/logged_hunt.txt", hunt_id); 
        symlink(target,name);
        //target: the real file, name: the link to the real file 
    
        return 0;
}


int list_hunts(void)
{
    DIR *d = opendir(".");
    if (!d) {
        perror("opendir failed");
        return 1;
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
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        // printf("please write commands:\nfor adding treasure:\n ./name_of_executable --add hunt_number_of_hunt\nfor listing hunts:\n./name_of_executable --list ");
            printf(
                "please write commands:\n"
                "  for adding treasure:\n"
                "    %s --add <hunt_id>\n"
                "  for listing hunts:\n"
                "    %s --list\n",
                argv[0], argv[0] ///instead of using ./name_of_executable, argv[0] always has the the name of executable
            );
            return 1;
        }

    if (strcmp(argv[1], "--add") == 0) {
        if (argc < 3) {
            printf("error: missing <hunt_id>\n");
            return 1;
        }
        char *hunt_id = argv[2];


        //create hunt must run before we can add data to 2 files
        if (create_hunt(hunt_id) != 0){
            printf("error creating 2 empty files inside hunt");
            return 1;
        } 

        char tpath[60], lpath[60]; //buffers
        sprintf(tpath, "%s/treasures.bin", hunt_id);
        sprintf(lpath, "%s/logged_hunt.txt", hunt_id);

        //open the already created files 
        int tfd = open(tpath, O_RDWR);
        int lfd = open(lpath, O_RDWR);
        if (tfd < 0 || lfd < 0) { //testing for errors opening files
            printf("error while opening files to write treasure data");
            return 1;
        }
        add_treasure(tfd, lfd);
        close(tfd);
        close(lfd);

        return create_symlink(hunt_id);
    }
    else if (strcmp(argv[1], "--list") == 0) {
        return list_hunts();
    }
    else {
        fprintf(stderr, "Unknown command '%s'\n", argv[1]);
        return 1;
    }
}




