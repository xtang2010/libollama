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

int test_copy_delete(char *ollama_host, char *model) 
{
    ollama_result_t res;

    ollama_t *ollama = ollama_init(ollama_host, NULL, 0);
    if (!ollama) {
        return -1;
    }

    char new_model[128];
    sprintf(new_model, "%s_copy", model);

    printf("Copy '%s' to '%s': ", model, new_model);
    fflush(stdout);
    if (ollama_copy(ollama, model, new_model, NULL) == -1) {
        printf("Failed\n");
        return -1;
    }
    printf("Successed.\n\n");

    res.type = OLLAMA_RESULT_TYPE_MEM_DYNAMIC;

    printf("List: ");
    fflush(stdout);
    if (ollama_list(ollama, &res) == -1) {
        printf("Failed\n");
        return -1;
    }
    printf("\n%s\n\n", res.buf.base);
    free(res.buf.base);

    printf("Delete '%s': ", new_model);
    fflush(stdout);
    if (ollama_delete(ollama, new_model, NULL) == -1) {
        printf("Failed\n");
        return -1;
    }
    printf("Successed.\n\n");

    res.type = OLLAMA_RESULT_TYPE_MEM_DYNAMIC;
    printf("List: ");
    fflush(stdout);
    if (ollama_list(ollama, &res) == -1) {
        printf("Failed\n");
        return -1;
    }
    printf("\n%s\n\n", res.buf.base);
    free(res.buf.base);

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

    return test_copy_delete(host, model);
}
