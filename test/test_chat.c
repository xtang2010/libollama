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
#include <errno.h>
#include <unistd.h>
#include <cjson/cJSON.h>

typedef struct chat_handle {
    cJSON  *history;
    char   *content;
    size_t content_len;
    size_t content_size;
} chat_handle_t;

static size_t chat_callback(void *contents, size_t size, size_t nmemb, void *handle)
{
    cJSON *message, *message_hist, *role, *content;
    size_t content_len;

    chat_handle_t *chandle = (chat_handle_t *)handle;
    const char  *errptr;
    cJSON *root = cJSON_ParseWithOpts(contents, &errptr, cJSON_False);

    if (root) {
        message = cJSON_GetObjectItemCaseSensitive(root, "message");
        content = cJSON_GetObjectItemCaseSensitive(message, "content");
        if (!message || !content) {
            printf("contents: %s\n", (char *)contents);
            return -1;
        }

        if (cJSON_IsTrue(cJSON_GetObjectItemCaseSensitive(root, "done"))) {
            role = cJSON_GetObjectItemCaseSensitive(message, "role");
            message_hist = cJSON_CreateObject();
            cJSON_AddItemToObject(message_hist, "role", cJSON_CreateString(role->valuestring));
            cJSON_AddItemToObject(message_hist, "content", cJSON_CreateString(chandle->content));
            cJSON_AddItemToArray(chandle->history, message_hist);
            chandle->content_len = 0;
            cJSON_Delete(root);
            printf("\n");
            return size * nmemb;
        }

        content_len = strlen(content->valuestring);
        if (chandle->content_len + content_len + 1 > chandle->content_size) {
            size_t newsize = (((chandle->content_len + content_len) / 4096) + 1) * 4096;
            char *ptr;
            ptr = realloc(chandle->content, newsize);
            if (!ptr) {
                printf("realloc error: %s\n", strerror(errno));
                free(chandle->content);
                chandle->content_len = 0;
                return -1;
            }
            chandle->content = ptr;
            chandle->content_size = newsize;
        }
        sprintf(&chandle->content[chandle->content_len], "%s", content->valuestring);
        chandle->content_len += content_len;
        printf("%s", content->valuestring);
        fflush(stdout);
    } else {
        printf("Contents: %s\nError Starts(%p): %s\n\n", (char *)contents, errptr, errptr);
    }
    return size * nmemb;
}

int test_chat(char *ollama_host, char *model) {

    ollama_t *ollama = ollama_init(ollama_host, NULL, 0);
    if (!ollama) {
        return -1;
    }

    cJSON *history = cJSON_CreateArray();
    cJSON *message;
    char *prompt, *msgstr;
    int ret;
    struct chat_handle chandle;
    
    memset(&chandle, 0, sizeof(chandle));
    chandle.history = history;
    
    prompt = "上海在哪里？";
    message = cJSON_CreateObject();
    if (!message) {
        return -1;
    }
    cJSON_AddItemToObject(message, "role", cJSON_CreateString("user"));
    cJSON_AddItemToObject(message, "content", cJSON_CreateString(prompt));
    cJSON_AddItemToArray(history, message);

    msgstr = cJSON_PrintUnformatted(history);
    printf(">>> %s\n", prompt);
    printf("<<< ");

    ollama_result_t res;
    res.type = OLLAMA_RESULT_TYPE_CALLBACK;
    res.call.callback = chat_callback;
    res.call.callback_handle = &chandle;

    ret = ollama_chat(ollama, model, msgstr, NULL, &res);
    free(msgstr);
    if (ret != 0) {
        return -1;
    }

    prompt = "那么苏州呢？";
    message = cJSON_CreateObject();
    if (!message) {
        return -1;
    }
    cJSON_AddItemToObject(message, "role", cJSON_CreateString("user"));
    cJSON_AddItemToObject(message, "content", cJSON_CreateString(prompt));
    cJSON_AddItemToArray(history, message);

    msgstr = cJSON_PrintUnformatted(history);
    printf(">>> %s\n", prompt);
    printf("<<< ");

    ret = ollama_chat(ollama, model, msgstr, NULL, &res);
    free(msgstr);
    if (ret != 0) {
        return -1;
    }

    cJSON_Delete(history);

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

    return test_chat(host, model);
}
