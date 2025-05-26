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
#include "ollama_priv.h"
#include <ollama/ollama.h>

#define BLKSIZE 2048

static size_t _curl_write_cb(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    ollama_result_t *res = (ollama_result_t *)userp;
    long copylen;

    if (!res) {
        return realsize;
    }

    /* if buf_len == 0， grow the receive buffer to hold total data */
    if (res->buf.len + realsize + 1 > res->buf.size) {
        /* if can't grow the buffer, then just skip the memcopy */
        if (res->type == OLLAMA_RESULT_TYPE_MEM) {
            copylen = res->buf.size - res->buf.len - 1;
            if (copylen > 0) {
                memcpy(&(res->buf.base[res->buf.len]), contents, copylen);
                res->buf.base[res->buf.len + copylen] = '\0';
            }
            res->buf.len += realsize;
            return realsize;
        } else {
            char *ptr;
            size_t newblk = (int)((res->buf.len + realsize + 1) / BLKSIZE + 1) * BLKSIZE;
            if (!(ptr = realloc(res->buf.base, newblk))) {
                free(res->buf.base);
                res->buf.base = NULL;
                return realsize;
            }
            res->buf.base = ptr;
            res->buf.size = newblk;
            copylen = realsize;
        }
    } else {
        copylen = realsize;
    }

    memcpy(&(res->buf.base[res->buf.len]), contents, copylen);
    res->buf.len += copylen;
    res->buf.base[res->buf.len] = '\0';

    return realsize;
}

int _curl_get(CURL *curl, char *url, ollama_result_t *res)
{
    curl_easy_setopt(curl, CURLOPT_URL, url);
    if (!res) {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _curl_write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
    } else {
        switch (res->type) {

        case OLLAMA_RESULT_TYPE_CALLBACK:
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, res->call.callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, res->call.callback_handle);
            break;

        case OLLAMA_RESULT_TYPE_MEM_DYNAMIC:
            res->buf.base = NULL;
            res->buf.len  = 0;
            res->buf.size = 0;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _curl_write_cb);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, res);
            break;
        
        case OLLAMA_RESULT_TYPE_MEM:
            res->buf.len  = 0;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _curl_write_cb);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, res);
            break;
        }
    }

    CURLcode rc;
    rc = curl_easy_perform(curl);
    if (rc != CURLE_OK && res && res->type == OLLAMA_RESULT_TYPE_MEM_DYNAMIC && res->buf.base) {
        free(res->buf.base);
        return -1;
    }

    int status;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);

    if (status != 200 && res && res->type == OLLAMA_RESULT_TYPE_MEM_DYNAMIC && res->buf.base) {
        free(res->buf.base);
    }

    return status;
}

int _curl_post(CURL *curl, char *url, char *data, ollama_result_t *res)
{
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    if (!res) {
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _curl_write_cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);
    } else {
        switch (res->type) {

        case OLLAMA_RESULT_TYPE_CALLBACK:
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, res->call.callback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, res->call.callback_handle);
            break;

        case OLLAMA_RESULT_TYPE_MEM_DYNAMIC:
            res->buf.base = NULL;
            res->buf.size = 0;
            // fall through
        
        case OLLAMA_RESULT_TYPE_MEM:
            res->buf.len  = 0;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _curl_write_cb);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, res);
            break;
        }
    }

    CURLcode rc;
    rc = curl_easy_perform(curl);
    if (rc != CURLE_OK && res && res->type == OLLAMA_RESULT_TYPE_MEM_DYNAMIC && res->buf.base) {
        free(res->buf.base);
        return -1;
    }

    int status;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);

    if (status != 200 && res && res->type == OLLAMA_RESULT_TYPE_MEM_DYNAMIC && res->buf.base) {
        free(res->buf.base);
    }

    return status;
}

ollama_t *check_ollama(ollama_t *ollama) {
    if (ollama) {
        curl_easy_reset(ollama->curl);
        return ollama;
    } else {
        return ollama_init(NULL, NULL, 0);
    }
}

char *jstring(char *jstr, ...) {
    size_t len;
    char *key, *value, *jfinal;
    char *cp;
    va_list args;
    
    if (!jstr) {
        len = 2;
    } else {
        len = strlen(jstr) + 2;
    }

    va_start(args, jstr);
    for (;;) {
        key = va_arg(args, char *);
        if (key == NULL) {
            break;
        }
        value = va_arg(args, char *);
        len += strlen(key) + 3 + strlen(value) + 3;
    }

    if ((jfinal = malloc(len)) == NULL) {
        return NULL;
    }

    jfinal[0] = '{';
    jfinal[1] = '\0';    

    va_start(args, jstr);
    for (;;) {
        key = va_arg(args, char *);
        if (key == NULL) {
            break;
        }
        value = va_arg(args, char *);
        len = strlen(jfinal);
        // skip white space in front
        for (cp = value; *cp; cp++) {
            if (*cp != ' ' || *cp != '\t' || *cp != '\n' || *cp != '\r') {
                break;
            }
        }
        if (*cp == '[' || *cp == '{') {
            sprintf(&jfinal[len], "\"%s\":%s,", key, cp);
        } else {
            sprintf(&jfinal[len], "\"%s\":\"%s\",", key, value);
        }
    }
    
    len = strlen(jfinal);
    if (!jstr) {
        jfinal[len - 1] = '}';
        return jfinal;
    }

    int p;
    cp = jstr;
    for (p = 0; *cp != '\0'; cp++, p++) {
        if (*cp == ' ' || *cp == '\t' || *cp == '\n' || *cp == '\r') {
            continue;
        }
        if (*cp =='{') {
            sprintf(&jfinal[len], "%s", &jstr[p + 1]);
            break;
        }
    }

    return jfinal;
}
