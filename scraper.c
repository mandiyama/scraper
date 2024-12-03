#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

struct Memory {
    char *response;
    size_t size;
};

static size_t write_callback(void *ptr, size_t size, size_t nmemb, struct Memory *data) {
    size_t total_size = size * nmemb;
    data->response = realloc(data->response, data->size + total_size + 1);
    if (data->response == NULL) {
        printf("mem allocation err\n");
        exit(1);
    }
    memcpy(data->response + data->size, ptr, total_size);
    data->size += total_size;
    data->response[data->size] = '\0'; 
    return total_size;
}

int main() {
    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "initialization err curl\n");
        return 1;
    }

    const char *url = "https://example.com/";
    struct Memory data = { .response = malloc(1), .size = 0 };

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "err curl: %s\n", curl_easy_strerror(res));
    } else {
        printf("HTML:\n%s\n", data.response);
        FILE *output_file = fopen("html.txt", "w");
        if (output_file == NULL) {
            fprintf(stderr, "err when opening a file for recording\n");
            return 1;
        }
        fprintf(output_file, "HTML:\n%s\n", data.response);
        fclose(output_file);
        printf("HTML code of the page is written to a file html.txt\n");
    }
    free(data.response);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return 0;
}
