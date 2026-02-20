#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include "directory.h"
#include "string.h"
#include "vector.h"

int main(int argc, char **argv) {
    string *command = create_string();
    for (int i = 0; i < argc; i++) {
        if(i==0)continue;//skip the first argument
        if (i == 1) {//consider second argument for the compiler command
            if(strcmp(argv[i],"gcc")==0 || strcmp(argv[i],"cl")==0){
                string_append_str(command, argv[i]);//append gcc command for the first argument
            }
            else{
                printf("unsupported compiler '%s'\n",argv[i]);
                free_string(&command);
                exit(EXIT_FAILURE);
            }
            continue;
        }
        size_t len_of_arg = strlen(argv[i]); // get len of the argv[i]
        if (strcmp(argv[i]+(len_of_arg-2), ".h") ==0 || strcmp(argv[i]+(len_of_arg-2), ".c") ==0) { // check whether the last two positions of the argv[i] has the extension either .h or .c
            if(file_exists(argv[i])){
                char* file_content=file_read_text(argv[i]);
                if(file_content==NULL){ // if the file content is NULL exit the program
                    printf("unable to read file '%s'\n",argv[i]);
                    free_string(&command);
                    exit(EXIT_FAILURE);
                }
                string* file_string=create_string(); //create file_string
                if(string_append_str(file_string, file_content)){//append file_content to file_string, if unable to append exit the program
                    printf("unable to create temporary file string\n");
                    free_string(&command);
                    free_string(&file_string);
                    exit(EXIT_FAILURE);
                }
                free_string(&file_string);
            }
            else{
                printf("file '%s' does not exist\n",argv[i]);
                free_string(&command);
                exit(EXIT_FAILURE);// if file does not exist exit the program
            }
        }
        else {
            string_append_str(command," ");//append <space> for the gcc args
            string_append_str(command,argv[i]);//append the argv[i]
        }
    }
    printf("final compiler command : '%s'\n",string_cstr(command));
    free_string(&command);
}
