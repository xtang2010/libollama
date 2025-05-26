/*******************************************************************************
 * Copyright (c) 2023 BlackBerry Limited. All Rights Reserved.
 *
 * You must obtain a written license from and pay applicable license fees to QNX
 * Software Systems before you may reproduce, modify or distribute this software,
 * or any work that includes all or part of this software.   Free development
 * licenses are available for evaluation and non-commercial purposes.  For more
 * information visit [http://licensing.qnx.com] or email licensing@qnx.com.
 *
 * This file may contain contributions from others.  Please review this entire
 * file for other proprietary rights or license notices, as well as the QNX
 * Development Suite License Guide at [http://licensing.qnx.com/license-guide/]
 * for other information.
 *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "ollama_priv.h"

ollama_t * ollama_init(char *host, char **headers, int num_headers)
{
    ollama_t *ollama;

    if ((ollama = malloc(sizeof(*ollama))) == NULL) {
        return NULL;
    }
    memset(ollama, 0, sizeof(*ollama));

    if (host == NULL) {
        host = getenv("OLLAMA_HOST");
        if (!host) {
            return NULL;
        }
    }

    if ((ollama->curl = curl_easy_init()) == NULL) {
        free(ollama);
        return NULL;
    }
    
    int i;
    struct curl_slist *item;

    for (i = 0; i < num_headers; i++) {
        item = curl_slist_append(ollama->headers, headers[i]);
        if (!item) {
            if (!ollama->headers)
                curl_slist_free_all(ollama->headers);
            curl_easy_cleanup(ollama->curl);
            free(ollama);
            return NULL;
        }    
        ollama->headers = item;
    }
    if (ollama->headers)
        curl_easy_setopt(ollama->curl, CURLOPT_HTTPHEADER, ollama->headers);

    ollama->host = strdup(host);
    return ollama;
}

