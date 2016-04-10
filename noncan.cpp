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
#include <string>

#define PIPE 1
#define OUTPUT 2
#define INPUT 3

using namespace std;


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

//void parse(char *commandLine, char *args[], char **supplementPtr, vector <char*> &v, vector <int> &modeptr) {
void parse(char *commandLine, char **args, char **supplementPtr, char ***allargs, vector <int> &modeptr, int childcount) {
 // parse and store the command line and use pointer array so to indicate each first character of commands or files

    char *args2[30];
    *args = commandLine;
    *allargs++ = args;
    args++;
    childcount++;
    char* supplementPtr2 = NULL;

    while(*commandLine != '\0'){  //dl.dropboxusercontent.com/u/14380020/blogger/shell.c
        if(*commandLine == ' ' || *commandLine == '\n' || *commandLine == '\t'){

            //*commandLine = '\0'; //replace all the space with NULL
            if(*commandLine+1 != '|' && *commandLine+1 != '>' && *commandLine+1 != '<' && *commandLine+1 != ' '){
                write(STDOUT_FILENO,commandLine,20);
                *commandLine++ = '\0';
                *args++ = commandLine; //after replacement, the next pointer should see the next element of the
            }
            else{
                *commandLine++ = '\0';
            }
            //comment line
        }
        else if(*commandLine == '|'){

            modeptr.push_back(1); //pipe
            *args = NULL;
            //v.push_back(*args);
            //write(STDOUT_FILENO,"here1",5);

            *commandLine++ = '\0';
            while(*commandLine == ' ' || *commandLine == '\t')
                *commandLine++ = '\0';
            *supplementPtr = commandLine;
            //endPtr = supplementPtr
            parse(*supplementPtr, args2, &supplementPtr2, allargs, modeptr, childcount);
            //write(STDOUT_FILENO,"here0",5);
            break;
        }
        else if(*commandLine == '>'){
            modeptr.push_back(2); //OUTPUT_redirect
            *args = NULL;
            //v.push_back(*args);
            commandLine++;
            /*if(*commandLine == '>')  //another redirection?
             {
             *modeptr = OUTPUT_APP;
             commandLine++;
             }*/
            while(*commandLine == ' ' || *commandLine == '\t')
                commandLine++;
            *supplementPtr = commandLine;
            //endPtr = supplementPtr;
            break;
        }
        else if(*commandLine == '<'){
            modeptr.push_back(3);
            *args = NULL;
            //v.push_back(*args);
            commandLine++;
            while(*commandLine == ' ' || *commandLine == '\t')
                commandLine++;
            *supplementPtr = commandLine;
            //endPtr = supplementPtr;
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


void showPath() {  //may need modify later
    int last_occur=0;//last occurance of '/'
    char *pch;
    int size_Array=0;
    char current_Buff[300];
    char current_directory[20];//current directory

    getcwd(current_Buff, 300); //get the current dictory path and store into current_Buff
    string current_Buff_string(current_Buff);

    pch = strrchr(current_Buff, '/'); //we look for the last '/' and save the index of the position
    last_occur = pch-current_Buff+1; //subtract total - last '/' position == length until last '/'
    size_Array = sizeof(current_Buff)/sizeof(*current_Buff); //get the total length of the path
    strncpy(current_directory, current_Buff+last_occur, size_Array-last_occur-1); //copy last directory name
    if(current_Buff_string.length()>16){
        write(STDOUT_FILENO,"/.../",5);
    }
    write(STDOUT_FILENO,current_directory,sizeof(current_directory)); //write the last directory of the path
    write(STDOUT_FILENO," ",1);
}

string get_commandline(string command, vector<string> &history) {

    char inputChar;
    int history_Index = history.size(); //current index
    string history_cmd = "";
    char buff[300]; // command line
    //string command = "";
    char *args[100]; // argument
    int mode = 0;

    while(1){
        read(STDIN_FILENO, &inputChar, 1);

        if(0x04 == inputChar){ // Contro + d --> terminate
            break;
        }
        else if(0x7F == inputChar) { // backspace or delete
            if(command.empty()){
                write(STDOUT_FILENO, "\a",1);
            }
            else{
                command.pop_back(); //remove last character of string
                write(STDOUT_FILENO, "\b \b",3);
            }
        }
        else if(0x1B == inputChar) { //whitespace for up/down arrow(' '+'['+'A' or 'B')
            read(STDIN_FILENO, &inputChar, 1); //reading second input of the arrow
            read(STDIN_FILENO, &inputChar, 1); //read third input
            if(0x41 == inputChar){ //up arrow and look up the history
                if(history_Index == 0) {
                    write(STDOUT_FILENO, "\a",1); //if the history is empty then ring the bell
                    continue;
                }
                else if(history_Index-1 >= 0){
                    history_Index--; //traverse the vector to look up the history <--
                    history_cmd = history[history_Index];

                    for(int i = 0; i<command.length();i++){
                        write(STDOUT_FILENO,"\b \b", 3);
                    }
                    command.clear();

                    write(STDOUT_FILENO,history_cmd.c_str(),history_cmd.length());
                    command.assign(history_cmd);
                }
            }
            else if(0x42 == inputChar){ //down arrow and look up the history
                if(history_Index+1 > (int)history.size()) { //nothing down
                    write(STDOUT_FILENO, "\a",1);
                    continue;
                }
                    history_Index++;
                    history_cmd = history[history_Index];

                    for(int i = 0; i<command.length();i++){
                        write(STDOUT_FILENO,"\b \b", 3);
                    }
                    command.clear();

                    write(STDOUT_FILENO,history_cmd.c_str(),history_cmd.length());
                    command.assign(history_cmd);
            }
        }
        else if (0x0A == inputChar) { //enter to show current directory
            history_Index = history.size(); //index of current command to be put in history
            if(command.empty()) { //if there is no input in the command line and empty
                write(STDOUT_FILENO,"\n",1);
                showPath();

            }
            else {
                if(history.size() >= 10)
                    history.erase(history.begin());
                //command.append(1,inputChar);
                history.push_back(command); //push to current index
                //write(STDOUT_FILENO,command.c_str(),command.length());
                //command.append(1,inputChar); //end with "\n"
                // strcpy(buff,command.c_str()); //string copy to store into char array 'buff'

                write(STDOUT_FILENO,"\n",1);
                //showPath();
                break;
            }
        }
        else {
            write(STDOUT_FILENO,&inputChar, 1); //keep write whatever the input is getting
            command += inputChar; //append every char to string command
        }
    }//end of while loop to read each command line
    return command;
}


int main(int argc, char *argv[]){
    struct termios SavedTermAttributes;
    vector<string> history; //History
    vector<char*> v;
    vector<int> modeptr;
    char commandline_char[100];
    char *args[30];
    char **allargs[30];
    int childcount = 0;
    char *endPtr = NULL;
    char *supplementPtr = NULL;
    SetNonCanonicalMode(STDIN_FILENO, &SavedTermAttributes);

    while(1){
    showPath();
    string command = "";
    string commandline = get_commandline(command, history);
    strcpy(commandline_char, commandline.c_str());
    //parsing


    //parse(commandline_char, args, &supplementPtr,v,modeptr);
    parse(commandline_char, args, &supplementPtr,allargs,modeptr, childcount);
        write(STDOUT_FILENO,allargs[0],10);
        write(STDOUT_FILENO,"\n",1);
        write(STDOUT_FILENO,&childcount,10);
        write(STDOUT_FILENO,"-------",7);
          write(STDOUT_FILENO,"\n",1);
    write(STDOUT_FILENO,allargs[0][0],5);
        write(STDOUT_FILENO,"\n",1);
    write(STDOUT_FILENO,allargs[0][1],5);
          write(STDOUT_FILENO,"\n",1);
        write(STDOUT_FILENO,allargs[1][0],5);
        write(STDOUT_FILENO,"\n",1);
        write(STDOUT_FILENO,allargs[1][1],5);
        write(STDOUT_FILENO,"\n",1);
        write(STDOUT_FILENO,allargs[2][0],5);
        write(STDOUT_FILENO,"\n",1);
    //char a = *v[0];
    //write(STDOUT_FILENO,args[0],2);

        //write(STDOUT_FILENO,&a,1);
    if(!strcmp(args[0],"ls")) { //equal is 0
        write(STDOUT_FILENO,args[0],2);
        DIR *dir_ptr;
        struct dirent *direntptr;
        char dirname2[20];
        string dirname = "";
        struct stat info;
        char modestr[11];
        char current_Buff[300];
        getcwd(current_Buff, 300); //get the current dictory path and store into current_Buff

        if(args[1] == NULL)
            dirname = current_Buff;
        else
            dirname = args[1];
        write(STDOUT_FILENO,dirname.c_str(),dirname.length());

        if((dir_ptr == opendir(dirname.c_str())) == 0 ) {
            write(STDOUT_FILENO, "cannot open",11);
        }
        else {
            while ( ( direntptr = readdir(dir_ptr)) == NULL ) {
                if (strcmp(direntptr->d_name,".")==0 ||
                    strcmp(direntptr->d_name,"..")==0) continue;

                strcpy(modestr,"----------" );
                if ( S_ISDIR(info.st_mode) )  modestr[0] = 'd';    /* directory?       */
                if ( S_ISCHR(info.st_mode) )  modestr[0] = 'c';    /* char devices     */
                if ( S_ISBLK(info.st_mode) )  modestr[0] = 'b';    /* block device     */

                if ( info.st_mode & S_IRUSR ) modestr[1] = 'r';    /* 3 bits for user  */
                if ( info.st_mode & S_IWUSR ) modestr[2] = 'w';
                if ( info.st_mode & S_IXUSR ) modestr[3] = 'x';

                if ( info.st_mode & S_IRGRP ) modestr[4] = 'r';    /* 3 bits for group */
                if ( info.st_mode & S_IWGRP ) modestr[5] = 'w';
                if ( info.st_mode & S_IXGRP ) modestr[6] = 'x';

                if ( info.st_mode & S_IROTH ) modestr[7] = 'r';    /* 3 bits for other */
                if ( info.st_mode & S_IWOTH ) modestr[8] = 'w';
                if ( info.st_mode & S_IXOTH ) modestr[9] = 'x';

                write(STDOUT_FILENO,modestr,10);
                write(STDOUT_FILENO,direntptr->d_name,strlen(direntptr->d_name));
            }
            closedir(dir_ptr);
        }
    }
    else if(!strcmp(args[0],"cd")) {
        if(args[1] == '\0'){ //if after cd command there is nothing then do below
            const char* home = getenv("HOME"); //get the env variable that indicate "HOME"
            int flag = chdir(home); //get the full path to the home directory
            if (flag < 0) //case is where there is no such as HOME directory
                write(STDOUT_FILENO,"Error changing directory",24);
        }
        else{ //otherwise write up the full path to HOME directory
            chdir(args[1]);
            //we need to implement the case of "cd .." and how they show as directory
            //such as /.../ecs150% and etc...
        }
    }
    else if(!strcmp(args[0],"pwd")) {
        char current_Buff[300];
        getcwd(current_Buff, 300); //get the current dictory path and store into current_Buff
        write(STDOUT_FILENO,current_Buff,sizeof(current_Buff));
        write(STDOUT_FILENO,"\n",1);
    }
    else if(!strcmp(args[0],"ff")) {

    }
    else {
       /* if(argc == 1) {
            execvp();
        } */
        write(STDOUT_FILENO,"execute",7);
    }
  }

    ResetCanonicalMode(STDIN_FILENO, &SavedTermAttributes); //switch to CanonoicalMode
    return 0;
}
