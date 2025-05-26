/*
    Copyright (c) 2025 xtang

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
#include <ollama/ollama.h>
#include <stdlib.h>
#include <unistd.h>
#include <cjson/cJSON.h>

static size_t generate_callback(void *contents, size_t size, size_t nmemb, void *handle)
{
    const char  *errptr;
    cJSON *root = cJSON_ParseWithOpts(contents, &errptr, 0);
    cJSON *response;

    if (root) {
        response = cJSON_GetObjectItemCaseSensitive(root, "response");
        if (response && strlen(response->valuestring) > 0) {
            printf("%s", response->valuestring);
            fflush(stdout);
        }
        if (cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(root, "done"))) {
            printf("\n\n");
            cJSON_Delete(root);            
        }
    } else {
        printf("Contents(%ld, %ld): %s\nError (%p): %s\n", size * nmemb, strlen((char *)contents), (char *)contents, errptr, errptr);
    }
    return size * nmemb;
}

int test_generate(char *ollama_host, char *model) {

    ollama_t *ollama = ollama_init(ollama_host, NULL, 0);
    if (!ollama) {
        return -1;
    }

    char *prompt = "你好";
    int ret;
    ollama_result_t res;

    res.type = OLLAMA_RESULT_TYPE_CALLBACK;
    res.call.callback = generate_callback;
    res.call.callback_handle = NULL;

    printf(">>> %s\n", prompt);
    printf("<<< ");

    ret = ollama_generate(ollama, model, prompt, NULL, &res);
    if (ret == 200) {
        printf("\n");
    }

    char *param = "{\"stream\": false}";

    prompt = "why the planet looks blue?";
    printf(">>> %s\n", prompt);
    printf("<<< ");

    res.type = OLLAMA_RESULT_TYPE_MEM_DYNAMIC;
    ret = ollama_generate(ollama, model, prompt, param, &res);
    if (ret == 0) {
        const char *errptr;
        cJSON *root = cJSON_ParseWithOpts(res.buf.base, &errptr, cJSON_False);

        if (!root) {
            printf("Contents(%ld): %s\nError (%p): %s\n", res.buf.len, res.buf.base, errptr, errptr);
            return -1;
        }

        cJSON *response = cJSON_GetObjectItemCaseSensitive(root, "response");
        if (response) {
            printf("%s\n", response->valuestring);
        }
        free(res.buf.base);
        printf("\n");
    }

    return 0;
}

int main(int argc, char **argv)
{
    char *host = "http://192.168.71.34:11434";
    char *model = "qwen3:4b";
    int opt;

    while ((opt = getopt(argc, argv, "h:m:")) != -1) {
        switch(opt) {
            case 'h':
                host = strdup(optarg);
                break;
            case 'm':
                model = strdup(optarg);
                break;
        }
    }

    return test_generate(host, model);
}
