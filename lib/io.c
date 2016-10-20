#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Read a file, space allocated! Should be freed
 * @param filename
 * @return 
 */
char * readFile(const char *filename)
{
    char *buffer = NULL;
    int string_size,read_size;
    FILE *handler = fopen(filename,"r");

    if (handler != NULL)
    {
        //seek the last byte of the file
        fseek(handler,0,SEEK_END);
        //offset from the first to the last byte, or in other words, filesize
        string_size = ftell (handler);
        //go back to the start of the file
        rewind(handler);

        //allocate a string that can hold it all
        buffer = (char*) malloc (sizeof(char) * (string_size + 1) );
        //read it all in one operation
        read_size = fread(buffer,sizeof(char),string_size,handler);
        //fread doesnt set it so put a \0 in the last position
        //and buffer is now officialy a string
        buffer[string_size] = '\0';

        if (string_size != read_size) {
           //something went wrong, throw away the memory and set
           //the buffer to NULL
           free(buffer);
           buffer = NULL;
        }
    } else {
        //perror("Error");
        return NULL;
    }

    fclose(handler);

    return buffer;
}

/**
 * @brief Write file to filesystem
 * @param str
 * @param filename
 */
int writeFile(const char * str, const char * filename)
{
    FILE *f = fopen(filename, "wb");
    if (f == NULL)
    {
        printf("Error opening file!\n");
        return 1;
    }

    fprintf(f, "%s", str);
    fclose(f);
    
    return 0;
}