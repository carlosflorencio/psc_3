#ifndef PSC_LIBHTTP_H
#define PSC_LIBHTTP_H
#include <jansson.h>

// HTTP.c
char* request(const char* url);
int httpGetToFile(const char * uri, const char * filename);
json_t * httpGetJsonData(const char *uri);

// IO.c
char * readFile(const char *filename);
int writeFile(const char * str, const char * filename);

// UTILS.c
void removeTrailingNewLine(char * str);
char * getApiKey(const char * filename);
char *replace_char (const char *str, char find, char replace);
char * strenconde(const char* qry);

// PARSER.c
#define MALLOC_STRING(length) malloc(sizeof(char) * length + 1);
#define SEARCH_URL "https://www.googleapis.com/books/v1/volumes?q=\"%s\"&key=%s"
#define VOLUME_URL "https://www.googleapis.com/books/v1/volumes/%s?key=%s"

typedef struct volume {
    char *id;
    char *selfLink;
    char *title;
    char *isbn13;
} Volume;

typedef struct collection {
    int count;
    Volume **volumes;
} Collection;

int googleBooksSearchVolumes(const char *apikey, const char *query_parameters, Collection *c);
int googleBooksGetEpubUrl(const char *apikey, const char *volumeId, char *epub_url, size_t maxlen);
char * makeSearchUri(const char *api, const char *query);
char * makeVolumeUri(const char *api, const char *id);
int makeVolume(const json_t *json, Volume **v);
void cleanVolume(Volume *v);
int makeCollection(const json_t *json, Collection *c);
void cleanCollection(Collection *c);
int getStringFromJson(char **dest, const json_t *stringObj);
int getIsbn13FromVolumeInfo(char **dest, json_t *volumeInfo);
void printVolume(const Volume *v);
void printCollection(const Collection *c);

#endif // PSC_LIBHTTP_H
