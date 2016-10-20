#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libhttp.h"
#include "jansson.h"

/**
 * @brief Search books by name
 * @param apikey
 * @param query_parameters
 * @param res
 * @return 
 */
int googleBooksSearchVolumes(const char *apikey, const char *query_parameters, Collection *c)
{
    char * uri = makeSearchUri(apikey, query_parameters);
    json_t *root = httpGetJsonData(uri);
    
    int res = makeCollection(root, c);
    
    json_decref(root);
    free(uri);
    
    return res == 0 ? c->count : -1;
}

/**
 * @brief Get epub url from a volume
 * @param apikey
 * @param volumeId
 * @param epub_url
 * @param maxlen
 * @return 
 */
int googleBooksGetEpubUrl(const char *apikey, const char *volumeId, char *epub_url, size_t maxlen)
{
    int ok = 0;
    char *uri = makeVolumeUri(apikey, volumeId);
    json_t *json = httpGetJsonData(uri);
    
    json_t *accessInfo, *epub, *isAvaiable, *downloadLink;
    
    accessInfo = json_object_get(json, "accessInfo");
    if(!json_is_object(accessInfo)) { printf("Error! accessInfo is not an object!\n"); return 0; }
    
    epub = json_object_get(accessInfo, "epub");
    if(!json_is_object(epub)) { printf("Error! epub is not an object!\n"); return 0; }
    
    isAvaiable = json_object_get(epub, "isAvailable");
    if(!json_is_boolean(isAvaiable)) { printf("Error! isAvaiable not there!\n"); return 0; }
    
    downloadLink = json_object_get(epub, "downloadLink");
    if(!json_is_string(downloadLink)) {
        // no download link? lets check for acsTokenLink
        downloadLink = json_object_get(epub, "acsTokenLink");
        if(!json_is_string(downloadLink)) {
            goto EXIT; // no link or token link avaiable
        } else {
            goto COPY;
        }
    } else {
            goto COPY;
    }
    
    COPY:
        strncpy(epub_url, json_string_value(downloadLink), maxlen - 1);
        epub_url[maxlen - 1] = '\0';
        ok = 1;
    
    EXIT:
    json_decref(json);
    free(uri);
    
    return ok;
}

/**
 * @brief Create a volume uri, should be freed
 * @param api
 * @param id
 * @return 
 */
char * makeVolumeUri(const char *api, const char *id)
{    
    char * uri = malloc(sizeof(char) * 255 +1);
    sprintf(uri, VOLUME_URL, id, api);
    
    return uri;
}

/**
 * @brief Create a search uri based on the api and query
 * @param api
 * @param query
 * @return 
 */
char * makeSearchUri(const char *api, const char *query)
{
    //replace spaces with %20
    char * queryEncoded = strenconde(query);
    
    char * uri = malloc(sizeof(char) * 255 +1);
    sprintf(uri, SEARCH_URL, queryEncoded, api);
    
    free(queryEncoded);
    return uri;
}

/**
 * @brief Obtain and allocate a string from a json string object
 * @param dest
 * @param stringObj
 * @return 
 */
int getStringFromJson(char **dest, const json_t *stringObj)
{
    if(!json_string_value(stringObj)){ printf("Error! Not a json string!\n"); return -1; }
    
    const char *str = json_string_value(stringObj);
    *dest = malloc(sizeof(char) * strlen(str) + 1);
    strcpy(*dest, str);
    
    return 0;
}

/**
 * @brief Get a isbn13 from the volumeInfo json object
 * @param dest
 * @param volumeInfo
 * @return 
 */
int getIsbn13FromVolumeInfo(char **dest, json_t *volumeInfo)
{
    json_t *industryIdentifiers = json_object_get(volumeInfo, "industryIdentifiers");
    if(!json_is_array(industryIdentifiers)) { printf("Error! Invalid industryIdentifiers"); return -1; }
    
    *dest = NULL;
    for(int i=0; i < json_array_size(industryIdentifiers); i++) {
        json_t * item = json_array_get(industryIdentifiers, i);
        const char *type = json_string_value(json_object_get(item, "type"));
        if(strcmp(type, "ISBN_13") == 0) {
            getStringFromJson(dest, json_object_get(item, "identifier"));
            return 0;
        }
    }
    
    return 0;
}

/**
 * @brief Constructs a volume from a json object
 * @param json
 * @param v
 * @return 
 */
int makeVolume(const json_t *json, Volume **v)
{   
    json_t *volumeInfo = json_object_get(json, "volumeInfo");
    if(!json_is_object(volumeInfo)) { printf("Error! invalid volumeInfo!\n"); return -1; }
    
    *v = malloc(sizeof(Volume));
    if(*v == NULL) {
        printf("Error! No memory for volume.\n");
        return -1;
    }
    
    if( getStringFromJson(&(*v)->id, json_object_get(json, "id")) ||
        getStringFromJson(&(*v)->selfLink, json_object_get(json, "selfLink")) ||
        getStringFromJson(&(*v)->title, json_object_get(volumeInfo, "title")) ||
        getIsbn13FromVolumeInfo(&(*v)->isbn13, volumeInfo)) 
        {
            return -1;  //Error
        } else {
            return 0;   //Ok
        }
}

/**
 * @brief Clean volume memory
 * @param v
 */
void cleanVolume(Volume *v)
{
    free(v->id);
    free(v->selfLink);
    free(v->title);
    free(v->isbn13);
    free(v);
}

/**
 * @brief Create a collection from a json object
 * @param json
 * @param c
 * @return 
 */
int makeCollection(const json_t *json, Collection *c)
{
    json_t *itemsArray = json_object_get(json, "items");
    
    if(!json_is_array(itemsArray)) {
        printf("%s\n", "Error! JSON cant get the list items!");
        return -1;
    }
    
    c->count = json_array_size(itemsArray);
    c->volumes = malloc(sizeof(Volume) * c->count);
    if(c->volumes == NULL) { printf("Error, no memory avaiable!\n"); return -1;}
    
    for(int i = 0; i < c->count; i++) {
        json_t *jsonVolume = json_array_get(itemsArray, i);
        if(makeVolume(jsonVolume, &c->volumes[i]) != 0) {
            printf("Error creating a volume!\n");
            return -1;
        }
    }
    
    return 0;
}

/**
 * @brief Clean collection
 * @param c
 */
void cleanCollection(Collection *c) 
{
    for(int i=0; i<c->count; i++) {
        cleanVolume(c->volumes[i]);
    }
    free(c->volumes);
    free(c);
}

/**
 * @brief Prints a volume
 * @param v
 */
void printVolume(const Volume *v) 
{
    printf("\tID: %s\n", v->id);
    printf("\tTitle: %s\n", v->title);
    printf("\tSelfLink: %s\n", v->selfLink);
    printf("\tISBN13: %s\n\n", v->isbn13);
}

/**
 * @brief Prints a collection
 * @param c
 */
void printCollection(const Collection *c)
{
    printf("============== Collection\n");
    printf("Items: %d\n", c->count);
    
    for(int i = 0; i < c->count; ++i) {
        printVolume(c->volumes[i]);
    }
}