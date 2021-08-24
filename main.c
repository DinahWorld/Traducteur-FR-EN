#include <stdio.h>

#include <stdlib.h>

#include <curl/curl.h>

#include <string.h>


struct MemoryStruct {
    char * memory;
    size_t size;
};

static size_t
WriteMemoryCallback(void * contents, size_t size, size_t nmemb, void * userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct * mem = (struct MemoryStruct * ) userp;

    char * ptr = realloc(mem -> memory, mem -> size + realsize + 1);
    if (!ptr) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem -> memory = ptr;
    memcpy( & (mem -> memory[mem -> size]), contents, realsize);
    mem -> size += realsize;
    mem -> memory[mem -> size] = 0;
    return realsize;
}

void printTrad(char * str) {
    for (int i = 44; i < strlen(str); i++) {

        if (str[i] != '"')
            printf("%c", str[i]);

        else {
            printf("\n");
            break;
        }

    }
}

int main() {
    FILE* file = NULL;
    char key[51];
    char keyArgum[67];
    struct MemoryStruct chunk;

    chunk.memory = malloc(1); /* will be grown as needed by the realloc above */
    chunk.size = 0; /* no data at this point */

    char wanted[100];
    char * trad = NULL;
    int sizeOfWanted = 0;

    file = fopen("key.txt", "r+");
    fgets(key,51,file);
    
    if(file != NULL){
      fgets(key,sizeof key,file);
      sprintf(keyArgum, "x-rapidapi-key: %s", key);
    }
    else{
      printf("Error no Key\n");
      exit(-1);
    }
    fclose(file);
    
    CURL * hnd = curl_easy_init();

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(hnd, CURLOPT_URL, "https://google-translate1.p.rapidapi.com/language/translate/v2");

    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void * ) & chunk);

    struct curl_slist * headers = NULL;
    headers = curl_slist_append(headers, "content-type: application/x-www-form-urlencoded");
    headers = curl_slist_append(headers, "accept-encoding: application/gzip");
    headers = curl_slist_append(headers, "x-rapidapi-host: google-translate1.p.rapidapi.com");
    headers = curl_slist_append(headers, keyArgum);
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    //Need to comment
    fgets(wanted, 100, stdin);

    //remove "\n" from the text
    sizeOfWanted = strcspn(wanted, "\n");
    wanted[sizeOfWanted] = 0;

    //le string que l'on va ajouter sera un tableau de char de 23 éléments
    trad = malloc((sizeOfWanted + 23) * sizeof(char));
    sprintf(trad, "q=%s&target=en&source=fr", wanted);

    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, trad);
    CURLcode ret = curl_easy_perform(hnd);

    if (ret != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(ret));
    } else {
        printTrad(chunk.memory);
    }

    /* cleanup curl stuff */
    curl_easy_cleanup(hnd);

    free(chunk.memory);
    free(trad);

    return 0;

}