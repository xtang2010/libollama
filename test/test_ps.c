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

int test_ps(char *ollama_host) 
{
    ollama_t *ollama = ollama_init(ollama_host, NULL, 0);
    if (!ollama) {
        return -1;
    }

    char buffer[4096];
    ollama_result_t res;

    res.type = OLLAMA_RESULT_TYPE_MEM;
    res.buf.base = buffer;
    res.buf.size = 4096;

    int ret;

    ret = ollama_ps(ollama, &res);
    if (ret != 0) {
        return -1;
    }

    cJSON *o = cJSON_Parse(res.buf.base);
    if (!o) {
        printf("Failed\n");
        return -1;
    }
    char *os = cJSON_Print(o);
    printf("%s\n", os);
    free(os);
    cJSON_Delete(o);

    return 0;
}

int main(int argc, char **argv)
{
    char *host = "http://192.168.71.34:11434";
    int opt;

    while ((opt = getopt(argc, argv, "h:")) != -1) {
        switch(opt) {
            case 'h':
                host = strdup(optarg);
                break;
        }
    }

    return test_ps(host);
}
