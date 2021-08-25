#include <stdio.h>

#include <stdlib.h>

#include <curl/curl.h>

#include <string.h>

#include<stdlib.h>


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

/* Show translation only */
void printTrad(char * str) {
    for (int i = 46; i < strlen(str); i++) {
        if (str[i] != '"')
            printf("%c", str[i]);
        else {
            printf("\n");
            break;
        }

    }
}

/* Main Program */
void curlTrad(int lang){

    char wanted[100];
    char * trad = NULL;
    int sizeOfWanted = 0;

    /* Your Api Key */
    char key[] = "Key";

    struct MemoryStruct chunk;

    /* will be grown as needed by the realloc above */
    chunk.memory = malloc(1);
    /* no data at this point */ 
    chunk.size = 0; 
    

    /*We get the sentence we want to translate*/
    fgets(wanted, 100, stdin);

    /*remove "\n" from the sentence*/
    sizeOfWanted = strcspn(wanted, "\n");
    wanted[sizeOfWanted] = 0; 

    /*the string that we are going to add will be an array of chars of 33 elements*/
    trad = malloc((sizeOfWanted + 33) * sizeof(char));

    /* Complete the sentence to be sent */
    if(lang == 0){
        sprintf(trad, "{\"text\":[\"%s\"],\"model_id\":\"fr-en\"}", wanted);
    }else{
        sprintf(trad, "{\"text\":[\"%s\"],\"model_id\":\"en-fr\"}", wanted);
    }

    CURL * hnd = curl_easy_init();

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(hnd, CURLOPT_URL, "https://api.eu-gb.language-translator.watson.cloud.ibm.com/instances/3bfff264-49fc-45f1-b77e-f652c0237310/v3/translate?version=2018-05-01");
    curl_easy_setopt(hnd, CURLOPT_USERPWD, "apikey");
    curl_easy_setopt(hnd, CURLOPT_PASSWORD, key);

    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void * ) & chunk);

    struct curl_slist * headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(hnd, CURLOPT_SSL_VERIFYHOST, 0L);
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
}
int main(int argc, char *argv[] ) {
    /*The default language is French, if the user wants to translate from English */
    /* he need add "en" */
    if(argc == 2)
    {  
        if(strcmp(argv[1],"en") == 0)
            curlTrad(1);
        else
            curlTrad(0);
    }
    else{
        curlTrad(0);
    }
        

    return 0;

}