#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file.h"
#include "directory.h"
#include "string.h"
#include "vector.h"

#define TEMPDIR ".gitcc/"
#define OFFSET 19

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
                    free(file_content);
                    exit(EXIT_FAILURE);
                }
                size_t len_of_content=strlen(file_content);
                if(len_of_content==0){// if the len of the file content is 0 skip to next iteration
                    printf("empty file passed skipping\n");
                    free_string(&command);
                    free(file_content);
                    continue;
                }
                string* file_string=create_string();
                if(file_string==NULL){//unable to create string, exit the program
                    free_string(&command);
                    free(file_content);
                    exit(EXIT_FAILURE);
                }
                string_append_str(file_string,file_content);
                vector* file_string_vector=string_split_char(file_string, '\n');
                if(file_string_vector==NULL){//unable to create string vector, exit the program
                    free_string(&command);
                    free_string(&file_string);
                    free(file_content);
                    exit(EXIT_FAILURE);
                }

                for(int j=0;j<vec_count(file_string_vector);j++){
                    string* line=vec_at(file_string_vector,j);
                    if(string_startswith_str(line, "#include_url")){
                        int url_start_index=string_indexof_char(line,'"');
                        int url_end_index=string_lastindexof_char(line,'"');
                        if(url_start_index==-1||url_end_index==-1){
                            printf("invalid directive at line %d : %s",j,string_cstr(line));
                        }
                        size_t allocation_size=(url_end_index-url_start_index)-1;
                        char* url_buffer =malloc(allocation_size+1);
                        strncpy(url_buffer, string_cstr(line)+url_start_index+1, (url_end_index-url_start_index)-1);
                        url_buffer[allocation_size]='\0';//append null terminator
                        printf("captured url : %s\n",url_buffer);

                        //create directory path by removing domain from url_buffer
                        char* directory_buffer=malloc(strlen(url_buffer)+1);
                        char* destination_buffer=malloc(strlen(url_buffer)+1);
                        memcpy(directory_buffer, url_buffer, strlen(url_buffer)+1);
                        memcpy(destination_buffer, url_buffer, strlen(url_buffer)+1);
                        memmove(directory_buffer,directory_buffer+OFFSET,strlen(directory_buffer) + 1 - OFFSET);
                        memmove(destination_buffer,destination_buffer+OFFSET,strlen(destination_buffer) + 1 - OFFSET);
                        string* directory_string = create_string();
                        string_append_str(directory_string,directory_buffer);
                        string* destination_string=create_string();
                        string_append_str(destination_string, destination_buffer);
                        string_prepend_str(destination_string, TEMPDIR);
                        string_prepend_str(directory_string,TEMPDIR);
                        char* temp_directory_buffer=(char*)string_cstr(directory_string);
                        size_t lastindexofforwardslash=string_lastindexof_char(directory_string, '/');
                        temp_directory_buffer[lastindexofforwardslash+1]='\0';


                        dir_create_recursive((char*)string_cstr(directory_string));
                        char* command_buffer=malloc(2048);
                        sprintf(command_buffer, "powershell -Command \"Invoke-WebRequest -Uri '%s' -OutFile '%s'\"",url_buffer,string_cstr(destination_string));
                        if(!file_exists(string_cstr(destination_string))){
                            system(command_buffer);
                        }
                        if(file_exists(string_cstr(destination_string))){
                            char* include_buffer=malloc(2048);
                            sprintf(include_buffer, "#include \"%s\"", (char*)string_cstr(destination_string));
                            if(string_replace_str(file_string, string_cstr(line), include_buffer)!=0){
                                printf("unable to replace the directive\n");
                                free(include_buffer);
                                free_string(&command);
                                free_string(&file_string);
                                free(file_content);
                                free_string(&directory_string);
                                free_string(&destination_string);
                                free(directory_buffer);
                                free(destination_buffer);
                                free(url_buffer);
                                free(command_buffer);
                                exit(EXIT_FAILURE);
                            }
                            string* file_path_string=create_string();
                            string_append_str(file_path_string, argv[i]);
                            size_t lastindexofforwardslash=string_lastindexof_char(file_path_string, '\\');
                            string_remove_range(file_path_string, 0, lastindexofforwardslash+1);
                            string_prepend_str(file_path_string, TEMPDIR);
                            file_create(string_cstr(file_path_string));
                            file_append_text(string_cstr(file_path_string), string_cstr(file_string));
                            string_append_str(command," ");//add space
                            string_append_str(command, string_cstr(file_path_string));//add the input file
                            free_string(&file_path_string);
                            free(include_buffer);
                        }
                        else{
                            printf("unable to download the file : '%s'\n",url_buffer);
                            free_string(&command);
                            free_string(&file_string);
                            free(file_content);
                            free_string(&directory_string);
                            free_string(&destination_string);
                            free(directory_buffer);
                            free(destination_buffer);
                            free(url_buffer);
                            free(command_buffer);
                            exit(EXIT_FAILURE);
                        }

                        free_string(&directory_string);
                        free_string(&destination_string);
                        free(directory_buffer);
                        free(destination_buffer);
                        free(url_buffer);
                        free(command_buffer);
                    }
                }

                free_string_vector(&file_string_vector);
                free_string(&file_string);
                free(file_content);
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
    system(string_cstr(command));
    free_string(&command);
}
