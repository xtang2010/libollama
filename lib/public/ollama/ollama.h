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
#ifndef OLLAMAINC_OLLAMA_H
#define OLLAMAINC_OLLAMA_H

#include <string.h>
#include <curl/curl.h>

typedef struct ollama {
    char *host;
    CURL *curl;
    struct curl_slist *headers;

} ollama_t;

typedef size_t (ollama_cb_t)(void *contents, size_t size, size_t nmemb, void *userp);

typedef enum {
    OLLAMA_RESULT_TYPE_MEM,
    OLLAMA_RESULT_TYPE_MEM_DYNAMIC,
    OLLAMA_RESULT_TYPE_CALLBACK
} ollama_resut_type_t;

typedef union {
    ollama_resut_type_t type;
    struct {
        ollama_resut_type_t type;
        char  *base;
        size_t len;
        size_t size;
    } buf;
    struct {
        ollama_resut_type_t type;
        ollama_cb_t *callback;
        void *callback_handle;
    } call;
} ollama_result_t;

extern ollama_t *ollama_init(char *host, char **headers, int num_headers);
extern int ollama_close(ollama_t *ollama);
extern int ollama_blob(ollama_t *ollama, char *digest, char *file, ollama_result_t *res);
extern int ollama_generate(ollama_t *ollama, char *model, char *prompt, char *param_json, ollama_result_t *res);
extern int ollama_chat(ollama_t *ollama, char *model, char *message_json, char *param_json, ollama_result_t *res);
extern int ollama_create(ollama_t *ollama, char *mode, char *param_json, ollama_result_t *res);
extern int ollama_copy(ollama_t *ollama, char *smodel, char *dmodel, ollama_result_t *res);
extern int ollama_delete(ollama_t *ollama, char *model, ollama_result_t *res);
extern int ollama_load(ollama_t *ollama, char *model, ollama_result_t *res);
extern int ollama_unload(ollama_t *ollama, char *model, ollama_result_t *res);
extern int ollama_pull(ollama_t *ollama, char *model, int insecure, int stream, ollama_result_t *res); 
extern int ollama_push(ollama_t *ollama, char *model, int insecure, int stream, ollama_result_t *res); 
extern int ollama_embed(ollama_t *ollama, char *model, char *inputs, char *param_json, ollama_result_t *res); 
extern int ollama_list(ollama_t *ollama, ollama_result_t *res);
extern int ollama_show(ollama_t *ollama, char *model, int verbose, ollama_result_t *res);
extern int ollama_ps(ollama_t *ollama, ollama_result_t *res);

#endif /* OLLAMAINC_OLLAMA_H */