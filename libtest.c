#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lib/libhttp.h"
#include <jansson.h>

/**
 * @brief Test httpGetToFile method
 * @param uri
 * @param filename
 * @return 
 */
int testHttpGetToFile(const char *uri, const char *filename)
{
    int res = httpGetToFile(uri, filename);
    
    if(res) {
        perror("Error loading http response to the file!\n");
        return 1;
    }
    
    // http response and size
    char * response = request(uri);
    int response_size = strlen(response);
    
    FILE * fp = fopen(filename,"r"); // read mode
    if( fp == NULL )
    {
      perror("Error while opening the file.\n");
      return 1;
    }
    
    //size of the file contents
    int file_chars = 0;
    int ch;
    while (1) {
            ch = fgetc(fp);
            if (ch == EOF)
                break;
            ++file_chars;
    }
    
    fclose(fp);
    free(response);
    remove(filename);
    
    return file_chars == response_size ? 0 : 1;
}

/**
 * @brief Test httpGetJsonData method
 * @return 
 */
int testHttpGetJsonData()
{
    char * uri = "https://adeetc.thothapp.com/api/v1";
    json_t *root = httpGetJsonData(uri);
    
    if(!root) {
        return 1;
    }
    
    json_t* app = json_object_get(root, "application");
    
    if(strcmp(json_string_value(app), "Thoth") == 0) {
        json_decref(root);
        return 0;
    }
    
    json_decref(root);
    return 1;
}


int main()
{
    char * api = getApiKey("api_key.txt");
    int res = 0;
    
    res += testHttpGetToFile("https://adeetc.thothapp.com/api/v1", "test_http.txt");
    res += testHttpGetJsonData();
    
    //Test googleBooksSearch
    Collection *c = malloc(sizeof(Collection));
    int col = googleBooksSearchVolumes(api, "Moby Dick", c);
    res += col == -1 ? 1 : 0;
    printCollection(c);
    
    //Test googleBooksGetEpubUrl
    size_t size = 200;
    char *epub = malloc(sizeof(char) * size);
    for(int i = 0; i < c->count; i++) {
        int success = googleBooksGetEpubUrl(api, c->volumes[i]->id, epub, size);
    
        if(success) {
            printf("Epub Url: %s\n\n", epub);
        } else {
            printf("No Epub Url!\n\n");
        }
    }
    
    
    cleanCollection(c);
    free(epub);
    
    if(res) {
        printf("%d tests failed!\n", res);
    } else {
        printf("All tests passed!\n");
    }
    
    free(api);
    
    return 0;
}