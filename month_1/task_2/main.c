#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include <curl/curl.h>

struct MemoryStruct {
  char *memory;
  size_t size;
};

void write_json(char *memory);

static size_t write_cb(char *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&mem->memory[mem->size], contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}


int main(int argc, char **argv) {
    if(argc != 2) {
        printf("Usage: %s <city>\n", argv[0]);
        return EXIT_FAILURE;
    }

    CURL *curl;
    CURLcode result;
    
    struct MemoryStruct chunk;
    
    result = curl_global_init(CURL_GLOBAL_ALL);
    if(result != CURLE_OK)
        return (int)result;
    
    chunk.memory = malloc(1); /* grown as needed by the realloc above */
    chunk.size = 0;           /* no data at this point */
    
    /* init the curl session */
    curl = curl_easy_init();
    if(curl) {
    
        /* specify URL to get */
        char url[200] = "https://wttr.in/";
        strcat(url, argv[1]);
        strcat(url, "?format=j1");
        printf("Connecting to: %s...\n", url);
        curl_easy_setopt(curl, CURLOPT_URL, url);
    
        /* send all data to this function */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    
        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    
        /* some servers do not like requests that are made without a user-agent
        field, so we provide one */
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    
        /* get it! */
        result = curl_easy_perform(curl);
    
        /* check for errors */
        if(result != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(result));
        }
        else {
        
            if(chunk.memory) {
                printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
                printf("%s\n", chunk.memory);

                write_json((unsigned long)chunk.size, chunk.memory);
            }

        }
    
        /* cleanup curl stuff */
        curl_easy_cleanup(curl);
    }
    
    free(chunk.memory);
    
    /* we are done with libcurl, so clean it up */
    curl_global_cleanup();
    
    if(!(int)result) {
        printf("Bad curl result\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void write_json(size_t size, char *memory) {
    FILE * f;
    f = fopen("./result.json", "w");
    fwrite(f, size, size, memory);
    fclose(f);

}