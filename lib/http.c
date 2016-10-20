#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>
#include "libhttp.h"

/**
 *  The buffer type.
 */
typedef struct _Buffer {
    size_t size;
    char *memory;
} Buffer;

 /**
  * @brief A write_function that copies data to a buffer.
  * @param data
  * @param size
  * @param nmemb
  * @param context
  * @return 
  */
size_t write_to_memory(char *data, size_t size, size_t nmemb, void *context)
{
    size_t byteCount = size * nmemb;
    Buffer *bufptr = (Buffer *)context;

    /* grow the buffer in order to hold more byteCount bytes */
    bufptr->memory = realloc(bufptr->memory, bufptr->size + byteCount + 1);
    if (bufptr->memory == NULL) {
        /* out of memory! */
        fprintf(stderr, "***error: not enough memory\n");
        return 0;
    }
    /* copy the new data block */
    memcpy(bufptr->memory + bufptr->size, data, byteCount);

    /* update size and close the C string */
    bufptr->size += byteCount;
    bufptr->memory[bufptr->size] = '\0';


    return byteCount;
}

/**
 * 
 * 
 */
/**
 * @brief Execute an HTTP GET for the specified URL and return data in a memory buffer
 * dynamically allocated.
 * @param url
 * @return pointer to the response
 */
char *request(const char *url)
{
    CURL *curl_handle;
    CURLcode curl_result;
    Buffer buffer = { .size = 0, .memory = NULL };
    char *result = NULL;

    /* initialize the libcurl library */
    curl_global_init(CURL_GLOBAL_ALL);

    /* initializa a CURL handle in order to perform an HTTP GET */
    curl_handle = curl_easy_init();
    if (!curl_handle) {
        fprintf(stderr, "***error: curl_easy_init() failed\n");
        goto global_cleanup;
    }

    /* set the VERBOSE option */
    //curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);

    /* configure the handler with the URL */
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

    /* some servers don't like requests that are made without a user-agent
     field, so we provide one */
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    /* follow HTTP 3xx redirects */
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);

    /* set WRITEFUNCTION and WRITEDATA to save body data in a block of memory */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_to_memory);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &buffer);

    /* perform the HTTP GET request */
    curl_result = curl_easy_perform(curl_handle);
    if (curl_result != CURLE_OK) {
        fprintf(stderr, "***error: curl_easy_perform() failed: %s\n",
                curl_easy_strerror(curl_result));
        goto handle_cleanup;
    }

    /* we got success, return the memory buffer */
    result = buffer.memory;

    /* for debug purposes, show the contents of the data buffer */
    /*
    fprintf(stderr, "%zu bytes received\n", (long)buffer.size);
    fprintf(stdout, "%.*s", (int)buffer.size, buffer.memory);
    */

    handle_cleanup:
    /* cleanup the curl handler */
    curl_easy_cleanup(curl_handle);

    global_cleanup:
    /* global libcurl cleanup */
    curl_global_cleanup();
    
    return result;
}

/**
 * @brief Write a http response to a file
 * @param uri
 * @param filename
 * @return 1 if error or 0 if ok
 */
int httpGetToFile(const char * uri, const char * filename)
{
    char * response = request(uri);
    
    int res = writeFile(response, filename);
    
    free(response);
    
    return res;
}

/**
 * @brief Create a json object from an http response
 * @param uri
 * @return 
 */
json_t* httpGetJsonData(const char *uri)
{
    json_t *root;
    json_error_t error;

    char * response = request(uri);
    root = json_loads(response, 0, &error);
    free(response);

    if(!root){
        fprintf(stderr, "Error loading json: on line %d: %s\n", error.line, error.text);
        return NULL;
    }
    
    return root; 
}