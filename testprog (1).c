#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>


#define MAX_BUF 1024

void readfile(int file); 
void writefile(int file);
void seekfile(int file);
bool checkZero(char str[]);
bool checkInputNum(char str[]);


void menu(){
    printf("options?\n");
    printf("r: Read a specified number of bytes\n");
    printf("w: Write data to the file\n"); 
    printf("s: Seek the file by providing offset value and whence value\n");
    printf("ctrl + d to quit\n");
    printf("\n");
    printf("Input: \n");
}

void switchCommand(char input[], int fd){
    if(strlen(input) == 2){
        switch(input[0]){
            case 'r': 
                readfile(fd);
                printf("\n");
                break;
            case 'w': 
                writefile(fd);
                printf("\n");
                break; 
            case 's': 
                seekfile(fd);
                printf("\n");
                break; 
            default: 
                printf("\n");
                printf("Wrong input please choose from the menu\n");
                break;
        }
    }
    else{
        printf("\n");
        printf("Wrong input please choose from the menu\n");
        printf("\n");
    }
    
}

int main(int argc, char **argv){
    //Declarations
    char input[MAX_BUF]; 
    char *checkEOF = NULL; 
    

    //Open file
    int file = open(argv[1], O_RDWR); 
    if(file == -1){
        printf("ERROR: File does not exist or cannot read/write\n");
        return -1; 
    }

    while(true){
        menu(); 
        checkEOF = fgets(input, MAX_BUF, stdin); 
        if(checkEOF == NULL){break;}
        switchCommand(input,file); 
    }  

    //Check whether file closed properly 
    int status = close(file); 
    if(status == 0){
        printf("Closing %s\n",argv[1]); 
    }
    else{
        printf("Error: File cannot be closed\n");
    }
    
    return 0; 
}


void readfile(int file){
    char numBytes[MAX_BUF];
    printf("What is the number of bytes you want to be read? \n");
    fgets(numBytes, MAX_BUF,stdin);
    
    if(atoi(numBytes) == 0){
        printf("Invalid input or 0, please enter a number greater than 0.\n");
        printf("Returning to main menu...\n");
    }
    else if(!checkInputNum(numBytes)){
        return; 
    }
    else{
        char *user_buffer = (char*)malloc(atoi(numBytes)); 
        int readBytes = read(file, user_buffer, atoi(numBytes)); 
        if(readBytes == -1){
            printf("Read failed\n");
        }
        else{
            printf("Data read: %s\n", user_buffer); 
        }
        free(user_buffer);
    }
}

void writefile(int file){
    char Input[MAX_BUF]; 
    printf("What would you like to write?\n");
    fgets(Input, MAX_BUF, stdin);
    int bytes_written = write(file, Input, strlen(Input));//strlen -> leaves null terminator 
    if(bytes_written == -1){
         printf("Write failed\n");
    } 
}


void seekfile(int file){
    bool offsetZero = false;
    bool whenceZero = false; 
    char offset_val[MAX_BUF]; 
    char whence[MAX_BUF]; 
    printf("Enter an offest value: \n");
    fgets(offset_val, MAX_BUF, stdin); 
    printf("Enter a whence value (0,1,2): \n"); 
    fgets(whence, MAX_BUF, stdin); 


    offsetZero = checkZero(offset_val);
    whenceZero = checkZero(whence);
   

    if((atoi(offset_val) == 0 && offsetZero == false) || (atoi(whence) == 0 && whenceZero == false) ){
        printf("Invalid input. Input must be numbers\n"); 
        printf("Returning to main menu...\n");
    }
    else if(!checkInputNum(offset_val) || !checkInputNum(whence)){return;}
    else if(whenceZero == false && atoi(whence) != 1 && atoi(whence) != 2){
        printf("Error: Whence must be 0, 1, or 2\n"); 
        printf("Returning to main menu...\n");
    }
    
    else {
       off_t offset = lseek(file, atoi(offset_val), atoi(whence)); 
        if(offset == -1){
            printf("Seek failed\n"); 
        }
    }
}


bool checkZero(char str[]){
    if(str[0] == '0' && strlen(str) == 2){
        return true; 
    }
    return false; 
}


bool checkInputNum(char str[]){
    bool flag = true; 
    if(str[0] != '-'){
        if(isdigit(str[0]) == 0){
            flag = false;
        }
    }
    for(int i = 1; i < strlen(str) -1 ; ++i){
        if(isdigit(str[i]) == 0){
            printf("Invalid input. Please enter numbers only\n");
            printf("Returning to main menu...\n");
            flag = false;
        }
    }
    return flag; 
}