# libollama

The lightweight ollama library in C language.

## License

MIT License

>    Copyright (c) 2025 xtang
>
>    Permission is hereby granted, free of charge, to any person obtaining a copy
>    of this software and associated documentation files (the "Software"), to deal
>    in the Software without restriction, including without limitation the rights
>    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
>    copies of the Software, and to permit persons to whom the Software is
>    furnished to do so, subject to the following conditions:
>
>    The above copyright notice and this permission notice shall be included in all
>    copies or substantial portions of the Software.
>
>    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
>    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
>    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
>    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
>    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
>    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
>    SOFTWARE.

## Usage

### Welcome to libollama.

Ollama is an open sourced local LLM framework, it can help user to deploy, manage and run large language model in local environment. (https://ollama.com/)

Ollama  provide a REST API for applications to access the LLM server from remote. libollama is aimed to provide a C library implementation, for C programs could take advantage of Ollama.

The library is try to be simple and less dependency. It uses libcurl (https://curl.se/libcurl/) to do the http calls, and that's the only dependency. 

### Building

The code is tested to be built for Linux and QNX.

#### Build on QNX

On QNX, make sure you have the proper development environment setup (source qnxsdp-env.sh). For detail, please checkout https://www.qnx.com

```
make install
```

This will built the install the "libollama.so" and "libollamaS.a" for both "x86_64" and "aarch64". It also built test programs in test/nto/ directories for testing the library.

The "libollama.so" and "libollamaS.a" will then be nstalled  into $QNX_TARGET/$CPU/usr/local/lib, and the header file "ollama/ollama.h" will be installed in $QNX_TARGET/usr/local/include.

#### Build on Linux

A simple CMakeLists.txt is provide. Simply forllow below steps.

```
cd build
cmake ..
```

This will create a Makefile and a bunch of other files. You can then compile it:

```
make
```

You can also compiled the additional test code and run a test:

```
make test_all
make test
```

And install it with `make install` if you want. By default it installs the headers `/usr/local/include/cjson` and the libraries to `/usr/local/lib`.

### Use libollama in your program. 

libollama implemented all the ollama APIs, which explained here: https://github.com/ollama/ollama/blob/main/docs/api.md

```c
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
```

#### include the header file in your code

```c
#include <ollama/ollama.h>
```

#### Data structure ollama_t

You can use "ollama_init()" to allocate an "ollama_t" context, before you use any other libollama apis. This gives you a chance to specify ollama's URL, and any special http headers you want to use during all further calls. Remember to call ollama_close() to release the context.

```c

ollama_t *ollama = ollama_init("http://192.168.1.1", NULL, 0);
if (!ollama) {
	return -1;
}

...

ollama_close(ollama);
```

You can also skip the allocation of the context, simply pass in "NULL" when calling the APIs. In this case, the API will then create a context internally and use environment "OLLAMA_HOST" as the URL.

#### Data structure char *param_json

Most of the libollama API take a "char *param_json", this is the optional parameters you can pass in to control the LLM behavier in more detail. For example, you can pass below to ollama_chat() or ollama_generate() to switch them to "stream off" mode. 

```c
char *param_json = "{\"stream\": false}";

ollama_chat(ollama, model, messages, param.json, &res);
```

For more detail of what you can set as the parameters, please check ollama api document (https://github.com/ollama/ollama/blob/main/docs/api.md#generate-a-completion) 

#### Data structure ollama_result_t

All the API return an int as a result, where 0 indicate the call is successed, and other value indicating a failure. For more result, you need to pass in a "ollama_result_t *" to the API, to tell libollama how you wan the result back.

```c
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
```

There are multiple ways of request result, this decided by "ollama_reuslt_type_t type;"

##### OLLAMA_RESULT_TYPE_CALLBACK

This indicate you have prepared a callback, so you need to fill in the call structure before you pass the ollama_result_t to the API

```c
static size_t generate_callback(void *contents, size_t size, size_t nmemb, void *userp) {
	printf("contents: %s\n", contents);
	return size * nmemb;
}

ollama_result_t res;

res.type = OLLAMA_RESULT_TYPE_CALLBACK;
res.call.callback = generate_callback;
res.call.callback_handle = NULL;
```

The callback function essentially is a libcurl callback, it has a prototype like this:

```c
size_t ollama_callback(void *contents, size_t size, size_t nmemb, void *userp);
```

The "contents" is the return value of the http call, and the "userp" is the callback_handle you passed in. And size * nmemb is how much bytes is returned.

The callback method is specially useful, when you call "ollama_generate()" or "ollama_chat()" in "stream mode". (Default mode). Your callback will be called multiple times during stream mode, and returned token by token.

##### OLLAMA_RESULT_TYPE_MEM

This indicate you will provide a memory buffer, the call will store the result in the memory buffer.

```c
ollama_result_t res;
char buffer[4096];

res.type = OLLAMA_RESULT_TYPE_MEM;
res.buf.base = buffer;
res.buf.size = 4096;

if (ollama_ps(ollama, &res) == 0) {
	printf("Returned: %s\n", buffer);
}
```

##### OLLAMA_RESULT_TYPE_MEM_DYNAMIC

Sometime, the result is dynamic and hard to predict the size, in this case, you can pass in OLLAMA_RESULT_TYPE_MEM_DYNAMIC. This will let libollama to allocate memory internally, the result will returned in res.buf.base when the call sucessed. Just don't forget to free it after use it.

```c
ollama_result_t res;

res.type = OLLAMA_RESULT_TYPE_MEM_DYNAMIC;

if (ollama_show(ollama, "gemmi") == 0) {
	printf("Returned: %s\n", res.buf.base);
	free(res.buf.base);
}
```

#### compile and link

Compile your code that is using libollama, link it with -lollama, make sure you have the right Library search path.

### Samples

Check the source code in "test/" to see more advanced ways to use them.

# Enjoy!

