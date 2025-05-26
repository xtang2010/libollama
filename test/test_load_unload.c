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

int test_load_unload(char *ollama_host, char *model) 
{
    char buffer[4096];
    int ret;
    ollama_result_t res;

    ollama_t *ollama = ollama_init(ollama_host, NULL, 0);
    if (!ollama) {
        return -1;
    }

    printf("Loading model '%s': ", model);
    if (ollama_load(ollama, model, NULL) == -1) {
        printf("Failed\n");
        return -1;
    }
    printf("Successed\n");

    printf("ollama_ps: \n");
    res.type = OLLAMA_RESULT_TYPE_MEM_DYNAMIC;
    ret = ollama_ps(ollama, &res);
    if (ret == -1) {
        printf("Failed\n");
        return -1;
    }
    cJSON *o = cJSON_Parse(res.buf.base);
    if (!o) {
        free(res.buf.base);
        printf("Failed\n");
        return -1;
        }
    char *os = cJSON_Print(o);
    printf("%s\n", os);
    free(os);
    cJSON_Delete(o);
    free(res.buf.base);

    printf("Unloading model '%s': ", model);
    if (ollama_unload(ollama, model, NULL) == -1) {
        printf("Failed\n");
        return -1;
    }
    printf("Successed\n");

    printf("ollama_ps: ");
    memset(buffer, 0, 2048);
    res.type = OLLAMA_RESULT_TYPE_MEM;
    res.buf.base = buffer;
    res.buf.size = 2048;
    sleep(1);
    ret = ollama_ps(ollama, &res);
    if (ret != 0) {
        printf("Failed\n");
        return -1;
    }

    o = cJSON_Parse(res.buf.base);
    if (!o) {
        printf("Failed\n");
        return -1;
    }
    os = cJSON_Print(o);
    printf("%s\n", os);
    free(os);
    cJSON_Delete(o);

    ollama_close(ollama);
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

    return test_load_unload(host, model);
}
