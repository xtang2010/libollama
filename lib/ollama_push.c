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

int ollama_push(ollama_t *ollama, char *model, int insecure, int stream, ollama_result_t *res)
{
    ollama_t *pollama;
    int n;

    if (!model || !(pollama = check_ollama(ollama))) {
        return -1;
    }

    char url_push[128];
    n = snprintf(url_push, 128, "%s/api/push", pollama->host);
    if (n >= 128) {
        return -1;
    }

    char data[128];
    n = snprintf(data, 128, "{ \
        \"model\": \"%s\", \
        \"insecure\": %s,  \
        \"stream\": %s     \
        }",                \
        model, insecure ? "true" : "false", stream ? "true" : "false");
    
    if (n >= 128) {
        return -1;
    }

    int status;
    status = _curl_post(pollama->curl, url_push, data, res);

    if (!ollama) {
        ollama_close(pollama);
    }

    return status == 200 ? 0 : -1;    
}