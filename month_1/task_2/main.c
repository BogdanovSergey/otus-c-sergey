#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#include <curl/curl.h>
#include "cjson/cJSON.h"

struct MemoryStruct {
  char *memory;
  size_t size;
};

void write_json(size_t size, const char *memory);
void parse_json(size_t size, const char *memory);

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
    CURLcode result = CURLE_OK;
    
    struct MemoryStruct chunk;
    
    result = curl_global_init(CURL_GLOBAL_ALL);
    if(result != CURLE_OK)
        return (int)result;
    
    chunk.memory = malloc(1); /* grown as needed by the realloc above */
    chunk.size = 0;           /* no data at this point */
    if (chunk.memory == NULL) {
        printf("Failed to allocate memory\n");
        curl_global_cleanup();
        return EXIT_FAILURE;
    }
    
    /* init the curl session */
    curl = curl_easy_init();
    if(curl) {
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
    
        result = curl_easy_perform(curl);
    
        if(result != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(result));
        } else {
            if(chunk.memory) {
                printf("%zu bytes retrieved\n", chunk.size);
                //printf("%s\n", chunk.memory);

                write_json(chunk.size, chunk.memory);
                parse_json(chunk.size, chunk.memory);
            }

        }
    
        /* cleanup curl stuff */
        curl_easy_cleanup(curl);
    } else {
        printf("Bad curl init\n");
        curl_global_cleanup();
        return EXIT_FAILURE;
    }
    
    free(chunk.memory);
    
    curl_global_cleanup();
    
    if(result != CURLE_OK) {
        printf("Bad curl result\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void write_json(size_t size, const char *memory) {
    FILE * f;
    f = fopen("./result.json", "w");
    if(f) {
        fwrite(memory, size, 1, f);
        fclose(f);
    }
}

void parse_json(size_t size, const char *memory) {
    cJSON *json = cJSON_ParseWithLength(memory, size);
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "JSON parse error before: %s\n", error_ptr);
        }
        return;
    }

    printf("\n");
    cJSON *current_condition = cJSON_GetObjectItemCaseSensitive(json, "current_condition");
    if (cJSON_IsArray(current_condition)) {
        cJSON *el0 = cJSON_GetArrayItem(current_condition, 0);

        // текстовое описание погоды
        cJSON *weather_desc = cJSON_GetObjectItemCaseSensitive(el0, "weatherDesc");
        if (cJSON_IsArray(weather_desc)) {
            cJSON *first_desc = cJSON_GetArrayItem(weather_desc, 0);
            cJSON *value = cJSON_GetObjectItemCaseSensitive(first_desc, "value");
            if (cJSON_IsString(value) && (value->valuestring != NULL)) {
                printf("Weather: %s\n", value->valuestring);
            }
        }

        // Направление и скорость ветра
        cJSON *win_dir = cJSON_GetObjectItemCaseSensitive(el0, "winddir16Point");
        if (cJSON_IsString(win_dir) && (win_dir->valuestring != NULL)) {
                printf("Wind direction: %s\n", win_dir->valuestring);
        }
        cJSON *win_speed = cJSON_GetObjectItemCaseSensitive(el0, "windspeedKmph");
        if (cJSON_IsString(win_speed) && (win_speed->valuestring != NULL)) {
                printf("Wind speed: %s Kmph\n", win_speed->valuestring);
        }

        // Температура
        cJSON *temp = cJSON_GetObjectItemCaseSensitive(el0, "temp_C");
        if (cJSON_IsString(temp) && (temp->valuestring != NULL)) {
                printf("Temp: %sC\n", temp->valuestring);
        }

    }
    

    cJSON_Delete(json);
}
