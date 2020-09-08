//
// Created by tangs on 2020/9/8.
//

#include "image_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

void image_loader_init() {
    stbi_set_flip_vertically_on_load(1);
}

unsigned char *image_loader_load (char const *filename, int *x, int *y, int *channels_in_file, int desired_channels) {
    return stbi_load(filename, x, y, channels_in_file, desired_channels);
}

void image_loader_image_free(unsigned char *data) {
    stbi_image_free(data);
}