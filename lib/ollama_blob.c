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

int ollama_blob(ollama_t *ollama, char *digest, char *file, ollama_result_t *res)
{
    ollama_t *pollama;
    int n;

    if (!(pollama = check_ollama(ollama))) {
        return -1;
    }

    char url_blob[512];
    n = snprintf(url_blob, 128, "%s/api/blob/%s", pollama->host, digest);
    if (n >= 512) {
        return -1;
    }

    FILE *fp = NULL;
    if (file) {
        size_t fsize;

        fp = fopen(file, "rb");
        if (!fp) {
            return -1;
        }
        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        /* setup for file upload */
        curl_easy_setopt(pollama->curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(pollama->curl, CURLOPT_READDATA, fp);
        curl_easy_setopt(pollama->curl, CURLOPT_INFILESIZE_LARGE, fsize);
    }

    int status;
    status = _curl_get(pollama->curl, url_blob, res);

    if (fp) {
        fclose(fp);
    }

    if (!ollama) {
        ollama_close(pollama);
    }

    return status == 200 ? 0 : -1;    
}