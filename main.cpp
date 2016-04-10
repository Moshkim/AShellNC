//This is ashell.cpp - project1
//Kwanil Kim
//Qianhui Fan

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <ctype.h>
#include <vector>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PIPE 1
#define OUTPUT 2
#define INPUT 3



void ResetCanonicalMode(int fd, struct termios *savedattributes){
    tcsetattr(fd, TCSANOW, savedattributes);
}

void SetNonCanonicalMode(int fd, struct termios *savedattributes){
    struct termios TermAttributes;
    char *name;
    
    // Make sure stdin is a terminal.
    if(!isatty(fd)){
        fprintf (stderr, "Not a terminal.\n");
        exit(0);
    }
    
    // Save the terminal attributes so we can restore them later.
    tcgetattr(fd, savedattributes);
    
    // Set the funny terminal modes.
    tcgetattr (fd, &TermAttributes);
    TermAttributes.c_lflag &= ~(ICANON | ECHO); // Clear ICANON and ECHO.
    TermAttributes.c_cc[VMIN] = 1;
    TermAttributes.c_cc[VTIME] = 0;
    tcsetattr(fd, TCSAFLUSH, &TermAttributes);
}

void parse(char *commandLine, char *endPtr, vector <char*> &v, vector <int> &modeptr) { /* parse and store the command line and use pointer array so to indicate each first character of commands or files*/

    char *args[30];
    *args = commandLine;
    char* supplementPtr = NULL;

    while(*commandLine != '\0'){  //dl.dropboxusercontent.com/u/14380020/blogger/shell.c
        if(*commandLine == ' ' || *commandLine == '\n' || *commandLine == '\t'){
            *commandLine++ = '\0'; //replace all the space with NULL
            *args++ = commandLine; //after replacement, the next pointer should see the next element of the comment line
        }
        else if(*commandLine == '|'){
            modeptr.pushback(1); //pipe
            *args = '\0';
            v.pushback(args);
            commandLine++;
            while(*commandLine == ' ' || *commandLine == '\t')
                commandLine++;
            *supplementPtr = commandLine;
            endPtr = supplementPtr;
            parse(*supplementPtr, v, modeptr);
            break;
        }
        else if(*commandLine == '>'){
            modeptr.pushback(2); //OUTPUT_redirect
            *args = '\0';
            v.pushback(args);
            commandLine++;
            /*if(*commandLine == '>')  //another redirection?
            {
                *modeptr = OUTPUT_APP;
                commandLine++;
            }*/
            while(*commandLine == ' ' || *commandLine == '\t')
                commandLine++;
            *supplementPtr = commandLine;
            endPtr = supplementPtr;
            break;
        }
        else if(*commandLine == '<'){
            modeptr.pushback(3);
            *args = '\0';]
            v.pushback(args);
            commandLine++;
            while(*commandLine == ' ' || *commandLine == '\t')
                commandLine++;
            *supplementPtr =commandLine;
            endPtr = supplementPtr;
            //chop(*supplementPtr); //do we need?
            //terminate = 1;
            break;
        }
        else{
            commandLine++; // otherwise keep increment commandline
        }

    }//end of while
    //*args = '\0'; // once finish reading the commandLine the last pointer should point to NULL
}//end of parsing

void execute(char **args, char *endPtr, vector <char*> &v, vector <int> &modeptr){ //execute function that handles fork() and pipe()

    int sizeOfArg = v.size(); // number of childs
    pid_t pid;
    int p[2];

    for(int i = 0; i < sizeOfArg - 1; i++){
        pipe(p); // creating all the pipes == child - 1
    }

    if(fork() == 0){ // child process


    }
    else{ //parents process


    }

    if(mode == PIPELINE)
    {

    }

    if(**args[0] == 'cd'){
        cd(**args);
    }
    else if(**args[0] == 'ls'){
        ls(**args);
    }
    else if(**args[0] == 'pwd'){
        curr_dir = getenv("PWD");
    }
    else if(**args[0] == 'ff'){

    }
}

void ls(char **args) {  /*www.johnloomis.org/ece537/notes/Files/Examples/ls2.html */
    DIR *dir_ptr;
    struct dirent *direntptr;
    char dirname[];
    struct stat info;
    char modestr[11];
    
    if(**args[1] == '\0')
        dirname[] = ".";
    else
        dirname[] = args[1];
    
    if((dir_ptr == opendir( dirname ) ) == NULL )
        //write(STDOUT_FILENO, "cannot open")
    else {
        while ( ( direntp = readdir( dir_ptr ) ) != NULL ) {
            if (strcmp(direntp->d_name,".")==0 ||
                strcmp(direntp->d_name,"..")==0) continue;
            
            strcpy(modestr,"----------" );
            if ( S_ISDIR(info->st_mode) )  modestr[0] = 'd';    /* directory?       */
            if ( S_ISCHR(info->st_mode) )  modestr[0] = 'c';    /* char devices     */
            if ( S_ISBLK(info->st_mode) )  modestr[0] = 'b';    /* block device     */
            
            if ( info->st_mode & S_IRUSR ) modestr[1] = 'r';    /* 3 bits for user  */
            if ( info->st_mode & S_IWUSR ) modestr[2] = 'w';
            if ( info->st_mode & S_IXUSR ) modestr[3] = 'x';
            
            if ( info->st_mode & S_IRGRP ) modestr[4] = 'r';    /* 3 bits for group */
            if ( info->st_mode & S_IWGRP ) modestr[5] = 'w';
            if ( info->st_mode & S_IXGRP ) modestr[6] = 'x';
            
            if ( info->st_mode & S_IROTH ) modestr[7] = 'r';    /* 3 bits for other */
            if ( info->st_mode & S_IWOTH ) modestr[8] = 'w';
            if ( info->st_mode & S_IXOTH ) smodetr[9] = 'x';
            
            write(STDOUT_FILENO,modestr,11);
            write(STDOUT_FILENO,direntp->d_name,strlen(direntp->d_name));
        }
        closedir(dir_ptr);
    }
    
}// end of ls


