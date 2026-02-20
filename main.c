#include "file.h"
#include "directory.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    string *command = create_string();
    for (int i = 0; i < argc; i++) {
        if (i == 0) {
            string_append_str(command, "gcc");//append gcc command for the first argument
            continue;
        }
        size_t len_of_arg = strlen(argv[i]); // get len of the argv[i]
        if (strcmp(argv[i]+(len_of_arg-2), ".h") ==0 || strcmp(argv[i]+(len_of_arg-2), ".c") ==0) { // check whether the last two positions of the argv[i] has the extension either .h or .c
            if(file_exists(argv[i])){
                char* file_content=file_read_text(argv[i]);
                if(file_content==NULL){ // if the file content is NULL exit the program
                    printf("unable to read file '%s'\n",argv[i]);
                    exit(EXIT_FAILURE);
                }
            }
            else{
                printf("file '%s' does not exist\n",argv[i]);
                exit(EXIT_FAILURE);// if file does not exist exit the program
            }
        }
        else {
            string_append_str(command," ");//append <space> for the gcc args
            string_append_str(command,argv[i]);//append the argv[i]
        }
    }
    printf("final gcc command : '%s'\n",string_cstr(command));
    free_string(&command);
}
