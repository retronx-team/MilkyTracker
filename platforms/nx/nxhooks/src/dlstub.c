#include <dlfcn.h>
#include <stddef.h>

void *dlopen(const char *filename, int flag) {
    return NULL;
}

char *dlerror(void) {
    return "nodl";
}

void *dlsym(void *handle, const char *symbol) {
    return NULL;
}

int dlclose(void *handle) {
    return -1;
}