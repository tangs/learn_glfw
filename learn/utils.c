//
// Created by tangs on 2020/8/27.
//
#include <stdio.h>

int read_file(const char* path, char *bytes, unsigned int len) {
    FILE *file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "Can't open file:%s\n", path);
        return 1;
    }
    size_t n = fread(bytes, 1, len, file);
    if (!n) {
        return 2;
    }
    bytes[n] = 0;
    fclose(file);
//    printf("%s:%s\n", path, bytes);
    return 0;
}