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

int test_show(char *ollama_host, char *model) 
{
    ollama_t *ollama = ollama_init(ollama_host, NULL, 0);
    if (!ollama) {
        return -1;
    }

    ollama_result_t res;
    res.type = OLLAMA_RESULT_TYPE_MEM_DYNAMIC;

    int ret;
    ret = ollama_show(ollama, model, 1, &res);
    if (ret != 0) {
        return -1;
    }

        cJSON *root;
    char *lstr;

    if ((root = cJSON_Parse(res.buf.base)) == NULL) {
        return -1;
    }

    if ((lstr = cJSON_Print(root)) == NULL) {
        return -1;
    }

    printf("ollama_show('%s'):\n%s\n", model, lstr);
    free(lstr);
    free(res.buf.base);
    cJSON_Delete(root);

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

    return test_show(host, model);
}