void cd(char **args){ //cd function that takes char array as parameter
    if(**args[1] == '\0'){ //if after cd command there is nothing then do below
        const char* home = getenv("HOME"); //get the env variable that indicate "HOME"
        int flag = chdir(home); //get the full path to the home directory
        if (flag < 0) //case is where there is no such as HOME directory
            write(STDOUT_FILENO,"Error changing directory");
    }
    else{ //otherwise write up the full path to HOME directory
        chdir(**args[1]);
        //we need to implement the case of "cd .." and how they show as directory
        //such as /.../ecs150% and etc...
    }
}


int main(int argc, char *argv[]){
    struct termios SavedTermAttributes;
    char inputChar;
    int history_Index = 0;   // 0-9
    vector<string> history; //
    vector<char *> v;
    vector<int> modeptr;
    char *endPtr = NULL;
    char current_Buff[300]; //whole directory path
    char buff[300]; // command line
    String command;
    char *args[100]; // argument
    char *pch;
    char current_directory[20];//current directory
    int last_occur=0;//last occurance of '/'
    int size_Array=0;
    int mode = 0;

    
    
    SetNonCanonicalMode(STDIN_FILENO, &SavedTermAttributes);
    
    while(1){
        read(STDIN_FILENO, &inputChar, 1);
        
        if(0x04 == inputChar){ // Contro + d --> terminate
            break;
        }
        else if(0x7C == inputChar | 0x7F == inputChar) { // backspace or delete
            if(command.empty()){
                write(STDOUT_FILENO, "\a",1);
            }
            else{
                write(STDOUT_FILENO, "\b \b",3);
            }
        }
        else if(0x20 == inputChar) { //whitespace for up/down arrow(' '+'['+'A' or 'B')
            read(STDIN_FILENO, &inputChar, 1); //reading second input of the arrow
            read(STDIN_FILENO, &inputChar, 1); //read third input
            if(0x41 == inputChar){ //up arrow and look up the history
                if(history.empty()) {
                    write(STDOUT_FILENO, "\a",1); //if the history is empty then ring the bell
                    continue;
                }
                history_Index--; //traverse the vector to look up the history <--
                write(STDOUT_FILENO,history.at(history_Index));
            }
            else { //down arrow and look up the history
                if(history.at(history_Index) == NULL) {
                    write(STDOUT_FILENO, "\a",1); //if the history is empty then ring the bell
                    continue;
                }
                history_Index++;
                write(STDOUT_FILENO,history.at(history_Index));
            }
        }
        else if (0x0A == inputChar) { //enter to show current directory
            if(command.empty()) { //if there is no input in the command line and empty
                getcwd(current_Buff, 300); //get the current dictory path and store into current_Buff
                pch = strrchr(current_Buff, '/'); //we look for the last '/' and save the index of the position
                last_occur = pch-current_Buff+1; //subtract total - last '/' position == length until last '/'
                size_Array = sizeof(current_Buff)/sizeof(*current_Buff); //get the total length of the path
                strncpy(current_directory, current_Buff+last_occur, size_Array-last_occur-1) //copy last directory name
                write(STDOUT_FILENO,current_directory,sizeof(current_directory)); //write the last directory of the path
            }
            else {
                history.pushback(command); //if comman line is not empty
                command.append(1,inputChar); //NULL(?)
                strcpy(buff,command.c_str()); //string copy to store into char array 'buff'
                parse(buff, &endPtr, v, modeptr); //parse the whole command to parse function
                execute(args, &endPtr, v, modeptr); //once the parse is done --> pass the pointer array to execute function
                //parse
            }
        }
        else {
            write(STDOUT_FILENO,inputChar, 1); //keep write whatever the input is getting
            command.append(1,inputChar); //append every char to string command
        }
        // else{
        //     if(isprint(RXChar)){
        //         printf("RX: '%c' 0x%02X\n",RXChar, RXChar);
        //     }
        //     else{
        //         printf("RX: ' ' 0x%02X\n",RXChar);
        //     }
        // }
    }
    
    ResetCanonicalMode(STDIN_FILENO, &SavedTermAttributes); //switch to CanonoicalMode
    return 0;
}
