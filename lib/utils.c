#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libhttp.h"

/**
 * @brief Remove trailing new line of the string
 * @param str
 */
void removeTrailingNewLine(char * str)
{
    int size = strlen(str);

    if(str[size - 1] == '\n')
        str[strlen(str)-1] = 0;

}

/**
 * @brief Reads a file containing an api key, should be freed!
 * @param filename
 * @return 
 */
char * getApiKey(const char * filename) 
{
    char * api = readFile(filename);

    removeTrailingNewLine(api);

    return api;
}

/**
 * @brief Replaces a string char by another must be freed!
 * @param str
 * @param find
 * @param replace
 * @return 
 */
char *replace_char (const char *str, char find, char replace) {
    char * copy;
    copy = malloc(sizeof(char) * strlen(str) + 1);
    strcpy(copy, str);
    
    int i=0;
    while(copy[i]!='\0') {
        if(copy[i] == find) {
           copy[i] = replace;
        }  
        
        i++; 
    }
    
    return copy;
}

/**
 * @brief String encode spaces, should be freed!
 * @param qry
 * @return 
 */
char * strenconde(const char* qry)
{    
    //count spaces and get the new size
    int new_string_length = 0;
    for (const char *c = qry; *c != '\0'; c++) {
        if (*c == ' ') new_string_length += 2;
        new_string_length++;
    }
    
    // alloc the new string with the new size
    char *qstr = malloc((new_string_length + 1) * sizeof qstr[0]);
    const char *c1 = qry;
    char *c2;
    for (c2 = qstr; *c1 != '\0'; c1++) {
        if (*c1 == ' ') {
            c2[0] = '%';
            c2[1] = '2';
            c2[2] = '0';
            c2 += 3;
        }else{
            *c2 = *c1;
            c2++;
        }
    }
    *c2 = '\0';
    
    return qstr;
}
