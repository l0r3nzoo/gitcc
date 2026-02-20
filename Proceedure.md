# gitcc - v0.0.1

## Processing Logic

1. Take arguments the same way as gcc (linux) or (cl)
2. Check every arg in the argv check if the last two positions of string match with .c or .h
3. Check if the file exists 
4. Open file and find #include_url directives 
5. For each directive download the file from internet and put it in a tmp folder and create tmp file replace the #include_url with downloaded file     
6. append the modified file to gcc or cl command string
7. Do this until all args are processed
8. call gcc or cl with the generated command

## Example 
- gitcc main.c -o main.exe (main.c has #include_url with url pointing to git or some internet file)
- download the file put it in a tmp folder
- modify the main.c file by replacing the #include_url directive with #include "/tmp/downloadedfile.h/c"
- build command for gcc or cl, gcc /tmp/main.c -o main.exe
